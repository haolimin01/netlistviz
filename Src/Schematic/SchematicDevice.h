#ifndef NETLISTVIZ_SCHEMATIC_SCHEMATICDEVICE_H
#define NETLISTVIZ_SCHEMATIC_SCHEMATICDEVICE_H

#include <QGraphicsPathItem>

QT_BEGIN_NAMESPACE
class QGraphicsPixmapItem;
QT_END_NAMESPACE;

class CktNode;


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

	CktNode* GetNode(int index) const;
	void     AddNode(CktNode *node);
	int      GetNodeId(int index) const;

	DeviceType GetDeviceType() const { return m_deviceType; }

	int  type() const override { return Type; }

	QColor GetColor() const  { return m_color; }

	void SetName(QString name) { m_name = name; }
	void SetValue(double value) { m_value = value; }
    QString GetName() const { return m_name; }
	double GetValue() const { return m_value; }
	
	void SetContextMenu(QMenu *contextMenu) { m_contextMenu = contextMenu; }

	QVector<QRectF>  GetTerminalRects() const;

	void SetShowNodeFlag(bool show = true)  { m_showNodeFlag = show; }

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

	QVector<CktNode *> m_terminals;

	DeviceType      m_deviceType;
	QMenu          *m_contextMenu;

	QColor			m_color;      // device color
	Orientation     m_devOrien;   // device orientation
	int             m_terNumber;  // terminal number
	QPixmap        *m_imag;       // device image
	bool            m_showNodeFlag;

	QVector<QRectF> m_terRects;

	/* For CktParser */
	QString         m_name;       // device value
	double          m_value;      // device name
};

#endif // NETLISTVIZ_SCHEMATIC_SCHEMATICDEVICE_H
