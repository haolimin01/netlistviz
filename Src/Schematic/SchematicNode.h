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

	~SchematicNode();

	/* For QToolButton in MainWindow's ToolBox */
	QPixmap GetImage() const;
	void    SetNodeSize(qreal nodeSize) { m_nodeSize = nodeSize; }

	int     type() const override { return Type; }

	void    AddDevice(SchematicDevice *device);
	void    RemoveDevice(SchematicDevice *device);
	void    RemoveDevices();

private:
	void DrawNode();
	void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;

	QMenu   *m_contextMenu;
	qreal    m_nodeSize;

	QVector<SchematicDevice*> m_devices;
};

#endif // NETLISTVIZ_SCHEMATIC_SCHEMATICNODE_H