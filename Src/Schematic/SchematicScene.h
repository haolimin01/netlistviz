#ifndef NETLISTVIZ_SCHEMATIC_SCHEMATICSCENE_H
#define NETLISTVIZ_SCHEMATIC_SCHEMATICSCENE_H

#include "SchematicNode.h"
#include "SchematicTextItem.h"
#include "SchematicDevice.h"

#include <QGraphicsScene>

QT_BEGIN_NAMESPACE
class QGraphicsSceneMouseEvent;
class QMenu;
class QPointF;
class QGraphicsLineItem;
class QFont;
class QGraphicsTextItem;
class QColor;
QT_END_NAMESPACE
class SchematicData;
class SchematicLayout;


class SchematicScene : public QGraphicsScene
{
    Q_OBJECT

public:
    enum Mode
    {
        BaseMode,
        InsertNodeMode,
        InsertDeviceMode, // line
        InsertTextMode
    };

    explicit SchematicScene(QMenu *itemMenu, QObject *parent = nullptr);
    ~SchematicScene();

    void SetTextColor(const QColor &color);
    void SetNodeColor(const QColor &color);
    void SetFont(const QFont &font);
    void SetDeviceType(SchematicDevice::DeviceType type);

    void RenderSchematicData(SchematicData *data);

    /* Add and Remove Item */
    // void AddItem(QGraphicsItem *);
    // void RemoveItem(QGraphicsItem *);

public slots:
    void SetMode(Mode mode);
    void EditorLostFocus(SchematicTextItem *item);

signals:
    void NodeInserted(SchematicNode *item);
    void DeviceInserted(SchematicDevice *item);
    void TextInserted(QGraphicsTextItem *item);

protected:
    /* override functions */
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent) override;

    void InsertSchematicDevice(SchematicDevice::DeviceType, SchematicNode *, SchematicNode *);
    void InsertSchematicNode(const QPointF &);
    void InsertSchematicTextItem(const QPointF &);

private:
    void InitVariables();
    bool IsItemChange(int type) const;

    QMenu             *m_itemMenu;
    Mode               m_mode;
    QPointF            m_startPoint;
    QFont              m_font;

    SchematicTextItem *m_text;
    SchematicNode     *m_node;
    SchematicDevice   *m_device;
    QGraphicsLineItem *m_line;

    SchematicDevice::DeviceType m_deviceType;

    QColor             m_textColor;
    QColor             m_nodeColor;

    SchematicLayout   *m_schLayout;
};

#endif // NETLISTVIZ_SCHEMATIC_SCHEMATICSCENE_H
