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

/* Circuit Containers */
class Device;
class Node;
class Terminal;
class Wire;
class QRectF;
typedef QMap<QString, Device*>          DeviceTable;   
typedef QMap<QString, Node*>            NodeTable;
typedef QVector<Device*>                DeviceList;
typedef QMap<TerminalType, Terminal*>   TerminalTable;
typedef QVector<Terminal*>              TerminalList;
typedef QVector<Wire*>                  WireList;

/* Schematic Containers */
class SchematicDevice;
class SchematicTerminal;
typedef QMap<QString, SchematicDevice*>        SDeviceTable;
typedef QVector<SchematicDevice*>              SDeviceList;
typedef QMap<TerminalType, SchematicTerminal*> STerminalTable;
typedef QVector<SchematicTerminal*>            STerminalList;

#endif // NETLISTVIZ_DEFINE_TYPEDEFINE_H
