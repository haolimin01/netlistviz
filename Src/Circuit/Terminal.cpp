#include "Terminal.h"
#include <assert.h>
#include <sstream>
#include <iostream>
#include <QDebug>
#include "Node.h"
#include "Device.h"

Terminal::Terminal(Node *node)
{
    assert(node);
    m_node = node;
    m_id = -1;
    m_device = nullptr;
    m_type = General;
    m_sTerminal = nullptr;
}

Terminal::~Terminal()
{
#ifdef TRACEx
    qInfo() << LINE_INFO << "deleting terminal" << endl;
#endif
    m_node = nullptr;
}

int Terminal::NodeId() const
{
    return m_node->Id();
}

bool Terminal::NodeIsGnd() const
{
    return m_node->IsGnd();
}

void Terminal::Print() const
{
    std::stringstream ss;
    ss << "id(" << m_id << "), ";
    ss << "node(" << m_node->Name().toStdString() << "), ";
    ss << "device(" << m_device->Name().toStdString() << "), ";
    ss << "type(" << m_type << ")";

    std::cout << ss.str() << std::endl;
}