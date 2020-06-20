#ifndef NETLISTVIZ_SCHEMATIC_SCHEMATIC_DATA_H
#define NETLISTVIZ_SCHEMATIC_SCHEMATIC_DATA_H

/*
 *
 * SchematicData stores SchematicDevice and SchematicNode.
 * SchematicScene uses this class to render device and node.
 * CktParser uses this class to store circuit infomation.
 * 
 */

#include <QMap>
#include <QVector>
#include <QString>
#include "Define/Define.h"

class SchematicNode;
class SchematicDevice;
class SchematicLayout;
class SchematicScene;


class SchematicData
{
public:
    typedef QMap<QString, SchematicNode *>    NodeTable;
    typedef QMap<QString, SchematicDevice *>  DeviceTable;
    typedef QVector<SchematicNode *>          NodeList;
    typedef QVector<SchematicDevice *>        DeviceList;

public:
    SchematicData();
    ~SchematicData();

    /* Add Device and Node */
    void ParseC(QString, QString, QString, double);
    void ParseI(QString, QString, QString, double);
    void ParseL(QString, QString, QString, double);
    void ParseR(QString, QString, QString, double);
    void ParseV(QString, QString, QString, double);

    void PrintNodeAndDevice() const;

private:
    DISALLOW_COPY_AND_ASSIGN(SchematicData);

    /* If node name exists, return it's pointer */
    /* else create node and return it's pointer */
    SchematicNode* GetAddNode(const QString &name);
    bool           IsGnd(const QString &name) const;

    NodeTable   m_nodeTable;
    DeviceTable m_deviceTable;

    /* Record node and device order in netlist */
    NodeList    m_nodeList;
    DeviceList  m_deviceList;

    /* node number */
    int         m_nodeNumber;

    friend class SchematicLayout;
    friend class SchematicScene;
};

#endif // NETLISTVIZ_SCHEMATIC_SCHEMATIC_DATA_H
