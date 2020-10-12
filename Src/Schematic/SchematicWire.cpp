#include "SchematicWire.h"
#include "SchematicTerminal.h"
#include "SchematicDevice.h"
#include <QDebug>
#include <QStyleOptionGraphicsItem>
#include <QPainter>

SchematicWire::SchematicWire(SchematicDevice *startDevice, SchematicDevice *endDevice,
    SchematicTerminal *startTer, SchematicTerminal *endTer, QGraphicsItem *parent, QGraphicsScene *scene)
    : QGraphicsItem(parent)
{
    m_startDevice = startDevice;
    m_endDevice = endDevice;
    m_startTerminal = startTer;
    m_endTerminal = endTer;

    m_track = 0;
    m_sceneCol = 0;
    m_trackCount = 0;
    m_holdColCount = 1;

    Q_UNUSED(scene);

    Initialize();
}

SchematicWire::~SchematicWire()
{

}

void SchematicWire::Initialize()
{
    m_color = Qt::black;
    m_lineWidth = DFT_Wire_W;

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
#ifdef TRACEx
    qInfo() << LINE_INFO << endl;
#endif

    if (option->state & QStyle::State_Selected)
        painter->setPen(QPen(m_color, m_lineWidth, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin));
    else
        painter->setPen(QPen(m_color, m_lineWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    
    painter->setRenderHint(QPainter::Antialiasing);

    QPointF startPoint, endPoint;
    for (int i = 0; i < m_wirePathPoints.size() - 1; ++ i) {
        startPoint = m_wirePathPoints.at(i);
        endPoint = m_wirePathPoints.at(i + 1);
        painter->drawLine(startPoint, endPoint);
    }
}

QVariant SchematicWire::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (NOT m_startDevice->isSelected())
        m_startDevice->UpdateWirePosition();
    if (NOT m_endDevice->isSelected())
        m_endDevice->UpdateWirePosition();

    return QGraphicsItem::itemChange(change, value);
}

void SchematicWire::SetWirePathPoints(const QVector<QPointF> &wirePoints)
{
    m_wirePathPoints.clear();
    /* points contains start and end points, size = 2 */
    m_wirePathPoints = wirePoints;
}

void SchematicWire::UpdatePosition(SchematicTerminal *terminal)
{
    QPointF newScenePos = terminal->ScenePos();
    prepareGeometryChange();

    bool isStartPoint = true;
    if (terminal == m_endTerminal)
        isStartPoint = false;
    
    QPointF startPoint = newScenePos, endPoint = m_wirePathPoints.back();
    if (NOT isStartPoint) {
        startPoint = m_wirePathPoints.front();
        endPoint = newScenePos;
    }

    if (isStartPoint)
        m_wirePathPoints.replace(0, startPoint);
    else
        m_wirePathPoints.replace(m_wirePathPoints.size() - 1, endPoint);
}

bool SchematicWire::HasGroundCap() const
{
    bool has = ((m_startDevice->GroundCap()) || (m_endDevice->GroundCap()));
    return has;
}

bool SchematicWire::HasCoupledCap() const
{
    bool has = ((m_startDevice->CoupledCap()) || (m_endDevice->CoupledCap()));
    return has;
}

void SchematicWire::Print() const
{
    printf("-------------------- Wire --------------------\n");
    QString tmp;
    tmp += ("(" + m_startDevice->Name() + " " + m_endDevice->Name() + "), ");
    tmp += ("track(" + QString::number(m_track) + "), ");
    tmp += ("thisChannelTrackCount(" + QString::number(m_trackCount) + "), ");
    qInfo() << tmp;
    printf("----------------------------------------------\n");
}
