#ifndef NETLISTVIZ_ASG_LEVEL_H
#define NETLISTVIZ_ASG_LEVEL_H

/*
 * @filename : Level.h
 * @author   : Hao Limin
 * @date     : 2020.09.12 
 * @email    : haolimin01@sjtu.edu.cn
 * @desp     : Level class contains some devices, represents one level.
 */

#include "Define/Define.h"
#include "Define/TypeDefine.h"
#include <set>

class Device;

class Level
{
public:
    Level();
    ~Level();

    void AddDevice(Device *device);

    DeviceList AllDevices() const;
    bool       LogicalRowOccupied(int logRow) const;
    bool       Empty() const { return (m_devices.size() == 0); }
    int        AllDeviceCount() const { return m_devices.size(); }

private:
    DISALLOW_COPY_AND_ASSIGN(Level);

    DeviceList    m_devices;
    std::set<int> m_devLogRowSet;
};

#endif // NETLISTVIZ_ASG_LEVEL_H
