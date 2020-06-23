#include "SchematicNode.h"
#include <QPainterPath>
#include <QPainter>
#include <QTransform>
#include <QGraphicsScene>
#include <QDebug>
#include "SchematicDevice.h"

const qreal NodeSize = 20;


SchematicNode::SchematicNode(QMenu *contextMenu, QTransform itemTransform,
							QGraphicsItem *parent)
	: QGraphicsPathItem(parent)
{
	InitVariables();

	setTransform(itemTransform);
	m_contextMenu = contextMenu;

	// DrawNode();
	m_color = Qt::black;

	setFlag(QGraphicsItem::ItemIsMovable, true);
	setFlag(QGraphicsItem::ItemIsSelectable, true);
	setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
}


SchematicNode::SchematicNode()
{
	InitVariables();
	setTransform(QTransform());
	m_contextMenu = nullptr;

	// DrawNode();

	setFlag(QGraphicsItem::ItemIsMovable, true);
	setFlag(QGraphicsItem::ItemIsSelectable, true);
	setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
}


SchematicNode::~SchematicNode()
{

}


void SchematicNode::InitVariables()
{
	m_nodeSize = NodeSize;
	m_id = -1;
	m_name = "";
	m_isGnd = false;
}


void SchematicNode::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
	// empty
}


QPixmap SchematicNode::GetImage() const
{
	QPixmap image(":images/schematicnode.png");
	return image;
}


void SchematicNode::AddDevice(SchematicDevice *device)
{
	m_devices.append(device);
}


void SchematicNode::RemoveDevice(SchematicDevice *device)
{
	m_devices.removeAll(device);
}


void SchematicNode::RemoveDevices()
{
	const auto devicesCopy = m_devices;
	for (SchematicDevice *device : devicesCopy) {
		device->GetStartNode()->RemoveDevice(device);
		device->GetEndNode()->RemoveDevice(device);
		scene()->removeItem(device);
		delete device;
	}
}


void SchematicNode::Print() const
{
	qInfo().nospace().noquote() << m_name << " id(" << m_id << ") isGnd("
		<< m_isGnd << ")" << endl;

}


void SchematicNode::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
	QPen tPen = pen();
	tPen.setColor(m_color);
	painter->setPen(tPen);
	painter->setBrush(m_color);

	QPainterPath path;
	qreal startX = - m_nodeSize / 2;
	qreal startY = - m_nodeSize / 2;
	path.addEllipse(startX, startY, m_nodeSize, m_nodeSize);
	setPath(path);
	painter->drawPath(path);

	if (isSelected()) {
		painter->setPen(QPen(Qt::black, 1, Qt::DashLine));
		painter->setBrush(Qt::NoBrush);
		painter->drawRect(startX, startY, m_nodeSize, m_nodeSize);
	}

}