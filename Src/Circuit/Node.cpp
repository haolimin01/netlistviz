#include "Node.h"
#include <sstream>
#include <iostream>
#include <QDebug>
#include "Circuit/Device.h"

Node::Node(QString name)
{
    m_name = name;
    m_id = -1;
    m_isGnd = false;
}

Node::~Node()
{
#ifdef TRACE
    qInfo() << LINE_INFO << "deleting node " << m_name << endl;
#endif
    m_deviceList.clear();
}

void Node::AddDevice(Device *device)
{
    m_deviceList.push_back(device);
}

void Node::Print() const
{
    std::stringstream ss;
    ss << "Name(" << m_name.toStdString() << "), ";
    ss << "id(" << m_id << "), ";
    ss << "Devices( ";

    foreach (Device *device, m_deviceList)
        ss << device->Name().toStdString() << " ";
    ss << ")";

    std::cout << ss.str() << std::endl;
}