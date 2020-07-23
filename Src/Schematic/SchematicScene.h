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
class QTextStream;
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
        InsertWireMode,
        InsertDeviceMode, // line
        InsertTextMode
    };

    explicit SchematicScene(QMenu *itemMenu, QObject *parent = nullptr);
    ~SchematicScene();

    void SetTextColor(const QColor &color);
    // void SetNodeColor(const QColor &color);
    void SetFont(const QFont &font);
    void SetDeviceType(SchematicDevice::DeviceType type);

    void RenderSchematicData(SchematicData *data);

    /* Write Schematic items to stream */
    void WriteSchematicToStream(QTextStream &stream) const;

    /* Load Schematic from stream to scene */
    void LoadSchematicFromStream(QTextStream &stream);

    void SetShowNodeFlag(bool show);

public slots:
    void SetMode(Mode mode)  { m_mode = mode; }
    void EditorLostFocus(SchematicTextItem *item);

signals:
    void DeviceInserted(SchematicDevice *item);
    void TextInserted(QGraphicsTextItem *item);

protected:
    /* override functions */
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent) override;

    void InsertSchematicDevice(SchematicDevice::DeviceType, const QPointF &);
    void InsertSchematicTextItem(const QPointF &);

private:
    void InitVariables();
    bool IsItemChange(int type) const;
    void SenseDeviceTerminal(const QPointF &) const;

    QMenu             *m_itemMenu;
    Mode               m_mode;
    QFont              m_font;

    SchematicTextItem *m_text;
    SchematicDevice   *m_device;

    SchematicDevice::DeviceType m_deviceType;

    QColor             m_textColor;
    QColor             m_deviceColor;

    SchematicLayout   *m_schLayout;

    /* Device number, assign to added device */
    /* Start from 0 */
    int                m_deviceNumber;

    bool               m_showNodeFlag;
};

#endif // NETLISTVIZ_SCHEMATIC_SCHEMATICSCENE_H
