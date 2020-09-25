#ifndef NETLISTVIZ_ASG_HYPER_LEVEL_H
#define NETLISTVIZ_ASG_HYPER_LEVEL_H


/*
 * @filename : HyperLevel.h
 * @date     : 2020.09.24
 * @author   : Hao Limin
 * @email    : haolimin01@sjtu.edu.cn 
 * @desp     : HyperLevel = FrontLeftLevel + FrontLevel + FrontRightLevel +
 *                          Channel + BackLevel
 * FrontLeftLevel, FrontLevel and FrontRightLevel place Vertical devices.
 * BackLevel places Horizontal devices.
 * FrontLeftLevel, FrontRightLevel place FrontLevel's parallel devices, they maybe empty.
 * BackLevel's parallel devices place up or down of BackLevel.
 * NOTE : One device's parallevl devices <= 2
 */

#include "Define/Define.h"
#include "Define/TypeDefine.h"
#include "Level.h"

class Channel;


class HyperLevel
{
public:
    explicit HyperLevel(int id);
    HyperLevel();
    ~HyperLevel();

    void       SetId(int id);
    int        Id() const { return m_id; }
    void       AddDevice(Device *dev);
    void       AddDevices(const DeviceList &devList);
    int        AllDeviceCount() const { return m_allDevices.size(); }
    DeviceList AllDevices() const { return m_allDevices; }
    void       AssignRowNumberByBubbleValue(IgnoreCap ignore);
    void       Adjust();
    Channel*   CreateChannel();
    void       AssignGeometricalCol(int &startCol);

    void       PrintAllDevices() const;
    void       PrintAllConnections() const;
    void       PrintAdjustment() const;
    void       PrintGeometricalPos() const;

private:
    DISALLOW_COPY_AND_ASSIGN(HyperLevel);

    /* For Adjust() */
    void        AdjustDeviceOrientation();  
    void        ClearLevels();
    void        AddToFrontLevel(Device *dev);

    DeviceList  m_allDevices;
    int         m_id;

    Level      *m_frontLeftLevel;
    Level      *m_frontRightLevel;
    Level      *m_frontLevel;
    Channel    *m_channel;
    Level      *m_backLevel;
};


#endif //NETLISTVIZ_ASG_HYPER_LEVEL_H
