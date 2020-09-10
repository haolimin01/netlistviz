#include "SchematicWire.h"
#include <QPainter>
#include <QStyle>
#include <QStyleOptionGraphicsItem>
#include <QDebug>
#include "Define/Define.h"

static int tempInt = 0;


SchematicWire::SchematicWire(SchematicDevice *startDev, SchematicDevice *endDev,
    TerminalType startTer, TerminalType endTer, QGraphicsItem *parent, QGraphicsScene *scene)
{
    m_startDevice = startDev;
    m_endDevice = endDev;
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

void SchematicWire::SetWirePathPoints(const QVector<QPointF> &points)
{
    m_wirePathPoints.clear();
    // m_wirePathPoints = points;
    /* points contains start and end points, size = 2 */
    Q_ASSERT(points.size() == 2);
    const QPointF &p1 = points.front();
    const QPointF &p2 = points.back();
    GenerateManhattanPathPoints(p1, p2);
}

void SchematicWire::UpdatePosition(SchematicDevice *device, TerminalType terminal, const QPointF &newPos)
{
#ifdef TRACE
    qInfo() << LINE_INFO << endl;
#endif
    QPointF newScenePos = mapFromItem(device, newPos);
    prepareGeometryChange();

    bool isStartPoint = true;

    if (device == m_startDevice AND terminal == m_startTerminal) {
        isStartPoint = true;
    }

    if (device == m_endDevice AND terminal == m_endTerminal) {
        isStartPoint = false;
    }

    QPointF startPoint = newScenePos, endPoint = m_wirePathPoints.back();
    if (NOT isStartPoint) {
        startPoint = m_wirePathPoints.front();
        endPoint = newScenePos;
    }

    m_wirePathPoints.clear();
    GenerateManhattanPathPoints(startPoint, endPoint);
}

void SchematicWire::SetAsBranch(bool branch)
{
    m_isBranch = branch;
}

/* p1 is wire startPoint, p2 is wire endPoint */
void SchematicWire::GenerateManhattanPathPoints(const QPointF &p1, const QPointF &p2)
{
    QPointF startPoint = p1;
    QPointF endPoint = p2;
    if (p1.x() > p2.x()) {
        startPoint = p2;
        endPoint = p1;
    }

    QPointF insertPoint;
    insertPoint.rx() = endPoint.x();
    insertPoint.ry() = startPoint.y();

    m_wirePathPoints.push_back(p1);
    m_wirePathPoints.push_back(insertPoint);
    m_wirePathPoints.push_back(p2);
}

QVariant SchematicWire::itemChange(GraphicsItemChange change, const QVariant &value)
{
#ifdef TRACE
    qInfo() << LINE_INFO << tempInt << endl;
    tempInt++;
#endif
    if (NOT m_startDevice->isSelected())
        m_startDevice->UpdateWirePosition();
    if (NOT m_endDevice->isSelected())
        m_endDevice->UpdateWirePosition();

    return QGraphicsItem::itemChange(change, value);
}
