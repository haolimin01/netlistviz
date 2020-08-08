#ifndef NETLISTVIZ_SCHEMATIC_SCHEMATICDEVICE_H
#define NETLISTVIZ_SCHEMATIC_SCHEMATICDEVICE_H

#include <QGraphicsPathItem>

QT_BEGIN_NAMESPACE
class QGraphicsPixmapItem;
class QGraphicsSceneMouseEvent;
QT_END_NAMESPACE;

class CktNode;
class SchematicWire;

enum NodeType    { Positive, Negative };

class SchematicDevice : public QGraphicsPathItem
{
public:
	enum { Type = UserType + 4 };
	enum DeviceType { Resistor=0, Capacitor, Inductor,
					 Vsrc/*3*/, Isrc };
	enum Orientation { Horizontal, Vertical };

public:
	SchematicDevice(DeviceType type, QMenu *contextMenu,
				QTransform itemTransform = QTransform(), QGraphicsItem *parent = nullptr);

	~SchematicDevice();

	QPixmap    Image();
	QRectF     boundingRect() const override;

	CktNode*   Node(NodeType type) const;
	void       AddNode(NodeType type, CktNode *node);
	int        NodeId(NodeType type) const;
	QPointF    NodePos(NodeType type) const;
	void       SetId(int id) { m_id = id; m_idGiven = true; }
	int        Id()  const { return m_id; }
	bool       IdGiven() const { return m_idGiven; }
	NodeType   GetNodeType(CktNode *node) const;
	void       SetOrientation(Orientation orien);
	void       SetSceneXY(int x, int y);
	int        SceneX() const  { return m_sceneX; }
	int        SceneY() const  { return m_sceneY; }
	bool       Placed() const  { return m_placed; }
	void       SetPlaced(bool placed)  { m_placed = placed; }

	DeviceType GetDeviceType() const { return m_deviceType; }
	int        type() const override { return Type; }
	QColor     Color() const  { return m_color; }
	void       SetName(QString name) { m_name = name; }
	void       SetValue(double value) { m_value = value; }
    QString    Name() const { return m_name; }
	double     Value() const { return m_value; }
	void       SetContextMenu(QMenu *contextMenu) { m_contextMenu = contextMenu; }
	void       SetShowNodeFlag(bool show = true)  { m_showNodeFlag = show; }
	void       AddWire(SchematicWire *wire, NodeType type);
	void       RemoveWires(bool deletion = true);
	void       RemoveWire(SchematicWire *wire, NodeType type);
	bool       TerminalsContain(const QPointF &scenePos) const;

	QMap<NodeType, QRectF> TerminalRects() const  { return m_terRects; }
	void Print() const;

protected:
	void         paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
			     	QWidget *widget = nullptr) override;
	void         contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
	QVariant     itemChange(GraphicsItemChange change, const QVariant &value) override;
	QPainterPath shape() const override;
	void         mousePressEvent(QGraphicsSceneMouseEvent *event) override;

private:
	void   InitVariables();
	QRectF DashRect() const;
	void   UpdateWirePosition();

	void DrawResistor();
	void DrawCapacitor();
	void DrawInductor();
	void DrawIsrc();
	void DrawVsrc();

	QMap<NodeType, CktNode *> m_terminals;
	QMap<NodeType, QVector<SchematicWire*>> m_wiresAtTerminal;

	DeviceType      m_deviceType;
	QMenu          *m_contextMenu;

	QColor			m_color;      // device color
	Orientation     m_devOrien;   // device orientation
	int             m_terNumber;  // terminal number
	QPixmap        *m_imag;       // device image
	bool            m_showNodeFlag;
	int             m_id;         // device id, for creating incidence matrix.
	bool            m_idGiven;

	QMap<NodeType, QRectF> m_terRects;

	/* For CktParser */
	QString         m_name;       // device value
	double          m_value;      // device name

	/* Grid position, for ASG */
	int             m_sceneX;
	int             m_sceneY;
	bool            m_placed;
};

#endif // NETLISTVIZ_SCHEMATIC_SCHEMATICDEVICE_H
