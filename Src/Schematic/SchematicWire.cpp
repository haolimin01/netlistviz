#include "SchematicWire.h"
#include <QPainter>
#include <QStyle>
#include <QStyleOptionGraphicsItem>
#include <QDebug>
#include "Define/Define.h"


SchematicWire::SchematicWire(SchematicDevice *startDev, SchematicDevice *endDev,
    TerminalType startTer, TerminalType endTer, QGraphicsItem *parent, QGraphicsScene *scene)
{
    m_startDev = startDev;
    m_endDev = endDev;
    m_color = Qt::black;
    m_startTerminal = startTer;
    m_endTerminal = endTer;
    
    m_isBranch = false;
    m_showBranchFlag = false;

    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
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
#ifdef TRACEx
    qInfo() << LINE_INFO << endl;
#endif

    int lineWidth = 2;
    if (m_isBranch && m_showBranchFlag) {
        lineWidth = 3;
        m_color = Qt::red;
    } else {
        m_color = Qt::black;
    }

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

void SchematicWire::SetWirePathPoints(QVector<QPointF> points)
{
    m_wirePathPoints.clear();
    m_wirePathPoints = points;
}

void SchematicWire::UpdatePosition(SchematicDevice *device, TerminalType terminal, const QPointF &newPos)
{
    QPointF scenePos = mapFromItem(device, newPos);
    prepareGeometryChange();
    int size = m_wirePathPoints.size();

    /* Now we just simply connect terminals of start device and end device */
    assert(size == 2);

    if (device == m_startDev AND terminal == m_startTerminal) {
        m_wirePathPoints[0].rx() = scenePos.x();
        m_wirePathPoints[0].ry() = scenePos.y();
        return;
    }

    if (device == m_endDev AND terminal == m_endTerminal) {
        m_wirePathPoints[1].rx() = scenePos.x();
        m_wirePathPoints[1].ry() = scenePos.y();
        return;
    }
}

void SchematicWire::SetAsBranch(bool branch)
{
    m_isBranch = branch;
}