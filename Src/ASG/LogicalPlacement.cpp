#include "ASG.h"
#include <QDebug>
#include "Circuit/CircuitGraph.h"
#include "Matrix.h"
#include "MatrixElement.h"
#include "Circuit/Device.h"
#include "Circuit/Terminal.h"
#include "Circuit/Node.h"
#include "Level.h"

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
    switch (m_ignoreCap) {
        case IgnoreNoCap:
            return BubbleSortIgnoreNoCap();
        case IgnoreGCap:
            return BubbleSortIgnoreGCap();
        case IgnoreGCCap:
            return BubbleSortIgnoreGCCap();
        default:
            qInfo() << "UNknown ignore cap type " << m_ignoreCap << endl;
            EXIT;
    }
}

int ASG::BubbleSortIgnoreNoCap()
{
#ifdef TRACE
    qInfo() << LINE_INFO << endl;
#endif
    if (m_levels.size() <= 1)
        return OKAY;

    foreach (Device *dev, m_ckt->GetDeviceList()) {
        dev->ClearPredecessors();
        dev->ClearSuccessors();
    }

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
    int indexOfMaxDeviceCount = 0;
    Level *levelOfMaxDeviceCount = m_levels.front();
    int maxDeviceCountInLevel = levelOfMaxDeviceCount->AllDeviceCount();

    Level *level = nullptr;
    for (int i = 1; i < m_levels.size(); ++ i) {
        level = m_levels.at(i);
        if (level->AllDeviceCount() > maxDeviceCountInLevel) {
            maxDeviceCountInLevel = level->AllDeviceCount();
            indexOfMaxDeviceCount = i;
            levelOfMaxDeviceCount = level;
        }
    }

#ifdef DEBUG
    qInfo() << "max device count(" << maxDeviceCountInLevel << "), index("
            << indexOfMaxDeviceCount << ")" << endl;
#endif
    Q_ASSERT(levelOfMaxDeviceCount);

    /* Third, assign row number to indexOfMaxDeviceCount */
    int row = 0;
    foreach (Device *dev, levelOfMaxDeviceCount->AllDevices()) {
        dev->SetRow(row);
        dev->SetBubbleValue(row); // useless here
        row += Row_Device_Factor;
    }

    /* Fourth, assign row number to [0, indexOfMaxDeviceCount) */
    for (int i = indexOfMaxDeviceCount - 1; i >= 0; -- i) {
        level = m_levels.at(i);
        foreach (Device *dev, level->AllDevices())
            dev->CalBubbleValueBySuccessors(m_ignoreCap);
        level->AssignRowNumberByBubbleValue(m_ignoreCap);
    }

    /* Fifth, assign row number to (indexOfMaxDeviceCount, lastLevel] */
    for (int i = indexOfMaxDeviceCount + 1; i < m_levels.size(); ++ i) {
        level = m_levels.at(i);
        foreach (Device *dev, level->AllDevices())
            dev->CalBubbleValueByPredecessors(m_ignoreCap);
        level->AssignRowNumberByBubbleValue(m_ignoreCap);
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

int ASG::BubbleSortIgnoreGCap()
{
#ifdef TRACE
    qInfo() << LINE_INFO << endl;
#endif

    if (m_levels.size() <= 1)
        return OKAY;

    foreach (Device *dev, m_ckt->GetDeviceList()) {
        dev->ClearPredecessors();
        dev->ClearSuccessors();
    }

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

    /* Second, find indexofMaxDeviceCount ignore groundCap */
    int indexOfMaxDeviceCount = 0;
    Level *levelOfMaxDeviceCount = m_levels.front();
    int maxDeviceCountInLevel = levelOfMaxDeviceCount->DeviceCountWithoutGCap();

    Level *level = nullptr;
    for (int i = 1; i < m_levels.size(); ++ i) {
        level = m_levels.at(i);
        if (level->DeviceCountWithoutGCap() > maxDeviceCountInLevel) {
            maxDeviceCountInLevel = level->DeviceCountWithoutGCap();
            indexOfMaxDeviceCount = i;
            levelOfMaxDeviceCount = level;
        }
    }

#ifdef DEBUG
    qInfo() << "max device count without ground cap(" << maxDeviceCountInLevel
            << "), index(" << indexOfMaxDeviceCount << ")" << endl;
#endif

    /* Third, assign row number to indexOfMaxDeviceCount */
    int row = 0;
    foreach (Device *dev, levelOfMaxDeviceCount->AllDevices()) {
        if (dev->GroundCap()) {
            dev->SetRow(0);
            dev->SetBubbleValue(0);
            continue;
        }
        dev->SetRow(row);
        dev->SetBubbleValue(row);
        row += Row_Device_Factor;
    }

    /* Fourth, assign row number to [0, indexOfMaxDeviceCount) */
    for (int i = indexOfMaxDeviceCount - 1; i >= 0; -- i) {
        level = m_levels.at(i);
        foreach (Device *dev, level->AllDevices()) {
            if (dev->GroundCap()) continue;
            dev->CalBubbleValueBySuccessors(m_ignoreCap); // ignore ground cap
        }
        level->AssignRowNumberByBubbleValue(m_ignoreCap); // ignore ground cap
    }

    /* Fifth, assign row number to (indexOfMaxDeviceCount, lastLevel] */
    for (int i = indexOfMaxDeviceCount + 1; i < m_levels.size(); ++ i) {
        level = m_levels.at(i);
        foreach (Device *dev, level->AllDevices()) {
            if (dev->GroundCap()) continue;
            dev->CalBubbleValueByPredecessors(m_ignoreCap); // ignore ground cap
        }
        level->AssignRowNumberByBubbleValue(m_ignoreCap);   // ignore ground cap
    }

#ifdef DEBUG
    printf("-----------------------------------\n");
    QString tmp;
    foreach (Device *dev, m_ckt->GetDeviceList()) {
        tmp += (dev->Name() + " ");
        tmp += ("level(" + QString::number(dev->LevelId()) + "), ");
        tmp += ("row(" + QString::number(dev->Row()) + "), ");
        tmp += ("bubbleValue(" + QString::number(dev->BubbleValue()) + "), ");
        tmp += ("groundCap(" + QString::number(dev->GroundCap()) + ")");
        qDebug() << tmp;
        tmp = "";
    }
    printf("-----------------------------------\n");
#endif
    
    return OKAY;
}

int ASG::BubbleSortIgnoreGCCap()
{
#ifdef TRACE
    qInfo() << LINE_INFO << endl;
#endif

    if (m_levels.size() <= 1)
        return OKAY;

    foreach (Device *dev, m_ckt->GetDeviceList()) {
        dev->ClearPredecessors();
        dev->ClearSuccessors();
    }

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

    /* Second, find indexofMaxDeviceCount ignore coupled and groundCap */
    int indexOfMaxDeviceCount = 0;
    Level *levelOfMaxDeviceCount = m_levels.front();
    int maxDeviceCountInLevel = levelOfMaxDeviceCount->DeviceCountWithoutGCCap();

    Level *level = nullptr;
    for (int i = 1; i < m_levels.size(); ++ i) {
        level = m_levels.at(i);
        if (level->DeviceCountWithoutGCCap() > maxDeviceCountInLevel) {
            maxDeviceCountInLevel = level->DeviceCountWithoutGCCap();
            indexOfMaxDeviceCount = i;
            levelOfMaxDeviceCount = level;
        }
    }

#ifdef DEBUG
    qInfo() << "max device count without coupled and ground cap(" << maxDeviceCountInLevel
            << "), index(" << indexOfMaxDeviceCount << ")" << endl;
#endif

    /* Third, assign row number to indexOfMaxDeviceCount */
    int row = 0;
    foreach (Device *dev, levelOfMaxDeviceCount->AllDevices()) {
        if (dev->GroundCap() || dev->CoupledCap()) {
            dev->SetRow(0);
            dev->SetBubbleValue(0);
            continue;
        }
        dev->SetRow(row);
        dev->SetBubbleValue(row);
        row += Row_Device_Factor;
    }

    /* Fourth, assign row number to [0, indexOfMaxDeviceCount) */
    for (int i = indexOfMaxDeviceCount - 1; i >= 0; -- i) {
        level = m_levels.at(i);
        foreach (Device *dev, level->AllDevices()) {
            if (dev->GroundCap()) continue;
            if (dev->CoupledCap()) continue;
            dev->CalBubbleValueBySuccessors(m_ignoreCap); // ignore coupled and ground cap
        }
        level->AssignRowNumberByBubbleValue(m_ignoreCap); // ignore coupled and ground cap
    }

    /* Fifth, assign row number to (indexOfMaxDeviceCount, lastLevel] */
    for (int i = indexOfMaxDeviceCount + 1; i < m_levels.size(); ++ i) {
        level = m_levels.at(i);
        foreach (Device *dev, level->AllDevices()) {
            if (dev->GroundCap()) continue;
            if (dev->CoupledCap()) continue;
            dev->CalBubbleValueByPredecessors(m_ignoreCap); // ignore coupled and ground cap
        }
        level->AssignRowNumberByBubbleValue(m_ignoreCap);   // ignore coupled and ground cap
    }

#ifdef DEBUG
    printf("-----------------------------------\n");
    QString tmp;
    foreach (Device *dev, m_ckt->GetDeviceList()) {
        tmp += (dev->Name() + " ");
        tmp += ("level(" + QString::number(dev->LevelId()) + "), ");
        tmp += ("row(" + QString::number(dev->Row()) + "), ");
        tmp += ("bubbleValue(" + QString::number(dev->BubbleValue()) + "), ");
        tmp += ("groundCap(" + QString::number(dev->GroundCap()) + ")");
        qDebug() << tmp;
        tmp = "";
    }
    printf("-----------------------------------\n");
#endif
    
    return OKAY;
}