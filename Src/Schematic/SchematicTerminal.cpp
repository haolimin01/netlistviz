#include "SchematicTerminal.h"
#include "Circuit/Node.h"
#include "SchematicDevice.h"
#include "SchematicWire.h"

SchematicTerminal::SchematicTerminal()
{
    m_type = Positive;
    m_device = nullptr;
    m_node = nullptr;
    m_id = 0;
}

SchematicTerminal::SchematicTerminal(TerminalType type, SchematicDevice *device)
{
    Q_ASSERT(device);
    m_type = type;
    m_device = device;
    m_node = nullptr;
    m_id = 0;
}

SchematicTerminal::~SchematicTerminal()
{
    /* delete node here */
    // if (m_node)
    //     delete m_node;
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

void SchematicTerminal::AddWire(SchematicWire *wire)
{
    m_wires.push_back(wire);
}

void SchematicTerminal::RemoveWire(SchematicWire *wire)
{
    int wireIndex = m_wires.indexOf(wire);
    if (wireIndex != -1)
        m_wires.removeAt(wireIndex);
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
