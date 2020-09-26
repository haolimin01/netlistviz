#include "Level.h"
#include <QDebug>
#include "Circuit/Device.h"

Level::Level()
{
}

Level::~Level()
{
    m_devices.clear();
    m_devLogRowSet.clear();
}

void Level::AddDevice(Device *device)
{
    m_devices.push_back(device);
    m_devLogRowSet.insert(device->LogicalRow());
}

bool Level::LogicalRowOccupied(int logRow) const
{
    std::set<int>::iterator it;
    it = m_devLogRowSet.find(logRow);
    if (it != m_devLogRowSet.end())
        return true;
    else
        return false;
}

DeviceList Level::AllDevices() const
{
    return m_devices;
}
