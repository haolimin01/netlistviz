#include "CktNode.h"
#include <QDebug>
#include <iostream>
#include <sstream>
#include "Schematic/SchematicDevice.h"

CktNode::CktNode(QString nodeName)
{
    m_name = nodeName;
    m_isGnd = false;
    m_posDevice = nullptr;
}

CktNode::~CktNode()
{
    /* devices will be deleted by SchematicScene */
    m_devices.clear();
}

void CktNode::AddDevice(SchematicDevice *device)
{
    m_devices.append(device);
}

void CktNode::ClearAllDevices()
{
    m_devices.clear();
}

void CktNode::SetScenePos(const QPointF &pos, SchematicDevice *device)
{
    if (NOT m_posDevice) {
        m_posDevice = device;
        m_pos = pos;
        return;
    }

    if (m_posDevice != device) {
        if (m_posDevice->GetDeviceType() == device->GetDeviceType())
            return;
        
        SchematicDevice::DeviceType t = m_posDevice->GetDeviceType();
        if (t == SchematicDevice::Vsrc || t == SchematicDevice::Isrc) {
            m_posDevice = device;
            m_pos = pos;
            return;
        }
        return;
    }
    
    m_pos = pos;
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