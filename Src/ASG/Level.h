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

    void AddDevice(Device *device);
    void AddDevices(const DeviceList &devList);

    DeviceList AllDevices() const;
    bool       Empty() const { return (m_devices.size() == 0); }
    int        AllDeviceCount() const { return m_devices.size(); }
    void       SetId(int id);
    int        Id() const { return m_id; }
    void       AssignDeviceLogicalRow();
    void       AssignDeviceGeometricalCol(int col);
    void       SetRowGap(int gap) { m_rowGap = gap; };
    int        RowGap() const { return m_rowGap; }
    WireList   Wires();

    void       PrintAllDevices() const;
    void       PrintAllConnections() const;
    void       PrintRowGap() const;
    void       PrintLogicalPos() const;
    void       PrintOrientation() const;
    void       PrintReverse() const;
    void       PrintGeometricalPos() const;

private:
    DISALLOW_COPY_AND_ASSIGN(Level);

    void UpdateDeviceLevelId();
    void SortByLogicalRow(DeviceList &devList) const;
    void RowsShiftUpBy(QVector<int> &rows, int n) const;
    void RowsFlexibleShiftUpBy(QVector<int> &rows, int n) const;
    void CollectWires();
    void AddWire(Wire *wire);

    DeviceList           m_devices;
    int                  m_id;
    QVector<int>         m_rows;
    QMap<QString, Wire*> m_wires; // deduplication

    int                  m_rowGap;
};

#endif // NETLISTVIZ_ASG_LEVEL_H
