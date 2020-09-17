#include "SchematicTerminal.h"
#include "Circuit/Terminal.h"
#include "Circuit/Node.h"
#include "SchematicDevice.h"

SchematicTerminal::SchematicTerminal(Terminal *terminal)
{
    Q_ASSERT(terminal);
    m_node = terminal->m_node;
    m_id = terminal->m_id;
    m_type = terminal->m_type;

    m_device = nullptr;
}

SchematicTerminal::SchematicTerminal(TerminalType type, SchematicDevice *device)
{
    Q_ASSERT(device);
    m_type = type;
    m_device = device;
}

SchematicTerminal::~SchematicTerminal()
{
    /* delete node here */
    if (m_node)
        delete m_node;
}

void SchematicTerminal::SetDevice(SchematicDevice *device)
{
    m_device = device;
}

bool SchematicTerminal::ConnectToGnd() const
{
    bool is = false;
    if (m_node) {
        if (m_node->Id() == 0)
           is = true; 
    } else {
        is = false;
    }

    return is;
}

QPointF SchematicTerminal::ScenePos() const
{
    if (NOT m_device)
        return QPointF(0, 0);
    else {
        return m_device->mapToScene(m_rect.center());
    }
}

QString SchematicTerminal::PrintString() const
{
    QString result;
    result += ("id(" + QString::number(m_id) + "), ");
    result += ("type(" + QString::number(m_type) + "), ");
    result += ("nodeName(" + m_node->Name() + "), ");
    result += ("belongTo(" + m_device->Name() + ")");
    return result;
}
