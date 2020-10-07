/*
 * @filename : TypeDefine.h
 * @author   : Hao Limin
 * @date     : 2020.09.02
 * @email    : haolimin01@sjtu.edu.cn
 * @desp     : Basic type defines
 */

#ifndef NETLISTVIZ_DEFINE_TYPEDEFINE_H
#define NETLISTVIZ_DEFINE_TYPEDEFINE_H

#include <QMap>
#include <QVector>
#include <QString>

/* For terminal */
enum TerminalType { Positive = 0, Negative, General };

/* Device Type */
enum DeviceType { RESISTOR = 0, CAPACITOR, INDUCTOR, ISRC, VSRC, GND, Other };

/* Orientation */
enum Orientation { Horizontal, Vertical };

/* ASG Dialog */
enum IgnoreCap { IgnoreGCap = 0, IgnoreCCap, IgnoreGCCap, IgnoreNoCap };

/* Circuit Containers */
class Device;
class Node;
class Terminal;
class Wire;
struct ConnectDescriptor;
class Channel;
class Level;
class Dot;
typedef QMap<QString, Device*>          DeviceTable;   
typedef QMap<QString, Node*>            NodeTable;
typedef QVector<Device*>                DeviceList;
typedef QVector<Node*>                  NodeList;
typedef QMap<TerminalType, Terminal*>   TerminalTable;
typedef QVector<Terminal*>              TerminalList;
typedef QVector<Wire*>                  WireList;
typedef QVector<ConnectDescriptor*>     ConnectDespList;
typedef QVector<Channel*>               ChannelList;
typedef QVector<Level*>                 LevelList;
typedef QVector<Dot*>                   DotList;

/* Schematic Containers */
class SchematicDevice;
class SchematicTerminal;
class SchematicWire;
class SchematicDot;
typedef QMap<QString, SchematicDevice*>        SDeviceTable;
typedef QVector<SchematicDevice*>              SDeviceList;
typedef QMap<TerminalType, SchematicTerminal*> STerminalTable;
typedef QVector<SchematicTerminal*>            STerminalList;
typedef QVector<SchematicWire*>                SWireList;
typedef QVector<SchematicDot*>                 SDotList;

#endif // NETLISTVIZ_DEFINE_TYPEDEFINE_H
