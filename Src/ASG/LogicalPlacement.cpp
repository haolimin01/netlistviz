#include "ASG.h"
#include <QDebug>
#include <QtMath>
#include <QTime>
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
    
#if 0
    /* Simulated Annealing */
    error = DetermineReferHyperLevelLogicalRow();
    if (error)
        return ERROR;
#endif

    error = DetermineFirstHyperLevelLogicalRow();
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

    return OKAY;
}

int ASG::DetermineFirstHyperLevelLogicalRow()
{
    /* 1. Find the max device count hyper level */
    HyperLevel *levelOfMaxDeviceCount = m_hyperLevels.front();
    int maxDeviceCountInLevel = levelOfMaxDeviceCount->AllDeviceCount();
    HyperLevel *hl = nullptr;

    for (int i = 1; i < m_hyperLevels.size(); ++ i) {
        hl = m_hyperLevels.at(i);
        if (hl->AllDeviceCount() > maxDeviceCountInLevel) {
            maxDeviceCountInLevel = hl->AllDeviceCount();
            levelOfMaxDeviceCount = hl;
        }
    }

    /* 2. Initial this hyper level logical row */
    int row = 0;
    foreach (Device *dev, levelOfMaxDeviceCount->AllDevices()) {
        dev->SetLogicalRow(row);
        dev->SetBubbleValue(row); // useless here
        row += Row_Device_Factor;
    }

#ifdef DEBUG
    qInfo() << "MaxDeviceHyperLevel "
            << "index(" << levelOfMaxDeviceCount->Id() << "), "
            << "deviceCount(" << levelOfMaxDeviceCount->AllDeviceCount() << ")";
#endif

    /* 3. Back Propagation */
    int thisIndex = levelOfMaxDeviceCount->Id();
    for (int i = thisIndex - 1; i >= 0; -- i) {
        hl = m_hyperLevels.at(i);
        foreach (Device *dev, hl->AllDevices())
            dev->CalBubbleValueBySuccessors(m_ignoreCap);
        hl->AssignRowNumberByBubbleValue(m_ignoreCap);
    }

    /* 4. Determine the First HyperLevel logical row */
    HyperLevel *firstHyperLevel = m_hyperLevels.front();
    int minRow = firstHyperLevel->MinLogicalRow();
    int currRow = minRow;
    foreach (Device *dev, firstHyperLevel->AllDevices()) {
        dev->SetLogicalRow(currRow);
        currRow += Row_Device_Factor;
    }

#ifdef DEBUG
    qInfo() << "First Hyper Level Logical Row";
    foreach (Device *dev, firstHyperLevel->AllDevices()) {
        qInfo() << dev->Name() << dev->LogicalRow();
    }
#endif

    return OKAY;
}

int ASG::DetermineReferHyperLevelLogicalRow()
{
    /* 1. find the refer hyper level */
    m_referHyperLevel = nullptr;
    HyperLevel *levelOfMaxDeviceCount = m_hyperLevels.front();
    int maxDeviceCountInLevel = levelOfMaxDeviceCount->AllDeviceCount();
    HyperLevel *hl = nullptr;

    for (int i = 1; i < m_hyperLevels.size(); ++ i) {
        hl = m_hyperLevels.at(i);
        if (hl->AllDeviceCount() > maxDeviceCountInLevel) {
            maxDeviceCountInLevel = hl->AllDeviceCount();
            levelOfMaxDeviceCount = hl;
        }
    }

    m_referHyperLevel = levelOfMaxDeviceCount;
    Q_ASSERT(m_referHyperLevel);
    /* Initial refer hyper level logical row */
    int row = 0;
    foreach (Device *dev, m_referHyperLevel->AllDevices()) {
        dev->SetLogicalRow(row);
        dev->SetBubbleValue(row); // useless here
        row += Row_Device_Factor;
    }

#ifdef DEBUG
    qInfo() << LINE_INFO << "HyperLevel "
            << "index(" << QString::number(m_referHyperLevel->Id()) << "), "
            << "deviceCount(" << QString::number(m_referHyperLevel->AllDeviceCount()) << ")";
#endif
    int referId = m_referHyperLevel->Id();
    HyperLevel *prevHyperLevel = nullptr, *nextHyperLevel = nullptr;
    if (referId >= 1)
        prevHyperLevel = m_hyperLevels.at(referId - 1);
    if (referId < m_hyperLevels.size() - 1)
        nextHyperLevel = m_hyperLevels.at(referId + 1);


    double initT = SA_INIT_T;
    double endT = SA_END_T;
    double alpha = SA_ALPHA;
    double oldCost = 0, currCost = 0;

    /* random number seed */
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
    // qsrand(QTime::currentTime().second());

    currCost = SACalCost(prevHyperLevel, m_referHyperLevel, nextHyperLevel);
    oldCost = currCost;

    double currT = initT;
    double dCost = 0;
    double P = 0;
    int iter = 0;
    double randF = 0;
    QVector<int> rowResult;

    foreach (Device *dev, m_referHyperLevel->AllDevices())
        rowResult.push_back(dev->LogicalRow());

    bool accept = true;

    while (currT > endT) {

        SAExchangeLogicalRow(m_referHyperLevel); 

        currCost = SACalCost(prevHyperLevel, m_referHyperLevel, nextHyperLevel);
        dCost = currCost - oldCost;

        if (dCost < 0)
            accept = true;
        else {
            P = qExp(- dCost / currT);
            randF = qrand() % 10000 / 10000.0;

            if (P > randF)
                accept = true;
            else
                accept = false;
        }

        if (accept) {
            oldCost = currCost;
            rowResult.clear();
            foreach (Device *dev, m_referHyperLevel->AllDevices())
                rowResult.push_back(dev->LogicalRow());
        }
        
#ifdef DEBUG
        qInfo() << "iter(" << iter << "), "
                << "oldCost(" << oldCost << "), "
                << "currCost(" << currCost << ")";
        qInfo() << "currT(" << currT << "), "
                << "P(" << P << "), "
                << "randF(" << randF << ")";
        qInfo() << rowResult;
#endif
        currT *= alpha;
        iter++;
    }

    return OKAY;
}

double ASG::SACalCost(HyperLevel *prev, HyperLevel *curr, HyperLevel *next)
{
    if (NOT curr)
        return 0;

    double wireCost = 0;
    double crossCost = 0;

    if (prev) {
        foreach (Device *dev, prev->AllDevices()) {
            dev->CalBubbleValueBySuccessors(m_ignoreCap);
        }
        prev->AssignRowNumberByBubbleValue(m_ignoreCap);
    }

    if (next) {
        foreach (Device *dev, next->AllDevices()) {
            dev->CalBubbleValueByPredecessors(m_ignoreCap);
        }
        next->AssignRowNumberByBubbleValue(m_ignoreCap);
    }

    Device *thisDev = nullptr, *otherDev = nullptr;
    int thisRow = 0, thisCol = 0;
    int otherRow = 0, otherCol = 0;
    QVector<QPair<int, int>> rowPairs;

    foreach (thisDev, curr->AllDevices()) {
        foreach (otherDev, thisDev->Predecessors()) {
            thisRow = thisDev->LogicalRow();
            thisCol = thisDev->LogicalCol();
            otherRow = otherDev->LogicalRow();
            otherCol = otherDev->LogicalCol();
            wireCost += (qFabs(thisRow - otherRow) + qFabs(thisCol - otherCol));
            // wireCost = qSqrt(qPow(thisRow - otherRow, 2) + qPow(thisCol - otherCol, 2));
            rowPairs.push_back(qMakePair(thisRow, otherRow));
        }
    }
    crossCost += WireCrossCount(rowPairs);

    foreach (thisDev, curr->AllDevices()) {
        foreach (otherDev, thisDev->Successors()) {
            thisRow = thisDev->LogicalRow();
            thisCol = thisDev->LogicalCol();
            otherRow = otherDev->LogicalRow();
            otherCol = otherDev->LogicalCol();
            wireCost += (qFabs(thisRow - otherRow) + qFabs(thisCol - otherCol));
            // wireCost = qSqrt(qPow(thisRow - otherRow, 2) + qPow(thisCol - otherCol, 2));
            rowPairs.push_back(qMakePair(thisRow, otherRow));
        }
    }
    crossCost += WireCrossCount(rowPairs);

    double totalCost = 0.5 * wireCost + 0.5 * crossCost;

#ifdef DEBUGx
    QString tmp;
    foreach (Device *dev, curr->AllDevices())
        tmp += (QString::number(dev->LogicalRow()) + " ");
    qInfo() << tmp << " totalCost " << totalCost;
#endif

    return totalCost;
}

int ASG::WireCrossCount(const QVector<QPair<int, int>> &rowPairs)
{
    int count = 0;
    QPair<int, int> outPair, inPair;
    for (int i = 0; i < rowPairs.size(); ++ i) {
        outPair = rowPairs.at(i);
        for (int j = i + 1; j < rowPairs.size(); ++ j) {
            inPair = rowPairs.at(j);
            if ((outPair.first - inPair.first) * (outPair.second - inPair.second) < 0)
                count++;
        }
    }

    return count;
}

int ASG::SAExchangeLogicalRow(HyperLevel *curr)
{
#ifdef TRACE
    qInfo() << LINE_INFO << endl;
#endif

    if (NOT curr)
        return OKAY;

    if (curr->AllDeviceCount() < 1)
        return OKAY;

    int size = curr->AllDeviceCount();
    int index1 = qrand() % size;
    int index2 = qrand() % size;
    while (index2 == index1)
        index2 = qrand() % size;
    
    Device *dev1 = curr->AllDevices().at(index1);
    Device *dev2 = curr->AllDevices().at(index2);

    int dev1Row = dev1->LogicalRow();
    int dev2Row = dev2->LogicalRow();
    dev1->SetLogicalRow(dev2Row);
    dev2->SetLogicalRow(dev1Row);

#ifdef DEBUGx
    qInfo() << dev1->Name() << "original row(" << QString::number(dev1Row) << "), "
            << "new row(" << QString::number(dev2Row) << ")";
    qInfo() << dev2->Name() << "original row(" << QString::number(dev2Row) << "), "
            << "new row(" << QString::number(dev1Row) << ")";
#endif

    return OKAY;
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

#if 0
    Q_ASSERT(m_referHyperLevel);

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
#endif

#if 0 // (Simulated Annealing)
    int referIndex = m_referHyperLevel->Id();
    HyperLevel *hl = nullptr;

    /* Third, assign row number to [0, referIndex) */
    for (int i = referIndex - 1; i >= 0; -- i) {
        hl = m_hyperLevels.at(i);
        foreach (Device *dev, hl->AllDevices())
            dev->CalBubbleValueBySuccessors(m_ignoreCap);
        hl->AssignRowNumberByBubbleValue(m_ignoreCap);
    }

    /* Fourth, assign row number to (referIndex, lastLevel] */
    for (int i = referIndex + 1; i < m_hyperLevels.size(); ++ i) {
        hl = m_hyperLevels.at(i);
        foreach (Device *dev, hl->AllDevices())
            dev->CalBubbleValueByPredecessors(m_ignoreCap);
        hl->AssignRowNumberByBubbleValue(m_ignoreCap);
    }
#endif

    /* Forward Propagation by First HyperLevel */
    HyperLevel *hl = nullptr;
    for (int i = 1; i < m_hyperLevels.size(); ++ i) {
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
