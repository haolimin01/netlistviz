#include "Wire.h"
#include "Circuit/Device.h"
#include "Circuit/Terminal.h"

Wire::Wire(Device *fromDevice, Terminal *fromTerminal,
    Device *toDevice, Terminal *toTerminal)
{
    m_fromDevice = fromDevice;
    m_fromTerminal = fromTerminal;
    m_toDevice = toDevice;
    m_toTerminal = toTerminal;
    m_track = -1;
    m_channelId = -1;
    m_trackGiven = false;
}

Wire::~Wire()
{

}

SchematicDevice* Wire::FromSDevice() const
{
    SchematicDevice *dev = m_fromDevice->GetSchematicDevice();
    Q_ASSERT(dev);
    return dev;
}

SchematicDevice* Wire::ToSDevice() const
{
    SchematicDevice *dev = m_toDevice->GetSchematicDevice();
    Q_ASSERT(dev);
    return dev;
}

SchematicTerminal* Wire::FromSTerminal() const
{
    SchematicTerminal* ter = m_fromTerminal->GetSchematicTerminal();
    Q_ASSERT(ter);
    return ter;
}

SchematicTerminal* Wire::ToSTerminal() const
{
    SchematicTerminal* ter = m_toTerminal->GetSchematicTerminal();
    Q_ASSERT(ter);
    return ter;
}
