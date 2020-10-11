#include "Dot.h"
#include "Circuit/Terminal.h"
#include <QString>

Dot::Dot(int channelId, int track, Terminal *terminal)
{
    m_channelId = channelId;
    m_track = track;
    m_terminal = terminal;

    m_geoCol = 0;
}

Dot::Dot(const Dot& otherDot)
{
    m_channelId = otherDot.m_channelId;
    m_geoCol = otherDot.m_geoCol;
    m_track = otherDot.m_track;
    m_terminal = otherDot.m_terminal;
}

Dot::~Dot()
{
    m_wires.clear();
}

int Dot::TerminalId() const
{
    return m_terminal->Id();
}

QString Dot::DeviceName() const
{
    return m_terminal->DeviceName();
}

SchematicTerminal* Dot::STerminal() const
{
    return m_terminal->GetSchematicTerminal();
}

void Dot::AddWire(Wire *wire)
{
    Wire *existWire = m_wires.value(wire, nullptr);
    if (NOT existWire)
        m_wires.insert(wire, wire);
}

WireList Dot::Wires() const
{
    WireList ret;
    foreach (Wire *wire, m_wires.keys())
        ret.push_back(wire);

    return ret;
}
