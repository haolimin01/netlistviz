#include "CktNode.h"
#include <QDebug>
#include <iostream>
#include <sstream>
#include "Schematic/SchematicDevice.h"
#include "Schematic/SchematicScene.h"

CktNode::CktNode(QString nodeName)
{
    m_name = nodeName;
    m_isGnd = false;
    // m_posDevice = nullptr;
    m_posGiven = false;
    // m_dotDevice = new SchematicDevice(SchematicDevice::Dot, nullptr);
}

CktNode::~CktNode()
{
    /* devices will be deleted by SchematicScene */
    m_devices.clear();
    /* dot device will be deleted by SchematicScene */
    // m_dotDevice = nullptr;
}

void CktNode::AddDevice(SchematicDevice *device)
{
    m_devices.append(device);
}

void CktNode::ClearAllDevices()
{
    m_devices.clear();
}

/*
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
*/

void CktNode::SetScenePos(const QPointF &scenePos)
{
    if (NOT m_posGiven) {
        m_pos = scenePos;
        m_posGiven = true;
        qDebug() << scenePos << endl;
    }
}

/* Node just has one scenePos */
/* It connects with some device terminals */
/*
void CktNode::UpdateScenePos(const QPointF &pos)
{
    if (NOT m_posGiven) {
        m_pos = pos;
        m_posGiven = true;
        m_dotDevice->setPos(m_pos);
        m_dotDevice->update();
        return;
    }

    if (pos == m_pos)
        return;

    if (pos.x() == m_pos.x()) {
        m_pos.ry() = (m_pos.y() + pos.y()) / 2;
    }

    if (pos.y() == m_pos.y()) {
        m_pos.rx() = (m_pos.x() + pos.x()) / 2;
    }

    m_dotDevice->setPos(m_pos);
    m_dotDevice->update();
}
*/

/*
void CktNode::AddToScene(QGraphicsScene *scene)
{
    if (m_dotDevice->Placed())
        return;

    scene->addItem(m_dotDevice);
    m_dotDevice->SetPlaced(true);
}
*/

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