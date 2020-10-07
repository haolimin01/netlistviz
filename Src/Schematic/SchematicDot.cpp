#include "SchematicDot.h"
#include "SchematicTerminal.h"
#include <QDebug>
#include <QPainter>

const static qreal DFT_DOT_LEN = 6;
const static qreal BRECT_LEN = 8;

SchematicDot::SchematicDot()
{
    m_terminal = nullptr;
    m_trackCount = 0;
    m_track = -1;
    m_geoCol = 0;
    m_dotLen = DFT_DOT_LEN;
    m_holdColCount = 1;
    m_sceneCol = 0;
    m_sceneRow = 0;

    DrawDot();

    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
}

SchematicDot::~SchematicDot()
{

}

QRectF SchematicDot::boundingRect() const
{
    return QRectF(-BRECT_LEN/2, -BRECT_LEN/2, BRECT_LEN, BRECT_LEN);
}

void SchematicDot::SetScale(qreal newScale)
{
    m_dotLen = DFT_DOT_LEN * newScale;
}

void SchematicDot::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                    QWidget *widget)
{
#ifdef TRACEx
    qInfo() << LINE_INFO << endl;
#endif

    painter->setBrush(QBrush(Qt::black));
    painter->setPen(Qt::NoPen);
    painter->drawPath(path());

    if (isSelected()) {
        painter->setPen(QPen(Qt::black, 1, Qt::DashLine));
        painter->setBrush(QBrush(Qt::NoBrush));
        painter->drawRect(DashRect());
    }
}

void SchematicDot::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    // do something
}

QPainterPath SchematicDot::shape() const
{
    QPainterPath path;
    path.addEllipse(-m_dotLen/2, -m_dotLen/2, m_dotLen, m_dotLen);
    return path;
}

void SchematicDot::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsPathItem::mousePressEvent(event);
}

void SchematicDot::DrawDot()
{
    QPainterPath path;
    path.addEllipse(-m_dotLen/2, -m_dotLen/2, m_dotLen, m_dotLen);
    setPath(path);
}

QRectF SchematicDot::DashRect() const
{
    return QRectF(-BRECT_LEN/2, -BRECT_LEN/2, BRECT_LEN, BRECT_LEN);
}

void SchematicDot::Print() const
{
    QString tmp = "";
    tmp += ("SDot Track(" + QString::number(m_track) + "), ");
    tmp += ("TrackCount(" + QString::number(m_trackCount) + "), ");
    tmp += ("GeoCol(" + QString::number(m_geoCol) + "), ");
    tmp += ("DeviceName(" + m_terminal->DeviceName() + ")");
    qInfo() << tmp;
}
