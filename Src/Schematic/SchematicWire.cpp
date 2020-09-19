#include "SchematicWire.h"
#include "ASG/Wire.h"
#include "SchematicTerminal.h"
#include "SchematicDevice.h"
#include <QDebug>

SchematicWire::SchematicWire(Wire *wire, QGraphicsItem *parent, QGraphicsScene *scene)
    : QGraphicsItem(parent)
{
    m_startDevice = wire->FromSDevice();
    m_endDevice = wire->ToSDevice();
    m_startTerminal = wire->FromSTerminal();
    m_endTerminal = wire->ToSTerminal();

    m_logCol = wire->ChannelId();
    m_track = wire->Track();

    Q_UNUSED(scene);

    Initialize();
}

SchematicWire::SchematicWire(SchematicDevice *startDevice, SchematicDevice *endDevice,
    SchematicTerminal *startTer, SchematicTerminal *endTer, QGraphicsItem *parent, QGraphicsScene *scene)
    : QGraphicsItem(parent)
{
    m_startDevice = startDevice;
    m_endDevice = endDevice;
    m_startTerminal = startTer;
    m_endTerminal = endTer;

    m_logCol = 0;
    m_track = 0;

    Q_UNUSED(scene);

    Initialize();
}

SchematicWire::~SchematicWire()
{

}

void SchematicWire::Initialize()
{
    m_color = Qt::black;
    m_geoCol = 0;
    m_thisChannelTrackCount = 0;
}

QRectF SchematicWire::boundingRect() const
{

}

void SchematicWire::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{

}

void SchematicWire::Print() const
{
    printf("-------------------- Wire --------------------\n");
    QString tmp;
    tmp += ("(" + m_startDevice->Name() + " " + m_endDevice->Name() + "), ");
    tmp += ("logCol(" + QString::number(m_logCol) + "), ");
    tmp += ("track(" + QString::number(m_track) + "), ");
    tmp += ("geoCol(" + QString::number(m_geoCol) + "), ");
    tmp += ("thisChannelTrackCount(" + QString::number(m_thisChannelTrackCount) + ")");
    qInfo() << tmp;
    printf("----------------------------------------------\n");
}