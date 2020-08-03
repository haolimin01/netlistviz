#include "CktNode.h"
#include <QDebug>
#include <iostream>
#include <sstream>
#include "Schematic/SchematicDevice.h"

CktNode::CktNode(QString nodeName)
{
    m_name = nodeName;
}

CktNode::~CktNode()
{

}

void CktNode::AddDevice(SchematicDevice *device)
{
    m_devices.append(device);
}

void CktNode::ClearAllDevices()
{
    m_devices.clear();
}

void CktNode::Print() const
{
    std::stringstream ss;
    ss << "Name(" << m_name.toStdString() << "), ";
    ss << "id(" << m_id << "), ";
    ss << "Devices( ";

    foreach (SchematicDevice *device, m_devices) {
        ss << device->Name().toStdString() << " ";
    }
    ss << ")";

    std::cout << ss.str() << std::endl;
}