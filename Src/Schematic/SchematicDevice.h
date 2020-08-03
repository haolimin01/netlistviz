#ifndef NETLISTVIZ_SCHEMATIC_SCHEMATICDEVICE_H
#define NETLISTVIZ_SCHEMATIC_SCHEMATICDEVICE_H

#include <QGraphicsPathItem>

QT_BEGIN_NAMESPACE
class QGraphicsPixmapItem;
class QGraphicsSceneMouseEvent;
QT_END_NAMESPACE;

class CktNode;
class SchematicWire;


class SchematicDevice : public QGraphicsPathItem
{
public:
	enum { Type = UserType + 4 };
	enum DeviceType { Resistor=0, Capacitor, Inductor,
					 Vsrc/*3*/, Isrc };
	enum Orientation { Horizontal, Vertical };
	enum NodeType    { Positive, Negative };

public:
	SchematicDevice(DeviceType type, QMenu *contextMenu,
				QTransform itemTransform = QTransform(), QGraphicsItem *parent = nullptr);

	~SchematicDevice();

	QPixmap    Image();
	QRectF     boundingRect() const override;

	CktNode*   Node(NodeType type) const;
	void       AddNode(NodeType type, CktNode *node);
	int        NodeId(NodeType type) const;
	void       SetId(int id) { m_id = id; }
	int        Id()  const { return m_id; }
	NodeType   GetNodeType(CktNode *node) const;

	DeviceType GetDeviceType() const { return m_deviceType; }
	int        type() const override { return Type; }
	QColor     Color() const  { return m_color; }
	void       SetName(QString name) { m_name = name; }
	void       SetValue(double value) { m_value = value; }
    QString    Name() const { return m_name; }
	double     Value() const { return m_value; }
	void       SetContextMenu(QMenu *contextMenu) { m_contextMenu = contextMenu; }
	QVector<QRectF>  TerminalRects() const;
	void       SetShowNodeFlag(bool show = true)  { m_showNodeFlag = show; }
	void       AddWire(SchematicWire *wire, int terIndex);
	void       RemoveWires(bool deletion = true);
	void       RemoveWire(SchematicWire *wire, int terIndex);

	void Print() const;

protected:
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
			QWidget *widget = nullptr) override;
	void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
	QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

private:
	void InitVariables();
	QRectF DashRect() const;
	void UpdateWirePosition();

	void DrawResistor();
	void DrawCapacitor();
	void DrawInductor();
	void DrawIsrc();
	void DrawVsrc();

	QMap<NodeType, CktNode *> m_terminals;
	QVector<QList<SchematicWire*> > m_wiresAtTerminal;

	DeviceType      m_deviceType;
	QMenu          *m_contextMenu;

	QColor			m_color;      // device color
	Orientation     m_devOrien;   // device orientation
	int             m_terNumber;  // terminal number
	QPixmap        *m_imag;       // device image
	bool            m_showNodeFlag;
	int             m_id;         // device id, for creating incidence matrix.

	QVector<QRectF> m_terRects;

	/* For CktParser */
	QString         m_name;       // device value
	double          m_value;      // device name
};

#endif // NETLISTVIZ_SCHEMATIC_SCHEMATICDEVICE_H
