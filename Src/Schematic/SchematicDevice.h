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

class SchematicDevice : public QGraphicsPathItem
{
public:
    enum { Type = UserType + 4 };

public:
    SchematicDevice(Device *dev, QMenu *contextMenu,
                QTransform transform = QTransform(), QGraphicsItem *parent = nullptr);

    SchematicDevice(DeviceType type, QMenu *contextMenu,
                QTransform transform = QTransform(), QGraphicsItem *parent = nullptr);
    
    ~SchematicDevice();

    QPixmap      Image();
    QRectF       boundingRect() const override;
    DeviceType   GetDeviceType() const { return m_deviceType; }
    void         SetShowNodeFlag(bool show) { m_showNodeFlag = show; }

protected:
    void         paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                  QWidget *widget = nullptr) override;
    void         contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
    QPainterPath shape() const override;
    void         mousePressEvent(QGraphicsSceneMouseEvent *event) override;

private:
    DISALLOW_COPY_AND_ASSIGN(SchematicDevice);

    void         Initialize();
    QRectF       DashRect() const;

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
    QColor             m_color;     // device color
    Orientation        m_devOrien;  // device orientation
    QPixmap           *m_imag;
    bool               m_showNodeFlag;
    QGraphicsTextItem *m_annotText;   // annotation text
    QPointF            m_annotRelPos; // annoatiton text relative position
    Device            *m_cktdev;
    DeviceType         m_deviceType;
    bool               m_isDevice;    // (gnd is not device)
    TerminalRectTable  m_terRects; // terminal type and it's rect
};

#endif // NETLISTVIZ_SCHEMATIC_SCHEMATICDEVICE_H