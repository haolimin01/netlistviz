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

class Device;

class Level
{
public:
    explicit Level(int id);
    Level();
    ~Level();

    void SetId(int id);
    int  Id() const    { return m_id; }
    void AddDevice(Device *device);
    void AddDevices(const DeviceList &devList);

    int  DeviceCountWithoutGCap() const;
    int  DeviceCountWithoutGCCap() const;
    int  AllDeviceCount() const;
    void AssignRowNumberByBubbleValue(IgnoreCap ignore);

    DeviceList AllDevices() const;

    void PrintAllDevices() const;
    void PrintAllConnections() const;

private:
    DISALLOW_COPY_AND_ASSIGN(Level);

    int        m_id;
    DeviceList m_deviceList;

    int        m_deviceCountWithoutCap;
    int        m_gCapCount;
    int        m_cCapCount;
};

#endif // NETLISTVIZ_ASG_LEVEL_H
