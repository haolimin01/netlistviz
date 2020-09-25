#include "ASG.h"
#include <QDebug>
#include "Circuit/CircuitGraph.h"
#include "Matrix.h"
#include "MatrixElement.h"
#include "Circuit/Device.h"
#include "Circuit/Terminal.h"
#include "Circuit/Node.h"
#include "Level.h"
#include "HyperLevel.h"

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
//     PlotHyperLevels(QObject::tr("After CalLogicalCol"));
// #endif
    
    error = CalLogicalRow();

#ifdef DEBUG
    PlotHyperLevels(QObject::tr("After CalLogicalRow"));
#endif

    error = AdjustHyperLevelInside();

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
    m_hyperLevels.clear();

    HyperLevel *hl = nullptr;
    int currLevelId = 0;
    /* hyper level 0 */
    hl = new HyperLevel(currLevelId);
    hl->AddDevices(m_ckt->FirstLevelDeviceList());

#ifdef DEBUG
    hl->PrintAllDevices();
#endif

    /* set first hyper level devices as visited */
    Device *device = nullptr;
    foreach (device, m_ckt->FirstLevelDeviceList())
        m_visited[device->Id()] = 1;
    
    int totalDeviceNumber = m_ckt->GetDeviceList().size();
    int currDeviceNumber = hl->AllDeviceCount();
    
    m_hyperLevels.push_back(hl);
    currLevelId++;

    /* the rest hyper levels */
    while (hl->AllDeviceCount() > 0) {
        hl = CreateNextHyperLevel(hl);
        if (hl->AllDeviceCount() == 0) break;
        hl->SetId(currLevelId);
        currLevelId++;
#ifdef DEBUG
        hl->PrintAllDevices();
#endif
        m_hyperLevels.push_back(hl);
        currDeviceNumber += hl->AllDeviceCount();
    }

#ifdef DEBUG
    qInfo() << LINE_INFO << "total device(" << totalDeviceNumber << "), "
            << "current device(" << currDeviceNumber << ")" << endl;
#endif

    return OKAY;
}

HyperLevel* ASG::CreateNextHyperLevel(HyperLevel *prevHyperLevel) const
{
    Q_ASSERT(prevHyperLevel);

    HyperLevel *nextHyperLevel = new HyperLevel();
    int id = 0;
    MatrixElement *element = nullptr;
    Device *device = nullptr;

    foreach (device, prevHyperLevel->AllDevices()) {
        id = device->Id();
        element = m_matrix->RowHead(id).head;
        while (element) {
            device = element->ToDevice();
            if (m_visited[device->Id()]) {
                element = element->NextInRow();
                continue;
            }
            nextHyperLevel->AddDevice(device);
            element = element->NextInRow();
            m_visited[device->Id()] = 1;
        }
    }

    return nextHyperLevel;
}

int ASG::CalLogicalRow()
{
    int error = ClassifyConnectDeviceByHyperLevel();
    if (error)
        return ERROR;

    error = BubbleSort();

    return error;
}

int ASG::ClassifyConnectDeviceByHyperLevel()
{
    foreach (Device *dev, m_ckt->GetDeviceList())
        dev->ClassifyConnectDeviceByHyperLevel();

#ifdef DEBUG
    foreach (HyperLevel *hl, m_hyperLevels)
        hl->PrintAllConnections();
#endif
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

    /* First, find indexOfMaxDeviceCount */
    int indexOfMaxDeviceCount = 0;
    HyperLevel *levelOfMaxDeviceCount = m_hyperLevels.front();
    int maxDeviceCountInLevel = levelOfMaxDeviceCount->AllDeviceCount();

    HyperLevel *hl = nullptr;
    for (int i = 1; i < m_hyperLevels.size(); ++ i) {
        hl = m_hyperLevels.at(i);
        if (hl->AllDeviceCount() > maxDeviceCountInLevel) {
            maxDeviceCountInLevel = hl->AllDeviceCount();
            indexOfMaxDeviceCount = i;
            levelOfMaxDeviceCount = hl;
        }
    }

#ifdef DEBUG
    qInfo() << "max device count(" << maxDeviceCountInLevel << "), index("
            << indexOfMaxDeviceCount << ")" << endl;
#endif
    Q_ASSERT(levelOfMaxDeviceCount);

    /* Second, assign row number to indexOfMaxDeviceCount */
    int row = 0;
    foreach (Device *dev, levelOfMaxDeviceCount->AllDevices()) {
        dev->SetLogicalRow(row);
        dev->SetBubbleValue(row); // useless here
        row += Row_Device_Factor;
    }

    /* Third, assign row number to [0, indexOfMaxDeviceCount) */
    for (int i = indexOfMaxDeviceCount - 1; i >= 0; -- i) {
        hl = m_hyperLevels.at(i);
        foreach (Device *dev, hl->AllDevices())
            dev->CalBubbleValueBySuccessors(m_ignoreCap);
        hl->AssignRowNumberByBubbleValue(m_ignoreCap);
    }

    /* Fourth, assign row number to (indexOfMaxDeviceCount, lastLevel] */
    for (int i = indexOfMaxDeviceCount + 1; i < m_hyperLevels.size(); ++ i) {
        hl = m_hyperLevels.at(i);
        foreach (Device *dev, hl->AllDevices())
            dev->CalBubbleValueByPredecessors(m_ignoreCap);
        hl->AssignRowNumberByBubbleValue(m_ignoreCap);
    }

#ifdef DEBUG
    printf("-----------------------------------\n");
    QString tmp;
    foreach (Device *dev, m_ckt->GetDeviceList()) {
        tmp += (dev->Name() + " ");
        tmp += ("logicalCol(" + QString::number(dev->LogicalCol()) + "), ");
        tmp += ("logicalRow(" + QString::number(dev->LogicalRow()) + "), ");
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

    return OKAY;
}

int ASG::BubbleSortIgnoreGCCap()
{
#ifdef TRACE
    qInfo() << LINE_INFO << endl;
#endif

    return OKAY;
}

/* decide device orientation in this function */
int ASG::AdjustHyperLevelInside()
{
    foreach (HyperLevel *hl, m_hyperLevels)
        hl->Adjust();

#ifdef DEBUG
    foreach (HyperLevel *hl, m_hyperLevels)
        hl->PrintAdjustment();
#endif

    return OKAY;
}
