#ifndef NETLISTVIZ_CIRCUIT_CIRCUITGRAPH_H
#define NETLISTVIZ_CIRCUIT_CIRCUITGRAPH_H

/*
 * @filename : CircuitGraph.h
 * @author   : Hao Limin 
 * @date     : 2020.09.11
 * @email    : haolimin01@sjtu.edu.cn
 * @desp     : Store circuit as graph.
 */

#include "Define/Define.h"
#include "Define/TypeDefine.h"
#include <QVector>

class Terminal;

class CircuitGraph
{
public:
    CircuitGraph();
    ~CircuitGraph();

    /* For Inserting Devices */
    int InsertR(QString, QString, QString, double);
    int InsertC(QString, QString, QString, double);
    int InsertL(QString, QString, QString, double);
    int InsertI(QString, QString, QString, double);
    int InsertV(QString, QString, QString, double);

    int         DeviceCount() const { return m_deviceList.size(); }
    DeviceList  GetDeviceList() const { return m_deviceList; }
    Device*     GetDevice(const QString &name) const;
    void        SetFirstLevelDeviceList(const DeviceList &devList);
    DeviceList  FirstLevelDeviceList() const { return m_firstLevelDeviceList; }
    int         FirstLevelDeviceListSize() const { return m_firstLevelDeviceList.size(); }

    void Clear();
    void DestroyAllNodes();
    void PrintCircuit() const;

private:
    DISALLOW_COPY_AND_ASSIGN(CircuitGraph);

    Terminal* InsertNode(const QString &name, Device *device);
    Terminal* CreateTerminal(Node *node);
    bool      IsGnd(const QString &name) const;


    /* For Inserting */
    DeviceTable   m_deviceTable;
    NodeTable     m_nodeTable;
    int           m_nodeNumber;
    int           m_deviceNumber;
    int           m_terminalNumber;

    /* For ASG */
    DeviceList    m_deviceList;
    DeviceList    m_firstLevelDeviceList;
};

#endif // NETLISTVIZ_CIRCUIT_CIRCUITGRAPH_H
