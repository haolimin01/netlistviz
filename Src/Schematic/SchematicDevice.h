#ifndef NETLISTVIZ_SCHEMATIC_SCHEMATICDEVICE_H
#define NETLISTVIZ_SCHEMATIC_SCHEMATICDEVICE_H

#include <QGraphicsPathItem>
#include "Define/TypeDefine.h"
#include "Define/Define.h"

QT_BEGIN_NAMESPACE
class QGraphicsPixmapItem;
class QGraphicsSceneMouseEvent;
QT_END_NAMESPACE;

class Device;
class SchematicWire;
class SchematicTerminal;

class SchematicDevice : public QGraphicsPathItem
{
public:
    enum { Type = UserType + 4 };

public:
    /* ASG entrance */
    SchematicDevice(Device *dev, QMenu *contextMenu = nullptr,
                QTransform transform = QTransform(), QGraphicsItem *parent = nullptr);

    /* Insert SchematicDevice (in Scene) entrance*/
    SchematicDevice(DeviceType type, QMenu *contextMenu,
                QTransform transform = QTransform(), QGraphicsItem *parent = nullptr);
    
    ~SchematicDevice();

    void         Initialize(); // draw device shape, set annotation and terminals shape
    QPixmap      Image();
    int          type() const override { return Type; }
    QRectF       boundingRect() const override;
    DeviceType   GetDeviceType() const { return m_deviceType; }
    void         SetContextMenu(QMenu *contextMenu) { m_contextMenu = contextMenu; }
    void         AddTerminal(TerminalType type, SchematicTerminal *terminal);
    QString      Name() const { return m_name; }
    void         SetShowTerminal(bool show) { m_showTerminal = show; }
    void         SetGeometricalPos(int col, int row);
    int          LogicalCol() const { return m_logCol; }
    int          LogicalRow() const { return m_logRow; }
	void         SetOrientation(Orientation orien);
	Orientation  GetOrientation() const { return m_devOrien; }
    void         SetScale(qreal newScale);
    void         SetReverse(bool reverse);
    bool         GroundCap() const;
    bool         CoupledCap() const;
    bool         TerminalsContain(const QPointF &scenePos) const;

    SchematicTerminal* GetTerminal(TerminalType type) const;
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

    void         DrawResistor();
    void         DrawCapacitor();
    void         DrawInductor();
    void         DrawIsrc();
    void         DrawVsrc();
    void         DrawGnd();

    QMenu             *m_contextMenu;
    QColor             m_color;        // device color
    Orientation        m_devOrien;     // device orientation
    QPixmap           *m_imag;
    QGraphicsTextItem *m_annotText;    // annotation text
    QPointF            m_annotRelPos;  // annoatiton text relative position
    bool               m_isDevice;     // (gnd is not device)
    STerminalTable     m_terminals;    // type : SchematicTerminal Ptr
    bool               m_showTerminal; // draw terminal rect on scene

    /* Copy from Circuit Device (Logical) */
    int                m_id;
    int                m_logCol;   // logical col
    int                m_logRow;   // logical row
    DeviceType         m_deviceType;
    QString            m_name;
    bool               m_reverse;

    /* Geometrical Position */
    int                m_geoCol; // geometrical column
    int                m_geoRow; // geometrical row
};

#endif // NETLISTVIZ_SCHEMATIC_SCHEMATICDEVICE_H
