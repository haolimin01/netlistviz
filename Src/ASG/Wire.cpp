#include "Wire.h"
#include "Circuit/Device.h"
#include "Circuit/Terminal.h"
#include <QDebug>

Wire::Wire(Device *fromDevice, Terminal *fromTerminal,
    Device *toDevice, Terminal *toTerminal)
{
    m_fromDevice = fromDevice;
    m_fromTerminal = fromTerminal;
    m_toDevice = toDevice;
    m_toTerminal = toTerminal;
    m_track = -1;
    m_channelId = 0;
    m_trackGiven = false;
    m_swire = nullptr;
}

Wire::~Wire()
{
#ifdef TRACEx
    qInfo() << LINE_INFO << endl;
#endif

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

bool Wire::HasGCap() const
{
    bool has = false;

    if (m_fromDevice->GroundCap())
        has = true;
    if (NOT has) {
        if (m_toDevice->GroundCap())
            has = true;
    }

    return has;
}

bool Wire::HasCCap() const
{
    bool has = false;

    if (m_fromDevice->CoupledCap())
        has = true;
    if (NOT has) {
        if (m_toDevice->CoupledCap())
            has = true;
    }

    return has;
}

bool Wire::IsHorizontal() const
{
    qreal fromTerRow = m_fromTerminal->LogicalRelRow();
    qreal toTerRow = m_toTerminal->LogicalRelRow();

    if (fromTerRow == toTerRow)
        return true;
    else
        return false;
}

bool Wire::HasCross(Wire *otherWire) const
{
    qreal thisFromTerRow = m_fromTerminal->LogicalRelRow();
    qreal thisToTerRow = m_toTerminal->LogicalRelRow();

    qreal otherFromTerRow = otherWire->m_fromTerminal->LogicalRelRow();
    qreal otherToTerRow = otherWire->m_toTerminal->LogicalRelRow();

    int m1 = (thisFromTerRow - otherFromTerRow) * (thisToTerRow - otherToTerRow);

    if (m1 < 0)
        return true;

    if (m1 == 0) // could be merged
        return false;

    int m2 = (thisFromTerRow - otherToTerRow) * (otherFromTerRow - thisToTerRow);
    if (m2 >= 0)
        return true;
    else
        return false;
}

bool Wire::CouldBeMerged(Wire *otherWire) const
{
    qreal thisFromTerRow = m_fromTerminal->LogicalRelRow();
    qreal thisToTerRow = m_toTerminal->LogicalRelRow();

    qreal otherFromTerRow = otherWire->m_fromTerminal->LogicalRelRow();
    qreal otherToTerRow = otherWire->m_toTerminal->LogicalRelRow();

    qreal m = (thisFromTerRow - otherFromTerRow) * (thisToTerRow - otherToTerRow);

#ifdef DEBUGx
    qInfo() << Name() << "and" << otherWire->Name();
    qInfo() << "F1(" << thisFromTerRow << "), "
            << "T1(" << thisToTerRow << "), "
            << "F2(" << otherFromTerRow << "), "
            << "T2(" << otherToTerRow << "), "
            << "m=" << m;
#endif

    if (m == 0)
        return true;
    else
        return false;
}

bool Wire::CouldBeSameTrack(Wire *otherWire) const
{
    bool hasCross = HasCross(otherWire);

#ifdef DEBUGx
    printf("=============== Wire ===============\n");
    QString tmp = "";
    tmp += ("(" + m_fromDevice->Name() + ", " + m_toDevice->Name() + "), ");
    tmp += ("(" + otherWire->m_fromDevice->Name() + ", " + otherWire->m_toDevice->Name() + ") ");
    tmp += ("cross(" + QString::number(hasCross) + ")");
    qInfo() << tmp;
#endif

    bool merged = CouldBeMerged(otherWire);

    return ((NOT hasCross) || merged);

    return merged;
}

QString Wire::Name() const
{
    QString ret;
    ret = "(" + m_fromDevice->Name() + ", " + m_toDevice->Name() + ")";
    return ret;
}

/* The same terminal between merged wires */
Terminal* Wire::SameTerminal(Wire *otherWire) const
{
    Terminal *terminal = nullptr;

    qreal thisFromTerRow = m_fromTerminal->LogicalRelRow();
    qreal thisToTerRow = m_toTerminal->LogicalRelRow();

    qreal otherFromTerRow = otherWire->m_fromTerminal->LogicalRelRow();
    qreal otherToTerRow = otherWire->m_toTerminal->LogicalRelRow();

    if (thisFromTerRow == otherFromTerRow)
        terminal = m_fromTerminal;
    else if (thisToTerRow == otherToTerRow)
        terminal = m_toTerminal;

#ifdef DEBUGx
    qDebug() << Name() << " and " << otherWire->Name();
    qDebug() << thisFromTerRow << " and " << otherFromTerRow;
    qDebug() << thisToTerRow << " and " << otherToTerRow;
#endif

    Q_ASSERT(terminal);

    return terminal;
}

