#ifndef NETLISTVIZ_SCHEMATIC_SCHEMATICWIRE_H
#define NETLISTVIZ_SCHEMATIC_SCHEMATICWIRE_H

/*
 * @filename : SchematicWire.h
 * @author   : Hao Limin
 * @date     : 2020.07.24
 */

#include <QGraphicsPathItem>

#include "SchematicDevice.h"

QT_BEGIN_NAMESPACE
class QRectF;
class QPointF;
QT_END_NAMESPACE;


struct WireDescriptor
{
    SchematicDevice *startDev;
    SchematicDevice *endDev;
    int              startTer;  // terminal index of the start device
    int              endTer;    // terminal index of the end device
    QVector<QPointF> pathPoints;
    QPointF          pos;       // wire position in the scene
};


class SchematicWire : public QGraphicsItem
{
public:
    enum { Type = UserType + 6 };

public:
    SchematicWire(SchematicDevice *startDev, SchematicDevice *endDev, int startTer, int endTer,
                  QGraphicsItem *parent = nullptr, QGraphicsScene *scene = nullptr);
    ~SchematicWire();

    int type() const  { return Type; }
    QRectF boundingRect() const override;

    void SetWirePathPoints(QVector<QPointF> points);
    void CompactPathPoints();
    void UpdatePosition(SchematicDevice *device, int terIndex, const QPointF &newPos);

    SchematicDevice* GetStartDevice() const  { return m_startDev; }
    SchematicDevice* GetEndDevice()   const  { return m_endDev; }
    int GetStartTerminalIndex() const  { return m_startTerIndex; }
    int GetEndTerminalIndex()   const  { return m_endTerIndex; }

protected:
    void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *widget = nullptr) override;

private:
    QColor           m_color;
    SchematicDevice *m_startDev;
    SchematicDevice *m_endDev;
    int              m_startTerIndex;
    int              m_endTerIndex;
    QVector<QPointF> m_wirePathPoints;
};

#endif // NETLISTVIZ_SCHEMATIC_SCHEMATICWIRE_H