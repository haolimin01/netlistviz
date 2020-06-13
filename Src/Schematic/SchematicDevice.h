#ifndef NETLISTVIZ_SCHEMATIC_SCHEMATICDEVICE_H
#define NETLISTVIZ_SCHEMATIC_SCHEMATICDEVICE_H

#include <QGraphicsPathItem>
#include <QGraphicsLineItem>

QT_BEGIN_NAMESPACE
class QGraphicsPixmapItem;
QT_END_NAMESPACE;

class SchematicNode;


class SchematicDevice : public QGraphicsLineItem
{
public:
	enum { Type = UserType + 4 };
	enum DeviceType {Resistor=0, Capacitor, Inductor,
					 Vsrc/*3*/, Isrc};

public:
	SchematicDevice(DeviceType type, SchematicNode *startNode, SchematicNode *endNode, 
				QGraphicsItem *parent = nullptr);
	~SchematicDevice();

	QPixmap GetImage() const;

	QRectF boundingRect() const override;
	// QPainterPath shape() const override;

	SchematicNode* GetStartNode() const { return m_startNode; }
	SchematicNode* GetEndNode()   const { return m_endNode; }

	int type() const override { return Type; }
	void UpdatePosition();

protected:
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
			QWidget *widget = nullptr) override;

private:
	void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;

	SchematicNode  *m_startNode;
	SchematicNode  *m_endNode;

	DeviceType      m_deviceType;
	QMenu          *m_contextMenu;
};

#endif // NETLISTVIZ_SCHEMATIC_SCHEMATICDEVICE_H
