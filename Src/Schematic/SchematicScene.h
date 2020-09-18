#ifndef NETLISTVIZ_SCHEMATIC_SCHEMATICSCENE_H
#define NETLISTVIZ_SCHEMATIC_SCHEMATICSCENE_H


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
    void SetDeviceType(DeviceType type);

    /* Schematic IO */
    void WriteSchematicToStream(QTextStream &stream) const;
    void LoadSchematicFromStream(QTextStream &stream);

    /* --------------- For ASG --------------- */
    // total column count (devices + channels), total row count (devices + spaces)
    int  RenderSchematicDevices(const SDeviceList &devices, int colCount, int rowCount);
    /* --------------------------------------- */

    void    SetShowTerminal(bool show);
    QPointF Center() const;


public slots:
    void SetMode(Mode mode)  { m_mode = mode; }
    void EditorLostFocus(SchematicTextItem *item);
    void SetShowBackground(bool show) { m_showBackground = show; }

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

    /* --------------- For ASG --------------- */
    void  ChangeDeviceScale(int colCount, int rowCount);
    qreal CalDeviceScale(int colCount, int rowCount);
    int   CalStartRow(int rowCount) const;
    int   CalStartCol(int colCount) const;
    void  UpdateDeviceScale(qreal newScale);
    void  ChangeDeviceOrientation(const SDeviceList &devices);
    void  RenderFixedGnds(const SDeviceList &devices);
    // void RenderFlexibleGnds(const SDeviceList &devices);
    void  SetDeviceAt(int col, int row, SchematicDevice *device);
    /*---------------------------------------- */

    SchematicDevice*   InsertSchematicDevice(DeviceType, const QPointF &);
    SchematicTextItem* InsertSchematicTextItem(const QPointF &);


    QMenu                      *m_itemMenu;
    Mode                        m_mode;
    QFont                       m_font;
    SchematicTextItem          *m_text;
    SchematicDevice            *m_device;
    DeviceType                  m_deviceType;

    QColor                      m_textColor;
    QColor                      m_deviceColor;

    bool                        m_showTerminal;
    bool                        m_showBackground;

    qreal                       m_itemScale;
    qreal                       m_gridW;
    qreal                       m_gridH;
    qreal                       m_margin;
};

#endif // NETLISTVIZ_SCHEMATIC_SCHEMATICSCENE_H
