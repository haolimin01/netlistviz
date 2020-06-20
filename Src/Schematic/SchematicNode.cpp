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

	DrawNode();

	setFlag(QGraphicsItem::ItemIsMovable, true);
	setFlag(QGraphicsItem::ItemIsSelectable, true);
	setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
}


SchematicNode::SchematicNode()
{
	InitVariables();
	setTransform(QTransform());
	m_contextMenu = nullptr;

	DrawNode();

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


void SchematicNode::DrawNode()
{
	QPainterPath path;
	qreal startX = - m_nodeSize / 2;
	qreal startY = - m_nodeSize / 2;
	path.addEllipse(startX, startY, m_nodeSize, m_nodeSize);
	setPath(path);
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


void SchematicNode::paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *)
{
	qInfo() << "Node paint" << endl;
}