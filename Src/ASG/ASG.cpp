#include "ASG.h"
#include <QDebug>
#include <QString>
#include "Matrix.h"
#include "MatrixElement.h"
#include "TablePlotter.h"
#include "Circuit/CircuitGraph.h"
#include "Circuit/Device.h"
#include "Circuit/Terminal.h"
#include "Circuit/Node.h"
#include "Level.h"
#include "Channel.h"
#include "Utilities/MyString.h"
#include "Schematic/SchematicDevice.h"
#include "Schematic/SchematicTerminal.h"
#include "Schematic/SchematicScene.h"


ASG::ASG(CircuitGraph *ckt)
{
    Q_ASSERT(ckt);
    m_ckt = ckt;
    m_matrix = nullptr;
    m_levelsPlotter = nullptr;

    m_visited = new int[m_ckt->DeviceCount()];  
    memset(m_visited, 0, sizeof(int) * m_ckt->DeviceCount());
}

ASG::ASG()
{
    m_ckt = nullptr;
    m_matrix = nullptr;
    m_visited = nullptr;
    m_levelsPlotter = nullptr;
}

ASG::~ASG()
{
#ifdef TRACE
    qInfo() << LINE_INFO << endl;
#endif
    if (m_matrix) delete m_matrix;
    delete []m_visited;
    
    foreach (Level *level, m_levels)
        delete level;
    m_levels.clear();

    if (m_levelsPlotter)
        delete m_levelsPlotter;

    foreach (Channel *channel, m_channels)
        delete channel;
    m_channels.clear();
}

void ASG::SetCircuitgraph(CircuitGraph *ckt)
{
    Q_ASSERT(ckt);
    m_ckt = ckt;

    if (m_matrix) {
        delete m_matrix;
        m_matrix = nullptr;
    }

    if (m_visited) delete []m_visited;
    m_visited = new int[m_ckt->DeviceCount()];
    memset(m_visited, 0, sizeof(int) * m_ckt->DeviceCount());
}

int ASG::LogicalPlacement()
{
#ifdef TRACE
    qInfo() << LINE_INFO << endl;
#endif
    int error = BuildIncidenceMatrix();
    if (error)
        return ERROR;
    
    error = CalLogicalCol();
    if (error)
        return ERROR;

// #ifdef DEBUG
//     PlotLevels(QObject::tr("After CalLogicalCol"));
// #endif
    
    error = CalLogicalRow();

#ifdef DEBUG
    PlotLevels(QObject::tr("After CalLogicalRow"));
#endif

    return error;
}

int ASG::LogicalRouting()
{
#ifdef TRACE
    qInfo() << LINE_INFO << endl;
#endif

    int error = Createchannels();
    if (error)
        return ERROR;
    
    error = AssignTrackNumber();
    if (error)
        return ERROR;

#ifdef DEBUG
    foreach (Channel *ch, m_channels)
        ch->Print();
#endif

    return OKAY;
}

int ASG::GeometricalPlacement(SchematicScene *scene)
{
#ifdef TRACE
    qInfo() << LINE_INFO << endl;
#endif
    int error = DecideDeviceWhetherToReverse();
    if (error)
        return ERROR;

    error = CreateSchematicDevices();
    if (error)
        return ERROR;

    error = RenderSchematicDevices(scene);
    if (error)
        return ERROR;

    return OKAY;
}

int ASG::GeometricalRouting()
{
#ifdef TRACE
    qInfo() << LINE_INFO << endl;
#endif

    return OKAY;
}

int ASG::BuildIncidenceMatrix()
{
    if (m_matrix) delete m_matrix;
    int size = m_ckt->DeviceCount();
    m_matrix = new Matrix(size);

    /* Row and Col header */
    int row = 0, col = 0;
    Device *device = nullptr;
    foreach (device, m_ckt->GetDeviceList()) {
        row = device->Id();
        col = device->Id();
        m_matrix->SetRowHeadDevice(row, device);
        m_matrix->SetColHeadDevice(col, device);
    }

    /* insert matrix elements */
    foreach (device, m_ckt->GetDeviceList()) {
        switch (device->GetDeviceType()) {
            case RESISTOR:
            case CAPACITOR:
            case INDUCTOR:
            case ISRC:
            case VSRC:
                InsertBasicDevice(device);
                break;
            default:;
        }
    }
#ifdef DEBUG
    m_matrix->Print();
    m_matrix->Plot();
#endif
    return OKAY;
}

/* R, L, C, V, I */
/* undirected graph */
int ASG::InsertBasicDevice(Device *device)
{
    /* device is from device */
    Node *posNode = device->GetTerminal(Positive)->GetNode();
    Node *negNode = device->GetTerminal(Negative)->GetNode();
    /* to device */
    Device *toDev = nullptr;
    int row = 0, col = 0;

    Q_ASSERT(posNode && negNode);

    if (NOT posNode->IsGnd()) {
        foreach (toDev, posNode->ConnectDeviceList()) {
            row = device->Id();
            col = toDev->Id();
            if (row == col) continue;
            Terminal *toTer = toDev->GetTerminal(posNode);
            Q_ASSERT(toTer);
            m_matrix->InsertElement(row, col, device, device->GetTerminal(Positive), toDev, toTer); 
        }
    }

    if (NOT negNode->IsGnd()) {
        foreach (toDev, negNode->ConnectDeviceList()) {
            row = device->Id();
            col = toDev->Id();
            if (row == col) continue;
            Terminal *toTer = toDev->GetTerminal(negNode);
            Q_ASSERT(toTer);
            m_matrix->InsertElement(row, col, device, device->GetTerminal(Negative), toDev, toTer);
        }
    }

    return OKAY;
}

int ASG::CalLogicalCol()
{
    /* set all device unvisited */
    memset(m_visited, 0, sizeof(int) * m_ckt->DeviceCount());
    m_levels.clear();

    Level *level = nullptr;
    int currLevelId = 0;
    /* level 0 */
    level = new Level(currLevelId);
    level->AddDevices(m_ckt->FirstLevelDeviceList());

#ifdef DEBUG
    level->PrintAllDevices();
#endif

    /* set first level devices as visited */
    Device *device = nullptr;
    foreach (device, m_ckt->FirstLevelDeviceList())
        m_visited[device->Id()] = 1;
    
    int totalDeviceNumber = m_ckt->GetDeviceList().size();
    int currDeviceNumber = level->AllDeviceCount();
    
    m_levels.push_back(level);
    currLevelId++;

    /* the rest levels */
    while (level->AllDeviceCount() > 0) {
        level = CreateNextLevel(level);
        if (level->AllDeviceCount() == 0) break;
        level->SetId(currLevelId);
        currLevelId++;
#ifdef DEBUG
        level->PrintAllDevices();
#endif
        m_levels.push_back(level);
        currDeviceNumber += level->AllDeviceCount();
    }

#ifdef DEBUG
    qInfo() << LINE_INFO << "total device(" << totalDeviceNumber << "), "
            << "current device(" << currDeviceNumber << ")" << endl;
#endif

    return OKAY;
}

Level* ASG::CreateNextLevel(Level *prevLevel) const
{
    Q_ASSERT(prevLevel);

    Level *nextLevel = new Level();
    int id = 0;
    MatrixElement *element = nullptr;
    Device *device = nullptr;

    foreach (device, prevLevel->AllDevices()) {
        id = device->Id();
        element = m_matrix->RowHead(id).head;
        while (element) {
            device = element->ToDevice();
            if (m_visited[device->Id()]) {
                element = element->NextInRow();
                continue;
            }
            nextLevel->AddDevice(device);
            element = element->NextInRow();
            m_visited[device->Id()] = 1;
        }
    }

    return nextLevel;
}

int ASG::CalLogicalRow()
{
    return BubbleSort();
}

int ASG::BubbleSort()
{
    return BubbleSortConsiderCap();
}

int ASG::BubbleSortConsiderCap()
{
#ifdef TRACE
    qInfo() << LINE_INFO << endl;
#endif
    if (m_levels.size() <= 1)
        return OKAY;

    /* First, create device connections */
    Level *frontLevel = m_levels.front();
    Level *rearLevel = nullptr;
    Device *frontDev = nullptr, *rearDev = nullptr;

    for (int i = 1; i < m_levels.size(); ++ i) {
        rearLevel = m_levels.at(i);
        foreach (frontDev, frontLevel->AllDevices()) {
            foreach (rearDev, rearLevel->AllDevices()) {
                if (frontDev->HasConnectionIgnoreGnd(rearDev)) {
                    frontDev->AddSuccessor(rearDev);
                    rearDev->AddPredecessor(frontDev);
                }
            }
        }
        frontLevel = rearLevel;
    }
#ifdef DEBUG
    foreach (Level *level, m_levels)
        level->PrintAllConnections();
#endif

    /* Second, find indexOfMaxDeviceCount */
    int maxDeviceCountInLevel = -1;
    int indexOfMaxDeviceCount = -1;
    Level *levelOfMaxDeviceCount = nullptr;
    Level *level = nullptr;
    for (int i = 0; i < m_levels.size(); ++ i) {
        level = m_levels.at(i);
        if (level->AllDeviceCount() > maxDeviceCountInLevel) {
            maxDeviceCountInLevel = level->AllDeviceCount();
            indexOfMaxDeviceCount = i;
            levelOfMaxDeviceCount = level;
        }
    }

    qInfo() << "max device count(" << maxDeviceCountInLevel << "), index("
            << indexOfMaxDeviceCount << ")" << endl;
    
    Q_ASSERT(levelOfMaxDeviceCount);

    /* Third, assign row number to indexOfMaxDeviceCount */
    int row = 0;
    foreach (Device *dev, levelOfMaxDeviceCount->AllDevices()) {
        dev->SetRow(row);
        dev->SetBubbleValue(row); // useless
        row += Row_Device_Factor;
    }

    /* Fourth, assign row number to [0, indexOfMaxDeviceCount) */
    for (int i = indexOfMaxDeviceCount - 1; i >= 0; -- i) {
        level = m_levels.at(i);
        foreach (Device *dev, level->AllDevices())
            dev->CalBubbleValueBySuccessors();
        level->AssignRowNumberByBubbleValue();
    }

    /* Fifth, assign row number to (indexOfMaxDeviceCount, lastLevel] */
    for (int i = indexOfMaxDeviceCount + 1; i < m_levels.size(); ++ i) {
        level = m_levels.at(i);
        foreach (Device *dev, level->AllDevices())
            dev->CalBubbleValueByPredecessors();
        level->AssignRowNumberByBubbleValue();
    }

#ifdef DEBUG
    printf("-----------------------------------\n");
    QString tmp;
    foreach (Device *dev, m_ckt->GetDeviceList()) {
        tmp += (dev->Name() + " ");
        tmp += ("level(" + QString::number(dev->LevelId()) + "), ");
        tmp += ("row(" + QString::number(dev->Row()) + "), ");
        tmp += ("bubbleValue(" + QString::number(dev->BubbleValue()) + ")");
        qDebug() << tmp;
        tmp = "";
    }
    printf("-----------------------------------\n");
#endif

    return OKAY;
}

int ASG::BubbleSortIgnoreCap()
{
#ifdef TRACE
    qInfo() << LINE_INFO << endl;
#endif

    return OKAY;
}

/*
 * Level0     Level1     Level2  ...  Level(n)
 *         |          |          |
 *     Channel0   channel1  Channel(n-1)
 */
int ASG::Createchannels()
{
#ifdef TRACE
    qInfo() << LINE_INFO << endl;
#endif

    m_channels.clear();

    if (m_levels.size() < 2)
        return OKAY;

    Level *level = nullptr;
    int channelIndex = 0;
    Channel *newChannel = nullptr;
    Device *dev = nullptr;

    for (int i = 1; i < m_levels.size(); ++ i) {
        level = m_levels.at(i);

        // create channel and wires
        newChannel = new Channel(channelIndex);

        foreach (dev, level->AllDevices()) {
            newChannel->AddWires(dev->WiresFromPredecessors());
        }

        m_channels.push_back(newChannel);
        channelIndex++;
    }


    return OKAY;
}

int ASG::AssignTrackNumber()
{
#ifdef TRACE
    qInfo() << LINE_INFO << endl;
#endif

    foreach (Channel *ch, m_channels) {
        ch->AssignTrackNumber();
    }

    return OKAY;
}

int ASG::DecideDeviceWhetherToReverse()
{
    /* First, set maxDeviceCount Level as no reverse */
    int maxDeviceCountInLevel = -1;
    int indexOfMaxDeviceCount = -1;
    Level *levelOfMaxDeviceCount = nullptr;
    Level *level = nullptr;
    for (int i = 0; i < m_levels.size(); ++ i) {
        level = m_levels.at(i);
        if (level->AllDeviceCount() > maxDeviceCountInLevel) {
            maxDeviceCountInLevel = level->AllDeviceCount();
            indexOfMaxDeviceCount = i;
            levelOfMaxDeviceCount = level;
        }
    }

    foreach (Device *dev, level->AllDevices()) {
        dev->SetReverse(false);
    }

    /* Second, deal with [0, indexOfMaxDeviceCount) */
    for (int i = indexOfMaxDeviceCount - 1; i >= 0; -- i) {
        level = m_levels.at(i);
        foreach (Device *dev, level->AllDevices())
            dev->DecideReverseBySuccessors();
    }

    /* Third, deal with (indexOfMaxDeviceCount, last] */
    for (int i = indexOfMaxDeviceCount + 1; i < m_levels.size(); ++ i) {
        level = m_levels.at(i);
        foreach (Device *dev, level->AllDevices())
            dev->DecideReverseByPredecessors();
    }

#ifdef DEBUG
    qDebug() << "--------------- Device Reverse ---------------";
    foreach (Device *dev, m_ckt->GetDeviceList()) {
        qDebug() << dev->Name() << " reverse(" << dev->Reverse() << ")";
    }
    qDebug() << "----------------------------------------------";
#endif

    return OKAY;
}

int ASG::CreateSchematicDevices()
{
    SchematicDevice *sdevice = nullptr;
    SchematicTerminal *sterminal = nullptr;
    Device *device = nullptr;
    Terminal *terminal = nullptr;

    foreach (device, m_ckt->GetDeviceList()) {
        sdevice = new SchematicDevice(device);
        foreach (terminal, device->GetTerminalList()) {
            sterminal = new SchematicTerminal(terminal);
            sdevice->AddTerminal(sterminal->GetTerminalType(), sterminal);
        }
        /* Initialize schematicDevice (Draw device shape, set annotation and tterminals pos) */
        sdevice->Initialize();
        m_sdeviceList.push_back(sdevice);
    }

#ifdef DEBUG
    foreach (sdevice, m_sdeviceList)
        sdevice->Print();
#endif

    return OKAY;
}

int ASG::RenderSchematicDevices(SchematicScene *scene)
{
    Q_ASSERT(scene);

    int levelCount = m_levels.size();
    int maxDeviceCountInLevel = -1;
    foreach (Level *level, m_levels) {
        if (level->AllDeviceCount() > maxDeviceCountInLevel) {
            maxDeviceCountInLevel = level->AllDeviceCount();
        }
    }

    int rowCount = maxDeviceCountInLevel * Row_Device_Factor; 
    int colCount = levelCount * 2; // levels + channels

    int error = scene->RenderSchematicDevices(m_sdeviceList, colCount, rowCount);
    return error;
}

/* Print and Plot */
void ASG::PlotLevels(const QString &title)
{
    if (m_levels.size() < 1)
        return;
    
    if (m_levelsPlotter) {
        m_levelsPlotter->close();
        m_levelsPlotter->clear();
    } else {
        m_levelsPlotter = new TablePlotter();
    }

    int maxDeviceCountInLevel = -1;
    foreach (Level *level, m_levels) {
        if (level->AllDeviceCount() > maxDeviceCountInLevel)
            maxDeviceCountInLevel = level->AllDeviceCount();
    }

    m_levelsPlotter->SetTableRowColCount(maxDeviceCountInLevel, m_levels.size());

    /* header */
    QStringList headerText;
    for (int i = 0; i < m_levels.size(); ++ i) {
        QString tmp = "L" + QString::number(m_levels.at(i)->Id());
        headerText << tmp;
    }
    m_levelsPlotter->SetColHeaderText(headerText);

    /* content */
    int row = 0;
    for (int i = 0; i < m_levels.size(); ++ i) {
        Level *level = m_levels.at(i);
        row = 0;
        foreach (Device *dev, level->AllDevices()) {
            m_levelsPlotter->AddItem(row, i, dev->Name());
            row++;
        }
    }

    m_levelsPlotter->setWindowTitle(title);
    m_levelsPlotter->Display();
}
