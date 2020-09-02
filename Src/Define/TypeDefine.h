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

class CktNode;
class SchematicDevice;

/* Containers */
typedef QMap<QString, CktNode*>         NodeTable;
typedef QMap<QString, SchematicDevice*> DeviceTable;
typedef QVector<CktNode*>               NodeList;
typedef QVector<SchematicDevice*>       DeviceList;

/* Iterators */
typedef NodeTable::iterator              NodeTableIter;
typedef NodeTable::const_iterator        NodeTableCIter;
typedef DeviceTable::iterator            DeviceTableIter;
typedef DeviceTable::const_iterator      DeviceTableCIter;
typedef NodeList::iterator               NodeListIter;
typedef NodeList::const_iterator         NodeListCIter;
typedef DeviceList::iterator             DeviceListIter;
typedef DeviceList::const_iterator       DeviceListCIter;

#endif // NETLISTVIZ_DEFINE_TYPEDEFINE_H