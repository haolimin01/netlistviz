#ifndef NETLISTVIZ_CIRCUIT_DEVICE_H
#define NETLISTVIZ_CIRCUIT_DEVICE_H

/*
 * @filename : Device.h
 * @author   : Hao Limin
 * @date     : 2020.09.11
 * @email    : haolimin01.sjtu.edu.cn 
 * @desp     : Device class, DO NOT contain geometrical information.
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
 * reverse: change orientation
 * 
 * We assume that device is horizontal now.
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
    void         SetLevelId(int levelid)   { m_levelId = levelid; }
    int          LevelId() const           { return m_levelId; }
    bool         HasConnection(Device *device) const;
    bool         HasConnectionIgnoreGnd(Device *device) const;
    int          BubbleValue() const       { return m_bubbleValue; }
    void         SetBubbleValue(int value) { m_bubbleValue = value; }
    void         SetReverse(int reverse)   { m_reverse = reverse; }
    bool         Reverse() const           { return m_reverse; }       
    void         DecideReverseByPredecessors(IgnoreCap ignore);
    void         DecideReverseBySuccessors(IgnoreCap ignore);
    void         SetRow(int row);
    int          Row() const               { return m_row; }
    void         AddPredecessor(Device *dev);
    void         AddSuccessor(Device *dev);
    DeviceList   Predecessors() const      { return m_predecessors; }
    DeviceList   Successors() const        { return m_successors; }
    void         CalBubbleValueByPredecessors(IgnoreCap ignore);
    void         CalBubbleValueBySuccessors(IgnoreCap ignore);
    WireList     WiresFromPredecessors() const;
    TerminalList GetTerminalList() const;
    void         ClearPredecessors() { m_predecessors.clear(); }
    void         ClearSuccessors()   { m_successors.clear(); }
    void         SetMaybeAtFirstLevel(bool at) { m_maybeAtFirstLevel = at; }
    bool         MaybeAtFirstLevel() const     { return m_maybeAtFirstLevel; }

    /* For creating SchematicWire */
    void                SetSchematicDevice(SchematicDevice *sDevice) { m_sDevice=sDevice;}
    SchematicDevice*    GetSchematicDevice() const { return m_sDevice; } 

    DeviceList          CapConnectDeviceList() const { return m_capConnectDeviceList; }
    STerminalTable      CapConnectSTerminalTable() const;

    void Print() const;
    void PrintBubbleValue() const;

private:
    DISALLOW_COPY_AND_ASSIGN(Device);
    bool    HasConnectionIgnoreGnd(Terminal *otherTer, TerminalType thisType);

    TerminalTable                     m_terminals;
    QString                           m_name;
    double                            m_value;
    DeviceType                        m_deviceType;
    bool                              m_groundCap;
    int                               m_id;
    /* If connects to ground, it maybe at first level (isrc, vsrc now) */
    bool                              m_maybeAtFirstLevel;
    bool                              m_reverse; 

    /* For Bubble Sort */
    int                               m_levelId; // col
    DeviceList                        m_predecessors;
    DeviceList                        m_successors;
    int                               m_bubbleValue;
    int                               m_row;   // row

    DeviceList                        m_capConnectDeviceList;    // for cap
    TerminalTable                     m_capConnectTerminalTable; // for cap
    SchematicDevice                  *m_sDevice; // For creating SchematicWire

    friend class SchematicDevice;
};

#endif // NETLISTVIZ_CIRCUIT_DEVICE_H
