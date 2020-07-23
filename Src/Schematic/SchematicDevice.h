#ifndef NETLISTVIZ_SCHEMATIC_SCHEMATICDEVICE_H
#define NETLISTVIZ_SCHEMATIC_SCHEMATICDEVICE_H

#include <QGraphicsPathItem>

QT_BEGIN_NAMESPACE
class QGraphicsPixmapItem;
QT_END_NAMESPACE;

class SchematicNode;


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

	QPixmap GetImage();

	QRectF boundingRect() const override;

	SchematicNode* GetPosNode() const  { return m_posNode; }
	SchematicNode* GetNegNode() const  { return m_negNode; }

	void SetPosNode(SchematicNode *posNode)  { m_posNode = posNode; }
	void SetNegNode(SchematicNode *negNode)  { m_negNode = negNode; }	

	int GetPosNodeId() const;
	int GetNegNodeId() const;

	DeviceType GetDeviceType() const { return m_deviceType; }

	int  type() const override { return Type; }

	QColor GetColor() const  { return m_color; }

	void SetName(QString name) { m_name = name; }
	void SetValue(double value) { m_value = value; }
    QString GetName() const { return m_name; }
	double GetValue() const { return m_value; }
	
	void SetContextMenu(QMenu *contextMenu) { m_contextMenu = contextMenu; }

	void Print() const;

protected:
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
			QWidget *widget = nullptr) override;
	void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;

private:
	void InitVariables();
	QRectF DashRect() const;

	void DrawResistor();
	void DrawCapacitor();
	void DrawInductor();
	void DrawIsrc();
	void DrawVsrc();

	SchematicNode  *m_posNode;
	SchematicNode  *m_negNode;

	DeviceType      m_deviceType;
	QMenu          *m_contextMenu;

	QColor			m_color;      // device color
	Orientation     m_devOrien;   // device orientation
	int             m_terNumber;  // terminal number
	QPixmap        *m_imag;       // device image

	/* For CktParser */
	QString         m_name;       // device value
	double          m_value;      // device name
};

#endif // NETLISTVIZ_SCHEMATIC_SCHEMATICDEVICE_H
