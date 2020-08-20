#ifndef NETLISTVIZ_ASG_ASG_H
#define NETLISTVIZ_ASG_ASG_H

/*
 * @filename : ASG.h
 * @author   : Hao Limin
 * @date     : 2020.07.31
 * @desp     : Automatic Schematic Generator.
 */

#include <QVector>
#include <QMap>
#include "Define/Define.h"

class  Matrix;
class  SchematicData;
class  SchematicDevice;
class  TablePlotter;
struct WireDescriptor;

typedef QVector<SchematicDevice *>  DeviceList;

/* each level device list */
class DevLevelDescriptor
{
public:
    explicit DevLevelDescriptor(int level);
    ~DevLevelDescriptor();

    void AddDevice(SchematicDevice *device);
    void AddDevices(const DeviceList &devList);
    DeviceList AllDevices() const;
    DeviceList AllDevicesWithoutCap() const;
    DeviceList Devices(int priority) const;

    int CapCount() const  { return m_capCnt; }
    int DeviceCntWithoutCap() const  { return m_deviceCntWithoutCap; }

private:
    DISALLOW_COPY_AND_ASSIGN(DevLevelDescriptor);

    int m_level;

    /* priority : DeviceList */
    QMap<int, DeviceList> m_levelDevices;

    int m_deviceCntWithoutCap;
    int m_capCnt;
};


class ASG
{
public:
    explicit ASG(SchematicData *data);
    ASG();
    ~ASG();

    void SetSchematicData(SchematicData *data);
    void BuildIncidenceMatrix();
    void Levelling();
    void Bubbling();

    bool BuildMatrixFinished() const { return m_buildMatrixFlag; }
    bool LevellingFinished() const { return m_levellingFlag; }
    bool BubblingFinished() const { return m_bubblingFlag; }

    /* 2-d array */
    QVector<DevLevelDescriptor*> FinalDevices() const { return m_devices; }
    QVector<WireDescriptor*>     WireDesps() const { return m_wireDesps; }

private:
    DISALLOW_COPY_AND_ASSIGN(ASG);

    /* return next level device list. */
    DeviceList FillNextLevelDeviceList(const DeviceList) const;

    void InsertRLC(SchematicDevice *device);
    void InsertVI(SchematicDevice *device);

    void GenerateWireDesps();
    // void TagDeviceOnBranch();

    void PrintAllDevices() const;
    void PlotAllDevices();

    Matrix        *m_matrix;      // incidence matrix
    int            m_matrixSize;  // matrix size
    SchematicData *m_ckt;         // circuit infomation

    /* level : DevLevelDescriptor */
    /* Do not contain GND */
    QVector<DevLevelDescriptor*> m_devices;
    /* for generate wires */
    QVector<WireDescriptor*> m_wireDesps;


    bool           m_buildMatrixFlag;
    bool           m_levellingFlag;
    bool           m_bubblingFlag;

    TablePlotter  *m_levelPlotter;
    TablePlotter  *m_bubblePlotter;
};

#endif // NETLISTVIZ_ASG_ASG_H