#include "SchematicDevice.h"
#include <QPainter>
#include <QPen>
#include <QDebug>
#include "SchematicNode.h"


SchematicDevice::SchematicDevice(DeviceType type, SchematicNode *startNode, SchematicNode *endNode, 
				            QGraphicsItem *parent)
    : m_deviceType(type), QGraphicsLineItem(parent)
{
    m_startNode = startNode;
    m_endNode = endNode;
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    m_contextMenu = nullptr;
    m_color = GetColorFromDeviceType(m_deviceType);
    setPen(QPen(m_color, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
}


SchematicDevice::~SchematicDevice()
{

}


QPixmap SchematicDevice::GetImage() const
{
    switch (m_deviceType) {
        case Resistor:  return QPixmap(":images/res.png");
        case Capacitor: return QPixmap(":images/cap.png");
        case Inductor:  return QPixmap(":images/ind.png");
        case Isrc:      return QPixmap(":images/isrc.png");
        case Vsrc:      return QPixmap(":images/vsrc.png");
        default:;
    }
}


int SchematicDevice::GetStartNodeId() const
{
    assert(m_startNode);
    return m_startNode->GetId();
}


int SchematicDevice::GetEndNodeId() const
{
    assert(m_endNode);
    return m_endNode->GetId();
}


void SchematicDevice::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    // do nothing now.
}


QRectF SchematicDevice::boundingRect() const
{
    qreal extra = (pen().width() + 20) / 2.0;

    return QRectF(line().p1(), QSizeF(line().p2().x() - line().p1().x(),
                                      line().p2().y() - line().p1().y()))
        .normalized()
        .adjusted(-extra, -extra, extra, extra);
}


// QPainterPath SchematicDevice::shape() const
// {
//     return nullptr;
// }


void SchematicDevice::UpdatePosition()
{
    QLineF line(mapFromItem(m_startNode, 0, 0), mapFromItem(m_endNode, 0, 0));
    setLine(line);
}


void SchematicDevice::paint(QPainter *painter, const QStyleOptionGraphicsItem *,
                  QWidget *)
{
    QPen tPen = pen();
    tPen.setColor(m_color);
    painter->setPen(tPen);
    painter->setBrush(m_color);
    setLine(QLineF(m_startNode->pos(), m_endNode->pos()));
    painter->drawLine(line());

    if (isSelected()) {
        painter->setPen(QPen(m_color, 1, Qt::DashLine));
        QLineF myLine = line();
        myLine.translate(0, 4.0);
        painter->drawLine(myLine);
        myLine.translate(0,-8.0);
        painter->drawLine(myLine);
    }
}


QColor SchematicDevice::GetColorFromDeviceType(DeviceType type)
{
    QColor color = Qt::black;
    switch (type) {
        case Resistor:  color = Qt::black;  break;
        case Capacitor: color = Qt::blue;   break;
        case Inductor:  color = Qt::yellow; break;
        case Isrc:      color = Qt::green;  break;
        case Vsrc:      color = Qt::red;    break;
        default:;
    }
    return color;
}


 void SchematicDevice::Print() const
 {
    qInfo().noquote().nospace() << m_name << " type(" << m_deviceType << ") posName("
            << m_startNode->GetName() << ") negName("
            << m_endNode->GetName() << ") value(" << m_value << ")" << endl;
 }

