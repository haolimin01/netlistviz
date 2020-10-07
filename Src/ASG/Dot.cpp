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
