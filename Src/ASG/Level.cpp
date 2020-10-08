#include "Level.h"
#include <QDebug>
#include "Circuit/Device.h"

Level::Level(int id)
{
    m_id = id;
    m_rowGap = DFT_MAX_DEVICE_ROW_GAP;
}

Level::Level()
{
    m_id = 0;
    m_rowGap = DFT_MAX_DEVICE_ROW_GAP;
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

#ifdef DEBUGx
    qInfo() << "Before Assign Logical Row in Level";
    PrintLogicalPos();
#endif

    /* 3. put the first devie's row into rows */
    Device *dev = m_devices.front();
    m_rows.push_back(dev->LogicalRow());

    /* 4. deal with the rest */
    int row = 0, currMaxRow = 0;
    for (int i = 1; i < m_devices.size(); ++ i) {
        dev = m_devices.at(i);
        row = dev->LogicalRow();
        currMaxRow = m_rows.back();

        if (row < currMaxRow) {
            RowsFlexibleShiftUpBy(m_rows, (currMaxRow - row) / 2);
            // row = m_rows.back() + m_rowGap;
            row = currMaxRow + m_rowGap;
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
        RowsFlexibleShiftUpBy(m_rows, m_rowGap);
        row = currMaxRow + m_rowGap;
        currMaxRow = row;
        m_rows.push_back(currMaxRow);
    }

    Q_ASSERT(m_rows.size() == m_devices.size());

    for (int i = 0; i < m_rows.size(); ++ i)
        m_devices[i]->SetLogicalRow(m_rows.at(i));

#ifdef DEBUGx
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

void Level::RowsFlexibleShiftUpBy(QVector<int> &rows, int n) const
{
    if (rows.size() == 0)
        return;
    
    int curr = 0, prev = 0;

    for (int i = rows.size() - 1; i > 0; -- i) {
        curr = rows.at(i);
        prev = rows.at(i - 1);
        if (curr - prev >= (n + 1)) {
            rows[i] -= n;
            return;
        }
        rows[i] -= n;
    }

    rows[0] -= n;
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

void Level::PrintRowGap() const
{
    printf("--------------- Level %d ---------------\n", m_id);

    qInfo() << "Row Gap(" << m_rowGap << ")"; 

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

