#ifndef NETLISTVIZ_SCHEMATIC_SCHEMATICNODE_H
#define NETLISTVIZ_SCHEMATIC_SCHEMATICNODE_H

#include <QGraphicsPixmapItem>
#include <QGraphicsPathItem>
#include <QVector>

QT_BEGIN_NAMESPACE
class QMenu;
class QGraphicsScene;
class QGraphicsItem;
class QTransform;
QT_END_NAMESPACE;

class SchematicDevice;


class SchematicNode : public QGraphicsPathItem
{
public:
	enum { Type = UserType + 5 };

public:
	SchematicNode(QMenu *contextMenu, QTransform itemTransform = QTransform(),
		QGraphicsItem *parent = nullptr);
	SchematicNode();

	~SchematicNode();

	/* For QToolButton in MainWindow's ToolBox */
	QPixmap GetImage() const;
	void    SetSize(qreal nodeSize) { m_nodeSize = nodeSize; }

	int     type() const override { return Type; }

	void    AddDevice(SchematicDevice *device);
	void    RemoveDevice(SchematicDevice *device);
	void    RemoveDevices();

    void    SetContextMenu(QMenu *contextMenu) { m_contextMenu = contextMenu; }
	void    SetColor(const QColor &color)  { m_color = color; }

    /* For Parse Netlist, Read and Write SchematicFile */
	void    SetId(int id)          { m_id = id; }
	void    SetName(QString name)  { m_name = name; }
	void    SetGnd(bool isGnd)     { m_isGnd = isGnd; }
	QString GetName() const        { return m_name; }
    int     GetId() const          { return m_id; }
    bool    IsGnd() const          { return m_isGnd; }
	QString GetColorName() const   { return m_color.name(); }
	qreal   GetSize() const        { return m_nodeSize; }
	QPointF GetPos() const         { return pos(); }

	void    Print() const;

protected:
	void    paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *widget = nullptr) override;

private:
	void    InitVariables();
	// void    DrawNode();
	void    contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;

	QMenu   *m_contextMenu;
	qreal    m_nodeSize;

	QVector<SchematicDevice*> m_devices;

	QColor   m_color;

	/* For Cktparser */
	int     m_id;
	QString m_name;
	bool    m_isGnd;
};

#endif // NETLISTVIZ_SCHEMATIC_SCHEMATICNODE_H
