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
    m_posGiven = false;
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

void CktNode::SetScenePos(const QPointF &scenePos)
{
    if (NOT m_posGiven) {
        m_pos = scenePos;
        m_posGiven = true;
        // qDebug() << scenePos << endl;
    }
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