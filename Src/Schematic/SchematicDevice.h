#ifndef NETLISTVIZ_SCHEMATIC_SCHEMATICDEVICE_H
#define NETLISTVIZ_SCHEMATIC_SCHEMATICDEVICE_H

#include <QGraphicsPathItem>
#include "Define/TypeDefine.h"
#include "Define/Define.h"

QT_BEGIN_NAMESPACE
class QGraphicsPixmapItem;
class QGraphicsSceneMouseEvent;
QT_END_NAMESPACE;

class  SchematicWire;
class  SchematicTerminal;
struct SConnector;

class SchematicDevice : public QGraphicsPathItem
{
public:
    enum { Type = UserType + 4 };

public:
    /* ASG entrance */
    SchematicDevice();

    /* Insert SchematicDevice (in Scene) entrance*/
    SchematicDevice(DeviceType type, QMenu *contextMenu,
                QTransform transform = QTransform(), QGraphicsItem *parent = nullptr);
    
    ~SchematicDevice();

    void         Initialize(); // draw device shape, set annotation and terminals shape
    QPixmap      Image();
    int          type() const override { return Type; }
    QRectF       boundingRect() const override;
    void         SetAnnotationVisible(bool show);
    void         SetDeviceType(DeviceType type) {m_deviceType = type; }
    DeviceType   GetDeviceType() const { return m_deviceType; }
    void         SetContextMenu(QMenu *contextMenu) { m_contextMenu = contextMenu; }
    void         AddTerminal(TerminalType type, SchematicTerminal *terminal);
    void         SetName(const QString &name);
    QString      Name() const { return m_name; }
    void         SetId(int id) { m_id = id; }
    void         SetShowTerminal(bool show) { m_showTerminal = show; }
	void         SetOrientation(Orientation orien);
	Orientation  GetOrientation() const { return m_devOrien; }
    void         SetScale(qreal newScale);
    void         SetReverse(bool reverse);
    bool         GroundCap() const;
    bool         CoupledCap() const;
    bool         TerminalsContain(const QPointF &scenePos) const;
    void         RemoveWires(bool deletion = true);
    void         UpdateWirePosition();
    QPointF      ScenePosByTerminalScenePos(SchematicTerminal *ter, const QPointF &terScenePos) const;
    void         SetScenePos(int col, int row) { m_sceneCol = col; m_sceneRow = row; }
    int          SceneCol() const { return m_sceneCol; }
    int          SceneRow() const { return m_sceneRow; }
    int          GeometricalCol() const { return m_geoCol; }
    int          GeometricalRow() const { return m_geoRow; }
    void         SetGeometricalPos(int col, int row);
    void         SetAsSmallGnd(bool smallGnd);
    void         AddConnector(SConnector *desp);

    SchematicTerminal* GetTerminal(TerminalType type) const;
    SchematicTerminal* ConnectTerminal() const;
    SchematicTerminal* ConnectTerminal(TerminalType type) const;
    STerminalTable     GetTerminalTable() const { return m_terminals; }

    /* Print and Plot */
    void         Print() const;

protected:
    void         paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                  QWidget *widget = nullptr) override;
    void         contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
    QPainterPath shape() const override;
    void         mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    QVariant     itemChange(GraphicsItemChange change, const QVariant &value) override;

private:
    DISALLOW_COPY_AND_ASSIGN(SchematicDevice);

    /* For Insert SchematicDevice entrance */
    void         CreateTerminalsBySelf();
    QRectF       DashRect() const;
    void         SetTerminalRect(TerminalType type, const QRectF &rect);

    /* For annotation text */
    void         CreateAnnotation(const QString &text);
    void         SetAnnotRelPos();
    QRectF       GndTerminalRect(bool smallGnd = false) const; // gnd or smallGnd rect
    void         ClearConnectors();

    void         DrawResistor();
    void         DrawCapacitor();
    void         DrawInductor();
    void         DrawIsrc();
    void         DrawVsrc();
    void         DrawGnd();
    void         DrawSmallGnd();

    QMenu             *m_contextMenu;
    QColor             m_color;        // device color
    Orientation        m_devOrien;     // device orientation
    QPixmap           *m_imag;
    QGraphicsTextItem *m_annotText;    // annotation text
    QPointF            m_annotRelPos;  // annoatiton text relative position
    bool               m_isDevice;     // (gnd is not device)
    STerminalTable     m_terminals;    // type : SchematicTerminal Ptr
    bool               m_showTerminal; // draw terminal rect on scene
    bool               m_smallGnd;     // whether to be small gnd

    /* Copy from Circuit Device */
    int                m_id;
    DeviceType         m_deviceType;
    QString            m_name;
    bool               m_reverse;
    /* Geometrical Position */
    int                m_geoCol; // geometrical column
    int                m_geoRow; // geometrical row

    /* Scene Position, in order to place in center */
    int                m_sceneCol;
    int                m_sceneRow;

    SConnectorList     m_connectors; // connect devices

    /* if device is ground or coupled cap */
    // STerminalTable     m_capConnectTerminalTable;

    /* if device is gnd */
    // SchematicTerminal *m_gndConnectTerminal;
};

#endif // NETLISTVIZ_SCHEMATIC_SCHEMATICDEVICE_H
