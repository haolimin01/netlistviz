#include "SchematicWire.h"
#include "ASG/Wire.h"
#include "SchematicTerminal.h"
#include "SchematicDevice.h"
#include <QDebug>
#include <QStyleOptionGraphicsItem>
#include <QPainter>

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

    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
}

QRectF SchematicWire::boundingRect() const
{
    qreal minX, minY, maxX, maxY;
    if (m_wirePathPoints.isEmpty())
        return QRectF(0, 0, 0, 0);
    
    minX = maxX = m_wirePathPoints.at(0).x();
    minY = maxY = m_wirePathPoints.at(0).y();

    foreach (QPointF point, m_wirePathPoints) {
        minX = qMin(minX, point.x());
        maxX = qMax(maxX, point.x());
        minY = qMin(minY, point.y());
        maxY = qMax(maxY, point.y());
    }

    return QRectF(minX, minY, maxX - minX, maxY - minY);
}

void SchematicWire::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
    int lineWidth = 2;

    if (option->state & QStyle::State_Selected)
        painter->setPen(QPen(m_color, lineWidth, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin));
    else
        painter->setPen(QPen(m_color, lineWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    
    painter->setRenderHint(QPainter::Antialiasing);

    QPointF startPoint, endPoint;
    for (int i = 0; i < m_wirePathPoints.size() - 1; ++ i) {
        startPoint = m_wirePathPoints.at(i);
        endPoint = m_wirePathPoints.at(i + 1);
        painter->drawLine(startPoint, endPoint);
    }
}

void SchematicWire::SetWirePathPoints(const QVector<QPointF> &wirePoints)
{
    m_wirePathPoints.clear();
    /* points contains start and end points, size = 2 */
    m_wirePathPoints = wirePoints;
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