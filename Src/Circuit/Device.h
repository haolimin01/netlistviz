#ifndef NETLISTVIZ_CIRCUIT_DEVICE_H
#define NETLISTVIZ_CIRCUIT_DEVICE_H

/*
 * @filename : Device.h
 * @author   : Hao Limin
 * @date     : 2020.09.11
 * @email    : haolimin01.sjtu.edu.cn 
 * @desp     : Device class, DO NOT contain geometrical information.
 * @modified : Hao Limin, 2020.09.23
 * @modified : Hao Limin, 2020.09.26
 */

#include "Define/TypeDefine.h"
#include "Define/Define.h"

class Terminal;
class Wire;
class SchematicDevice;

/* defalut device orientation:
 * vertical :  +
 *             -
 * 
 * horizontal: +-
 * 
 * reverse: change (+-) to (-+)
 * 
 */

class Device
{
public:
    Device(DeviceType, QString name);
    ~Device();

    int          AddTerminal(Terminal *terminal, TerminalType type);
    void         SetValue(double value)    { m_value = value; }
    void         SetId(int id)             { m_id = id; }
    int          Id() const                { return m_id; }
    QString      Name() const              { return m_name; }
    DeviceType   GetDeviceType() const     { return m_deviceType; }
    void         AddConnectDevice(Device *dev);
    void         SetAsGroundCap(bool is)   { m_groundCap = is; }
    bool         GroundCap() const         { return m_groundCap; }
    bool         CoupledCap() const; 
    Terminal*    GetTerminal(TerminalType type) const;
    Terminal*    GetTerminal(Node *node) const;
    void         DecideOrientationByPredecessors();
    Orientation  GetOrientation() const    { return m_orien; }
    void         ClassifyConnectDeviceByLevel();
    void         SetLogicalRow(int row)    { m_logRow = row; }
    int          LogicalRow() const        { return m_logRow; }
    void         SetLevelId(int id)        { m_levelId = id; }
    int          LevelId() const           { return m_levelId; }
    int          LogicalCol() const        { return m_levelId; }
    DeviceList   Predecessors() const      { return m_predecessors; }
    DeviceList   Successors() const        { return m_successors; }
    void         CalLogicalRowByPredecessors();
    void         ClearConnectDesps();
    void         SetMaybeAtFirstLevel(bool at) { m_maybeAtFirstLevel = at; }
    bool         MaybeAtFirstLevel() const     { return m_maybeAtFirstLevel; }
    ConnectDespList ConnectDesps() const { return m_connectDesps; }
    bool         Reverse() const           { return m_reverse; }       
    void         DecideReverseByPredecessors();
    void         DecideReverseBySuccessors();
    void         SetGeometricalCol(int col) { m_geoCol = col; }
    int          GeometricalCol() const     { return m_geoCol; }
    void         SetGeometricalRow(int row) { m_geoRow = row; }
    int          GeometricalRow() const     { return m_geoRow; }
    WireList     WiresFromPredecessors() const;

    TerminalList GetTerminalList() const;


    /* For creating SchematicWire */
    void                SetSchematicDevice(SchematicDevice *sDevice) { m_sDevice=sDevice;}
    SchematicDevice*    GetSchematicDevice() const { return m_sDevice; } 

    // DeviceList          CapConnectDeviceList() const { return m_capConnectDeviceList; }
    // STerminalTable      CapConnectSTerminalTable() const;

    void Print() const;

private:
    DISALLOW_COPY_AND_ASSIGN(Device);
    bool    HasConnectionIgnoreGnd(Terminal *otherTer, TerminalType thisType) const;
    bool    HasConnectionIgnoreGnd(Device *otherDev, TerminalType thisType) const;
    bool    MaybeVertical() const;

    TerminalTable                     m_terminals;
    QString                           m_name;
    double                            m_value;
    DeviceType                        m_deviceType;
    bool                              m_groundCap;
    int                               m_id;
    /* If connects to ground, it maybe at first level (isrc, vsrc now) */
    bool                              m_maybeAtFirstLevel;
    bool                              m_reverse; 

    DeviceList                        m_predecessors; // previous level
    DeviceList                        m_fellows;      // the same level
    DeviceList                        m_successors;   // next level
    int                               m_levelId;
    int                               m_logRow;       // logical row, can be < 0
    int                               m_geoRow;
    int                               m_geoCol;
    Orientation                       m_orien;        // orientation : Horizontal/Vertical
    ConnectDespList                   m_connectDesps;

    // DeviceList                        m_capConnectDeviceList;    // for cap
    // TerminalTable                     m_capConnectTerminalTable; // for cap
    SchematicDevice                  *m_sDevice; // For creating SchematicWire
};

#endif // NETLISTVIZ_CIRCUIT_DEVICE_H
