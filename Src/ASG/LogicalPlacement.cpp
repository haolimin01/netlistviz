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

    error = ClassifyConnectDeviceByLevel();
    if (error)
        return ERROR;

#ifdef DEBUGx
    PlotLevels(QObject::tr("After CalLogicalCol"));
#endif
    
    error = CalLogicalRow();

#ifdef DEBUG
    PlotLevels(QObject::tr("After CalLogicalRow"));
#endif

    error = DecideDeviceOrientation();
    if (error)
        return OKAY;

    error = DecideDeviceWhetherToReverse();
    if (error)
        return OKAY;

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
#ifdef DEBUG
    qInfo() << LINE_INFO << endl;
#endif

    m_levels.clear();

    /* set all device unvisited */
    memset(m_visited, 0, sizeof(int) * m_ckt->DeviceCount());

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
    while (NOT level->Empty()) {
        level = CreateNextLevel(level);
        if (level->Empty()) break;
        level->SetId(currLevelId);
        currLevelId++;
#ifdef DEBUG
        level->PrintAllDevices();
#endif
        m_levels.push_back(level);
        currDeviceNumber += level->AllDeviceCount();
    }

#ifdef DEBUG
    qInfo() << "total device(" << totalDeviceNumber << "), "
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

int ASG::ClassifyConnectDeviceByLevel()
{
    foreach (Device *dev, m_ckt->GetDeviceList())
        dev->ClassifyConnectDeviceByLevel();

#ifdef DEBUG
    foreach (Level *level, m_levels)
        level->PrintAllConnections();
#endif

    return OKAY;
}

int ASG::CalLogicalRow()
{
    int error = DetermineFirstLevelLogicalRow();
    if (error)
        return ERROR;

    error = ForwardPropagateLogicalRow();
    if (error)
        return ERROR;

    return error;
}

int ASG::DetermineFirstLevelLogicalRow()
{
    /* just simply assign logical row number */
    if (m_levels.size() < 1)
        return OKAY;
    
    Level *firstLevel = m_levels.front();
    int row = 0;
    foreach (Device *dev, firstLevel->AllDevices()) {
        dev->SetLogicalRow(row);
        row += Row_Device_Factor;
    }

#ifdef DEBUGx
    firstLevel->PrintLogicalPos();
#endif
}

int ASG::ForwardPropagateLogicalRow()
{
    if (m_levels.size() < 2)
        return OKAY;

    Level *level = nullptr;
    for (int i = 1; i < m_levels.size(); ++ i) {
        level = m_levels.at(i);
        level->AssignDeviceLogicalRow(); // assign logical row to devices
    }

#ifdef DEBUG
    foreach (Level *level, m_levels)
        level->PrintLogicalPos();
#endif

    return OKAY;
}

int ASG::DecideDeviceOrientation()
{
    foreach (Device *dev, m_ckt->GetDeviceList())
        dev->DecideOrientationByPredecessors();

#ifdef DEBUG
    foreach (Level *level, m_levels)
        level->PrintOrientation();
#endif

    return OKAY;
}

int ASG::DecideDeviceWhetherToReverse()
{
    foreach (Device *dev, m_ckt->GetDeviceList())
        dev->DecideReverseByPredecessors();

    if (m_levels.size() > 0) {
        foreach (Device *dev, m_levels.front()->AllDevices())
            dev->DecideReverseBySuccessors();
    }

#ifdef DEBUG
    foreach (Level *level, m_levels)
        level->PrintReverse();
#endif

    return OKAY;
}
