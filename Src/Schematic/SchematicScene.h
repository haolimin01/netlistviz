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
    // int  RenderSchematicDevices(const SDeviceList &devices, int colCount, int rowCount, bool ignoreGroundCap);
    int  RenderSchematicDevices(const SDeviceList &devices, int colCount, int rowCount, IgnoreCap ignore);
    int  RenderSchematicWiresInChannel(const SWireList &wires);
    int  RenderSchematicWiresInLevel(const SWireList &wires);
    int  RenderSchematicDots(const SDotList &dots);
    /* --------------------------------------- */

    void    SetShowTerminal(bool show);
    void    HideGroundCaps(bool hide);
    void    HideCoupledCaps(bool hide);
    void    HideGnds(bool hide);
    void    SetShowSmallGnd(bool showSmall);
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
    void  InitVariables();
    bool  IsItemChange(int type) const;
    void  SenseDeviceTerminal(const QPointF &scenePos) const;
    void  ProcessMousePress(const QPointF &scenePos);
    void  DrawWireTowardDeviceTerminal(const QPointF &scenePos);
    void  FinishDrawingWireAt(const QPointF &scenePos);

    /* --------------- For ASG --------------- */
    void     ChangeDeviceScale(int colCount, int rowCount);
    qreal    CalDeviceScale(int colCount, int rowCount);
    int      CalStartRow(int rowCount) const;
    int      CalStartCol(int colCount) const;
    void     UpdateDeviceScale(qreal newScale);
    void     UpdateWireScale(qreal newScale);
    void     RenderFixedGnds(const SDeviceList &devices);
    void     RenderGroundCaps(const SDeviceList &gcaps);
    void     RenderCoupledCaps(const SDeviceList &ccaps);
    void     SetDeviceAt(int col, int row, SchematicDevice *device);   // col, row (grid)
    void     SetDeviceAt(const QPointF &pos, SchematicDevice *device); // coordinate (scanePos)
    void     AddWiresToScene(const SWireList &wires);
    QPointF  SeekDotScenePos(SchematicDot *dot) const;
    QVector<QPointF> CreateWirePathPoints(SchematicWire *wire) const;
    void     UpdateDots();
    /*---------------------------------------- */

    SchematicDevice*   InsertSchematicDevice(DeviceType, const QPointF &);
    SchematicTextItem* InsertSchematicTextItem(const QPointF &);
    SchematicWire*     InsertSchematicWire(SchematicDevice *, SchematicDevice *,
                        SchematicTerminal *, SchematicTerminal *, const QVector<QPointF> &);


    QMenu                      *m_deviceMenu;
    Mode                        m_mode;
    QFont                       m_font;
    DeviceType                  m_deviceType;

    /* For SchematicWire */
    SchematicDevice            *m_startDevice;
    SchematicTerminal          *m_startTerminal;
    SchematicDevice            *m_endDevice;
    SchematicTerminal          *m_endTerminal;
    QPointF                     m_startPoint;
    QVector<QPointF>            m_currWirePathPoints;
    QGraphicsLineItem          *m_line;

    QColor                      m_textColor;
    QColor                      m_deviceColor;

    bool                        m_showTerminal;
    bool                        m_showBackground;
    bool                        m_showSmallGnd;

    qreal                       m_itemScale;
    qreal                       m_gridW;
    qreal                       m_gridH;
    qreal                       m_margin;

    SDeviceList                 m_gCapDeviceList;
    SDeviceList                 m_cCapDeviceList;
    SDeviceList                 m_gndList;
    SWireList                   m_hasGCapWireList;
    SWireList                   m_hasCCapWireList;
    SWireList                   m_hasGndWireList;

    SDotList                    m_dotList;
};

#endif // NETLISTVIZ_SCHEMATIC_SCHEMATICSCENE_H
