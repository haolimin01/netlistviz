#ifndef NETLISTVIZ_SCHEMATIC_SCHEMATICSCENE_H
#define NETLISTVIZ_SCHEMATIC_SCHEMATICSCENE_H

#include "SchematicTextItem.h"
#include "SchematicDevice.h"
#include "SchematicWire.h"
#include "ASG/ASG.h"

#include <QGraphicsScene>
#include <QMultiMap>
#include <QPair>

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


/* For supporting comparision */
class MyPointF : public QPointF
{
public:
    MyPointF(qreal xpos, qreal ypos) : QPointF(xpos, ypos) {}
    MyPointF(const QPointF &p) : QPointF(p.x(), p.y()) {}

    bool operator < (const MyPointF &pf) const
    {
        if (x() < pf.x())
            return true;
        else if (x() == pf.x())
            return y() < pf.y();
        else
            return false;
    }
};

const static int Grid_W = 50;
const static int Grid_H = 50;

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

    /* render schematic from ASG result, implemented in RenderSchematic.cpp */
    // void RenderSchematic(const QVector<DevLevelDescriptor*> &devices,
    //                      const QVector<WireDescriptor*> &wireDesps);

    /* these 2 functions are implemented in IOSchematic.cpp */
    /* Write Schematic items to stream */
    void WriteSchematicToStream(QTextStream &stream) const;
    /* Load Schematic from stream to scene */
    void LoadSchematicFromStream(QTextStream &stream);

    void SetShowNodeFlag(bool show);
    void SetShowBranchFlag(bool show);

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
    SchematicDevice*   InsertSchematicDevice(DeviceType, const QPointF &);
    SchematicTextItem* InsertSchematicTextItem(const QPointF &);
    SchematicWire*     InsertSchematicWire(SchematicDevice *, SchematicDevice *, TerminalType, TerminalType,
                            const QVector<QPointF>&, bool branch=false);
    SchematicWire*     InsertSchematicWire(const WireDescriptor *desp);


    /* For ASG */
    /* these functions are implemented in RenderSchematic.cpp */
    void DecideDeviceOrientation(int x, int y, SchematicDevice *device);
    void RenderFlexiableGnd(SchematicDevice *device);
    void RenderFixedGnd(SchematicDevice *device);
    void SetDeviceAt(int x, int y, SchematicDevice *device);
    void SetDeviceAt(const QPointF &pos, SchematicDevice *device);
    void TagDeviceOnBranch();
    bool ContainsWire(const MyPointF &p1, const MyPointF &p2); // startPoint, endPoint
    void DecideSceneRect(int levelCount, int maxDevCount); // change scene rect


    QMenu                      *m_itemMenu;
    Mode                        m_mode;
    QFont                       m_font;
    SchematicTextItem          *m_text;
    SchematicDevice            *m_device;
    DeviceType                  m_deviceType;
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
    bool                        m_showBranchFlag;

    /* for deleting duplicated wires */
    QMultiMap<MyPointF, MyPointF>  m_pointPairs;
};

#endif // NETLISTVIZ_SCHEMATIC_SCHEMATICSCENE_H
