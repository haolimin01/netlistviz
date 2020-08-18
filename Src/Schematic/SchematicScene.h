#ifndef NETLISTVIZ_SCHEMATIC_SCHEMATICSCENE_H
#define NETLISTVIZ_SCHEMATIC_SCHEMATICSCENE_H

#include "SchematicTextItem.h"
#include "SchematicDevice.h"
#include "SchematicWire.h"
#include "ASG/ASG.h"

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


class SchematicScene : public QGraphicsScene
{
    Q_OBJECT

public:
    enum Mode
    {
        BaseMode,
        InsertWireMode,
        InsertDeviceMode,
        InsertTextMode
    };

    explicit SchematicScene(QMenu *itemMenu, QObject *parent = nullptr);
    ~SchematicScene();

    void SetTextColor(const QColor &color);
    void SetFont(const QFont &font);
    void SetDeviceType(SchematicDevice::DeviceType type);

    // void RenderSchematic(const QVector<QVector<SchematicDevice*>> &levels,
    //                      const QVector<WireDescriptor*> &wireDesps);
    void RenderSchematic(const QVector<DevLevelDescriptor*> &devices,
                         const QVector<WireDescriptor*> &wireDesps);
    /* Write Schematic items to stream */
    void WriteSchematicToStream(QTextStream &stream) const;
    /* Load Schematic from stream to scene */
    void LoadSchematicFromStream(QTextStream &stream);

    void SetShowNodeFlag(bool show);

public slots:
    void SetMode(Mode mode)  { m_mode = mode; }
    void EditorLostFocus(SchematicTextItem *item);
    void SetEnableBackground(bool enabled) { m_backgroundFlag = enabled; }

signals:
    void DeviceInserted(SchematicDevice *item);
    void TextInserted(QGraphicsTextItem *item);

protected:
    /* override functions */
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent) override;
    void drawBackground(QPainter *painter, const QRectF &rect) override;

private:
    void InitVariables();
    bool IsItemChange(int type) const;
    void SenseDeviceTerminal(const QPointF &) const;
    void ProcessMousePress(const QPointF &scenePos);
    void DrawWireTowardDeviceTerminal(const QPointF &scenePos);
    void FinishDrawingWireAt(const QPointF &scenePos);

    /* Insert Item */
    SchematicDevice*   InsertSchematicDevice(SchematicDevice::DeviceType, const QPointF &);
    SchematicTextItem* InsertSchematicTextItem(const QPointF &);
    SchematicWire*     InsertSchematicWire(SchematicDevice *, SchematicDevice *, TerminalType, TerminalType,
                            const QVector<QPointF>&, bool branch=false);
    SchematicWire*     InsertSchematicWire(const WireDescriptor *desp);

    void DecideDeviceOrientation(int x, int y, SchematicDevice *device);
    void RenderGND(int x, int y, SchematicDevice *device);

    /* For ASG */
    void SetDeviceAt(int x, int y, SchematicDevice *device);
    void SetDeviceAt(const QPointF &pos, SchematicDevice *device);


    QMenu                      *m_itemMenu;
    Mode                        m_mode;
    QFont                       m_font;
    SchematicTextItem          *m_text;
    SchematicDevice            *m_device;
    SchematicDevice::DeviceType m_deviceType;
    QGraphicsLineItem          *m_line;

    /* For wire */
    SchematicDevice            *m_startDevice;
    TerminalType                m_startTerminal;
    QPointF                     m_startPoint;
    SchematicDevice            *m_endDevice;
    TerminalType                m_endTerminal;
    QPointF                     m_endPoint;
    QVector<QPointF>            m_curWirePathPoints;


    QColor                      m_textColor;
    QColor                      m_deviceColor;

    /* Device number, assign to added device */
    /* Start from 0 */
    int                         m_deviceNumber;

    bool                        m_showNodeFlag;

    bool                        m_backgroundFlag;
};

#endif // NETLISTVIZ_SCHEMATIC_SCHEMATICSCENE_H
