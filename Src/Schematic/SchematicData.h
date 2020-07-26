#ifndef NETLISTVIZ_SCHEMATIC_SCHEMATIC_DATA_H
#define NETLISTVIZ_SCHEMATIC_SCHEMATIC_DATA_H

/*
 *
 * SchematicData stores SchematicDevice and CktNode.
 * SchematicScene uses this class to render device and node.
 * CktParser uses this class to store circuit infomation.
 * 
 */

#include <QMap>
#include <QVector>
#include <QString>
#include "Define/Define.h"

class CktNode;
class SchematicDevice;
class SchematicTextItem;
class SchematicLayout;
class SchematicScene;


class SchematicData
{
public:
    typedef QMap<QString, CktNode *>          NodeTable;
    typedef QMap<QString, SchematicDevice *>  DeviceTable;
    typedef QVector<CktNode *>                NodeList;
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
    CktNode* GetAddNode(const QString &name);
    bool     IsGnd(const QString &name) const;

    /* Only use in CktParser */
    /* Do not contain the complete data */
    NodeTable   m_nodeTable;
    DeviceTable m_deviceTable;

    /* Record node and device order in netlist */
    NodeList    m_nodeList;
    DeviceList  m_deviceList;

    /* node number */
    int         m_nodeNumber;

    friend class SchematicScene;
};

#endif // NETLISTVIZ_SCHEMATIC_SCHEMATIC_DATA_H
