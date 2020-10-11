#ifndef NETLISTVIZ_CIRCUIT_NODE_H
#define NETLISTVIZ_CIRCUIT_NODE_H

/*
 * @filename : Node.h
 * @ahthor   : Hao Limin
 * @date     : 2020.09.11
 * @email    : haolimin01@sjtu.edu.cn
 * @desp     : netlist file's Node, add to device after converted to Terminal.
 *           : Node is unique, related to some terminals.
 */

#include "Define/Define.h"
#include "Define/TypeDefine.h"
#include <QString>

class Device;

class Node
{
public:
    explicit Node(QString name);
    ~Node();

    void       SetId(int id)   { m_id = id; }
    int        Id() const      { return m_id; }
    void       SetGnd(bool is) { m_isGnd = is; }
    bool       IsGnd() const   { return m_isGnd; }
    void       AddDevice(Device *device);
    DeviceList ConnectDeviceList() const { return m_deviceList; }
    QString    Name() const    { return m_name; }

    void Print() const;

private:
    DISALLOW_COPY_AND_ASSIGN(Node);

    QString     m_name;
    int         m_id;
    bool        m_isGnd;
    DeviceList  m_deviceList;
};

#endif // NETLISTVIZ_CIRCUIT_NODE_H
