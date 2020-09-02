#ifndef NETLISTVIZ_SCHEMATIC_SCHEMATIC_DATA_H
#define NETLISTVIZ_SCHEMATIC_SCHEMATIC_DATA_H

/*
 *
 * SchematicData stores SchematicDevice and CktNode.
 * SchematicScene uses this class to render device and node.
 * CktParser uses this class to store circuit infomation.
 * 
 */

#include "Define/Define.h"
#include "Define/TypeDefine.h"

class CktNode;
class SchematicDevice;
class SchematicTextItem;
class SchematicScene;
class ASG;


class SchematicData
{
public:
    SchematicData();
    ~SchematicData();

    /* Add Device and Node */
    void ParseC(QString, QString, QString, double);
    void ParseI(QString, QString, QString, double);
    void ParseL(QString, QString, QString, double);
    void ParseR(QString, QString, QString, double);
    void ParseV(QString, QString, QString, double);

    void AssignDeviceNumber();

    void Clear();
    void PrintNodeAndDevice() const;

    DeviceList GetDeviceList() const { return m_deviceList; }
    void SetFirstLevelDeviceList(const DeviceList &deviceList);
    SchematicDevice* Device(const QString &name) const;
    int FirstLevelDeviceListSize() const  { return m_firstLevelDeviceList.size(); }

    int DeviceCnt() const  { return m_deviceList.size(); }

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

    /* for ASG level 0 */
    DeviceList  m_firstLevelDeviceList;

    friend class SchematicScene;
    friend class ASG;
};

#endif // NETLISTVIZ_SCHEMATIC_SCHEMATIC_DATA_H
