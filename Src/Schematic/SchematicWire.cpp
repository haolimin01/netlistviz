#include "SchematicWire.h"
#include <QPainter>
#include <QStyle>
#include <QStyleOptionGraphicsItem>
#include <QDebug>
#include "Define/Define.h"


SchematicWire::SchematicWire(SchematicDevice *startDev, SchematicDevice *endDev,
    int startTer, int endTer, QGraphicsItem *parent, QGraphicsScene *scene)
{
    m_startDev = startDev;
    m_endDev = endDev;
    m_color = Qt::black;
    m_startTerIndex = startTer;
    m_endTerIndex = endTer;
}


SchematicWire::~SchematicWire()
{

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


void SchematicWire::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                        QWidget *)
{
#ifdef TRACE
    qInfo() << LINE_INFO << endl;
#endif
    if (option->state & QStyle::State_Selected)
        painter->setPen(QPen(m_color, 2, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin));
    else
        painter->setPen(QPen(m_color, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    
    painter->setRenderHint(QPainter::Antialiasing);

    QPointF startPoint, endPoint;
    for (int i = 0; i < m_wirePathPoints.size() - 1; ++ i) {
        startPoint = m_wirePathPoints.at(i);
        endPoint = m_wirePathPoints.at(i + 1);
        painter->drawLine(startPoint, endPoint);
    }
}


void SchematicWire::SetWirePathPoints(QVector<QPointF> points)
{
    m_wirePathPoints.clear();
    m_wirePathPoints = points;
}