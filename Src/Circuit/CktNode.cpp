#include "CktNode.h"

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

}