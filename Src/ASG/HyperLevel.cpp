#include "HyperLevel.h"
#include <QDebug>
#include "Circuit/Device.h"
#include "Channel.h"
#include "Wire.h"

HyperLevel::HyperLevel(int id)
{
    m_id = id;
    m_frontLeftLevel = nullptr;
    m_frontLevel = nullptr;
    m_frontRightLevel = nullptr;
    m_channel = nullptr;
    m_backLevel = nullptr;
}

HyperLevel::HyperLevel()
{
    m_id = 0;
    m_frontLeftLevel = nullptr;
    m_frontLevel = nullptr;
    m_frontRightLevel = nullptr;
    m_channel = nullptr;
    m_backLevel = nullptr;
}

HyperLevel::~HyperLevel()
{
    m_allDevices.clear();
    if (m_frontLeftLevel) {
        delete m_frontLeftLevel;
    }
    if (m_frontLevel) {
        delete m_frontLevel;
    }
    if (m_frontRightLevel) {
        delete m_frontRightLevel;
    }
    if (m_backLevel) {
        delete m_backLevel;
    }
}

void HyperLevel::AddDevice(Device *dev)
{
    m_allDevices.push_back(dev);
    dev->SetHyperLevelId(m_id);
}

void HyperLevel::AddDevices(const DeviceList &devList)
{
    foreach (Device *dev, devList)
        AddDevice(dev);
}

void HyperLevel::SetId(int id)
{
    m_id = id;
    foreach (Device *dev, m_allDevices)
        dev->SetHyperLevelId(id);
}

void HyperLevel::AssignRowNumberByBubbleValue(IgnoreCap ignore)
{
#ifdef DEBUGx
    qInfo() << "HyperLevel " << m_id << ", Before sorting ";
    foreach (Device *dev, m_allDevices)
        dev->PrintBubbleValue();
#endif

    /* sort device by bubble value */
    qSort(m_allDevices.begin(), m_allDevices.end(),
        [](Device *a, Device *b){ return a->BubbleValue() < b->BubbleValue(); });

#ifdef DEBUGx
    qInfo() << "HyperLevel " << m_id << ", After sorting ";
    foreach (Device *dev, m_allDevices)
        dev->PrintBubbleValue();
#endif

    /* we consider bubble value as device row now */
    int maxRow = -1, row = -1;
    int bubbleValue = -1;

    foreach (Device *dev, m_allDevices) {

        if (ignore == IgnoreGCap && dev->GroundCap()) continue;
        if ((ignore == IgnoreGCCap) && (dev->GroundCap() || dev->CoupledCap())) continue;

        bubbleValue = dev->BubbleValue();
        if (bubbleValue > maxRow) {
            row = bubbleValue;
            maxRow = row;
        } else {
            row = maxRow + 1;
            maxRow++;
        }

        dev->SetLogicalRow(row);
    }
}

void HyperLevel::Adjust()
{
    AdjustDeviceOrientation();
}

void HyperLevel::AdjustDeviceOrientation()
{
    ClearLevels();
    m_frontLeftLevel = new Level();
    m_frontLevel = new Level();
    m_frontRightLevel = new Level();
    m_backLevel = new Level();

    Orientation orien = Horizontal;

    foreach (Device *dev, m_allDevices) {
        // if (dev->IsVertical()) {
        //     orien = Vertical;
        //     AddToFrontLevel(dev);
        // } else {
        //     orien = Horizontal;
        //     m_backLevel->AddDevice(dev);
        // }
        orien = Horizontal;
        m_backLevel->AddDevice(dev);
        dev->SetOrientation(orien);
    }
}

void HyperLevel::AddToFrontLevel(Device *dev)
{
    bool parallel = false;
    Device *frontDev = nullptr;
    foreach (frontDev, m_frontLevel->AllDevices()) {
        parallel = dev->IsParallel(frontDev);
        if (parallel)
            break;
    }

    if (parallel) {

        dev->SetLogicalRow(frontDev->LogicalRow());
        if (m_frontLeftLevel->LogicalRowOccupied(dev->LogicalRow())) 
            m_frontRightLevel->AddDevice(dev);
        else 
            m_frontLeftLevel->AddDevice(dev);

    } else {
        m_frontLevel->AddDevice(dev);
    }
}

void HyperLevel::ClearLevels()
{
    if (m_frontLeftLevel) {
        delete m_frontLeftLevel;
        m_frontLeftLevel = nullptr;
    }
    if (m_frontLevel) {
        delete m_frontLevel;
        m_frontLevel = nullptr;
    }
    if (m_frontRightLevel) {
        delete m_frontRightLevel;
        m_frontRightLevel = nullptr;
    }
    if (m_backLevel) {
        delete m_backLevel;
        m_backLevel = nullptr;
    }
}

Channel* HyperLevel::CreateChannel()
{
    if (m_frontLevel->AllDeviceCount() == 0 &&
        m_backLevel->AllDeviceCount() == 0)
        return nullptr;

    if (m_channel)
        delete m_channel;

    m_channel = new Channel();
    Device *frontDev = nullptr, *backDev = nullptr;
    Terminal *thisTer = nullptr, *otherTer = nullptr;
    
    foreach (frontDev, m_frontLevel->AllDevices()) {
        foreach (backDev, m_backLevel->AllDevices()) {
            if (frontDev->HasConnectionIgnoreGnd(backDev, &thisTer, &otherTer)) {
                Q_ASSERT(thisTer && otherTer);
                Wire *newWire = new Wire(frontDev, thisTer, backDev, otherTer);
                m_channel->AddWire(newWire);
            }
        }
    }

    if (m_channel->Empty()) {
        delete m_channel;
        m_channel = nullptr;
        return nullptr;
    }

    return m_channel;
}

void HyperLevel::AssignGeometricalCol(int &startCol)
{
    Device *dev = nullptr;

    if (NOT m_frontLeftLevel->Empty()) {
        foreach (dev, m_frontLeftLevel->AllDevices())
            dev->SetGeometricalCol(startCol);
        startCol++;
    }

    if (NOT m_frontLevel->Empty()) {
        foreach (dev, m_frontLevel->AllDevices())
            dev->SetGeometricalCol(startCol);
        startCol++;
    }

    if (NOT m_frontRightLevel->Empty()) {
        foreach (dev, m_frontRightLevel->AllDevices());
            dev->SetGeometricalCol(startCol);
        startCol++;
    }

    if (m_channel)
        startCol++;

    if (NOT m_backLevel->Empty()) {
        foreach (dev, m_backLevel->AllDevices())
            dev->SetGeometricalCol(startCol);
        startCol++;
    }
}

int HyperLevel::MinLogicalRow() const
{
    Device *dev= m_allDevices.front();
    int minRow = dev->LogicalRow();
    for (int i = 1; i < m_allDevices.size(); ++ i) {
        dev = m_allDevices.at(i);
        if (dev->LogicalRow() < minRow)
            minRow = dev->LogicalRow();
    }

    return minRow;
}

void HyperLevel::PrintAllDevices() const
{
    printf("--------------- HyperLevel %d ---------------\n", m_id);

    foreach (Device *dev, m_allDevices)
        qInfo() << dev->Name()
                << " HyperLevel(" << QString::number(dev->HyperLevelId()) << ")";

    printf("--------------------------------------------\n");
}

void HyperLevel::PrintAllConnections() const
{
    printf("--------------- HyperLevel %d ---------------\n", m_id);

    QString result = "";
    foreach (Device *dev, m_allDevices) {
        result += (dev->Name() + " ");
        result += ("predecessors( ");
        foreach (Device *predecessor, dev->Predecessors())
            result += (predecessor->Name() + " ");
        result += ("), ");
        result += ("fellows( ");
        foreach (Device *fellow, dev->Fellows())
            result += (fellow->Name() + " ");
        result += ("), ");
        result += ("successors( ");
        foreach (Device *successor, dev->Successors())
            result += (successor->Name() + " ");
        result += (")");
        qInfo() << result;
        result = "";
    }

    printf("--------------------------------------------\n");
}

void HyperLevel::PrintAdjustment() const
{
    printf("--------------- HyperLevel %d ---------------\n", m_id);

    QString result = "";
    if (NOT m_frontLeftLevel->Empty()) {
        qInfo() << "FrontLeftLevel";
        foreach (Device *dev, m_frontLeftLevel->AllDevices()) {
            result = (dev->Name() + " ");
            result += ("logicalRow(" + QString::number(dev->LogicalRow()) + "), ");
            result += ("logicalCol(" + QString::number(dev->LogicalCol()) + "), ");
            result += ("orientation(" + QString::number(dev->GetOrientation()) + ")");
            qInfo() << result;
        }
    }
    if (NOT m_frontLevel->Empty()) {
        qInfo() << "FrontLevel";
        foreach (Device *dev, m_frontLevel->AllDevices()) {
            result = (dev->Name() + " ");
            result += ("logicalRow(" + QString::number(dev->LogicalRow()) + "), ");
            result += ("logicalCol(" + QString::number(dev->LogicalCol()) + "), ");
            result += ("orientation(" + QString::number(dev->GetOrientation()) + ")");
            qInfo() << result;
        }
    }
    if (NOT m_frontRightLevel->Empty()) {
        qInfo() << "FrontRightLevel";
        foreach (Device *dev, m_frontRightLevel->AllDevices()) {
            result = (dev->Name() + " ");
            result += ("logicalRow(" + QString::number(dev->LogicalRow()) + "), ");
            result += ("logicalCol(" + QString::number(dev->LogicalCol()) + "), ");
            result += ("orientation(" + QString::number(dev->GetOrientation()) + ")");
            qInfo() << result;
        }
    }
    if (NOT m_backLevel->Empty()) {
        qInfo() << "BackLevel";
        foreach (Device *dev, m_backLevel->AllDevices()) {
            result = (dev->Name() + " ");
            result += ("logicalRow(" + QString::number(dev->LogicalRow()) + "), ");
            result += ("logicalCol(" + QString::number(dev->LogicalCol()) + "), ");
            result += ("orientation(" + QString::number(dev->GetOrientation()) + ")");
            qInfo() << result;
        }
    }

    printf("--------------------------------------------\n");
}

void HyperLevel::PrintGeometricalPos() const
{
    printf("--------------- HyperLevel %d ---------------\n", m_id);

    QString result = "";
    foreach (Device *dev, m_allDevices) {
        result += (dev->Name() + " ");
        result += ("geometricalCol(" + QString::number(dev->GeometricalCol()) + "), ");
        result += ("geometricalRow(" + QString::number(dev->GeometricalRow()) + ")");
        qInfo() << result;
        result = "";
    }

    printf("--------------------------------------------\n");

}
