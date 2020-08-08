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
    NodeType         startTerminal;
    NodeType         endTerminal;
    QVector<QPointF> pathPoints;
    QPointF          pos;       // wire position in the scene
};


class SchematicWire : public QGraphicsItem
{
public:
    enum { Type = UserType + 6 };

public:
    SchematicWire(SchematicDevice *startDev, SchematicDevice *endDev, NodeType startTer, NodeType endTer,
                  QGraphicsItem *parent = nullptr, QGraphicsScene *scene = nullptr);
    ~SchematicWire();

    int type() const  { return Type; }
    QRectF boundingRect() const override;

    void SetWirePathPoints(QVector<QPointF> points);
    // void CompactPathPoints();
    void UpdatePosition(SchematicDevice *device, NodeType terminal, const QPointF &newPos);

    SchematicDevice* StartDevice() const  { return m_startDev; }
    SchematicDevice* EndDevice()   const  { return m_endDev; }
    NodeType         StartTerminal() const { return m_startTerminal; }
    NodeType         EndTerminal()   const { return m_endTerminal; }

protected:
    void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *widget = nullptr) override;

private:
    QColor           m_color;
    SchematicDevice *m_startDev;
    SchematicDevice *m_endDev;
    NodeType         m_startTerminal;
    NodeType         m_endTerminal;
    QVector<QPointF> m_wirePathPoints;
};

#endif // NETLISTVIZ_SCHEMATIC_SCHEMATICWIRE_H