#include "Level.h"
#include <QDebug>
#include "Circuit/Device.h"

Level::Level(int id)
{
    m_id = id;
}

Level::Level()
{
    m_id = 0;
}

Level::~Level()
{
    m_devices.clear();
}

void Level::AddDevice(Device *device)
{
    m_devices.push_back(device);
    device->SetLevelId(m_id);
}

void Level::AddDevices(const DeviceList &devList)
{
    foreach (Device *dev, devList)
        AddDevice(dev);
}

DeviceList Level::AllDevices() const
{
    return m_devices;
}

void Level::SetId(int id)
{
    m_id = id;
    UpdateDeviceLevelId();
}

void Level::UpdateDeviceLevelId()
{
    foreach (Device *dev, m_devices)
        dev->SetLevelId(m_id);
}

void Level::AssignDeviceLogicalRow()
{
    m_rows.clear();
    if (m_devices.size() < 1)
        return;

    /* 1. Initialize logical row */
    foreach (Device *dev, m_devices)
        dev->CalLogicalRowByPredecessors();

    /* 2. Sort by Logical row */
    SortByLogicalRow(m_devices);

#ifdef DEBUG
    qInfo() << "Before Assign Logical Row in Level";
    PrintLogicalPos();
#endif

    /* 3. put the first devie's row into rows */
    Device *dev = m_devices.front();
    m_rows.push_back(dev->LogicalRow());

    /* 4. deal with the rest */
    /* Maybe refine previous level in the future */
    int row = 0, currMaxRow = 0, prevRow = 0;
    bool oneShift = false;
    for (int i = 1; i < m_devices.size(); ++ i) {
        dev = m_devices.at(i);
        row = dev->LogicalRow();
        currMaxRow = m_rows.back();

        if (row < currMaxRow) {
            RowsShiftUpBy(m_rows, currMaxRow - row);
            row = currMaxRow + 1;
            currMaxRow = row;
            m_rows.push_back(currMaxRow);
            continue;
        }

        if (row > currMaxRow) {
            currMaxRow = row;
            m_rows.push_back(currMaxRow);
            continue;
        }

        /* row == currMxRow */
        oneShift = false;
        if (i >= 2) {
            prevRow = m_rows.at( i - 2);
            if (currMaxRow - prevRow >= 2)
                oneShift = true;
        }

        if (oneShift) {
            m_rows[i - 1] = currMaxRow - 1;
            row = currMaxRow;
        } else {
            RowsShiftUpBy(m_rows, 1);
            row = currMaxRow + 1;
        }

        m_rows.push_back(row);
    }

    Q_ASSERT(m_rows.size() == m_devices.size());

    for (int i = 0; i < m_rows.size(); ++ i)
        m_devices[i]->SetLogicalRow(m_rows.at(i));

#ifdef DEBUG
    qInfo() << "After Assign Logical Row in Level";
    PrintLogicalPos();
#endif
}

void Level::SortByLogicalRow(DeviceList &devList) const
{
    /* lambda expression */
    qSort(devList.begin(), devList.end(),
        [](Device *a, Device *b){ return a->LogicalRow() < b->LogicalRow(); });
}

void Level::RowsShiftUpBy(QVector<int> &rows, int n) const
{
    for (int i = 0; i < rows.size(); ++ i)
        rows[i] -= n;
}

void Level::AssignDeviceGeometricalCol(int col)
{
    foreach (Device *dev, m_devices) {
        dev->SetGeometricalCol(col);
    }
}

void Level::PrintAllDevices() const
{
    printf("--------------- Level %d ---------------\n", m_id);

    foreach (Device *dev, m_devices)
        qInfo() << dev->Name()
                << " Level(" << dev->LevelId() << ")";

    printf("---------------------------------------\n");
}

void Level::PrintAllConnections() const
{
    printf("--------------- Level %d ---------------\n", m_id);

    QString result = "";
    foreach (Device *dev, m_devices) {
        result += (dev->Name() + " ");
        result += ("predecessors( ");
        foreach (Device *predecessor, dev->Predecessors())
            result += (predecessor->Name() + " ");
        result += ("), ");
        result += ("successors( ");
        foreach (Device *successor, dev->Successors())
            result += (successor->Name() + " ");
        result += (")");
        qInfo() << result;
        result = "";
    }

    printf("---------------------------------------\n");
}

void Level::PrintLogicalPos() const
{
    printf("--------------- Level %d ---------------\n", m_id);

    foreach (Device *dev, m_devices) {
        qInfo() << dev->Name()
                << "logicalRow(" << dev->LogicalRow() << "),"
                << "logicalCol(" << dev->LogicalCol() << ")";
    }

    printf("---------------------------------------\n");
}

void Level::PrintOrientation() const
{
    printf("--------------- Level %d ---------------\n", m_id);

    QString tmp = "";
    foreach (Device *dev, m_devices) {
        tmp = (dev->Name() + " Orientation(");
        if (dev->GetOrientation() == Horizontal)
            tmp += "H)";
        else
            tmp += "V)";

        qInfo() << tmp;
    }

    printf("---------------------------------------\n");
}

void Level::PrintReverse() const
{
    printf("--------------- Level %d ---------------\n", m_id);

    QString tmp = "";
    foreach (Device *dev, m_devices) {
        tmp = (dev->Name() + " Reverse(");
        if (dev->Reverse())
            tmp += "yes)";
        else
            tmp += "no)";

        qInfo() << tmp;
    }

    printf("---------------------------------------\n");
}

void Level::PrintGeometricalPos() const
{
    printf("--------------- Level %d ---------------\n", m_id);

    foreach (Device *dev, m_devices) {
        qInfo() << dev->Name()
                << "geometricalRow(" << dev->GeometricalRow() << "),"
                << "geometricalCol(" << dev->GeometricalCol() << ")";
    }

    printf("---------------------------------------\n");
}

