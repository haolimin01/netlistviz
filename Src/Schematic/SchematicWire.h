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
    TerminalType     startTerminal;
    TerminalType     endTerminal;
    QVector<QPointF> pathPoints;
    QPointF          pos;       // wire position in the scene
    bool             isBranch = false;
};


class SchematicWire : public QGraphicsItem
{
public:
    enum { Type = UserType + 6 };

public:
    SchematicWire(SchematicDevice *startDev, SchematicDevice *endDev, TerminalType startTer, TerminalType endTer,
                  QGraphicsItem *parent = nullptr, QGraphicsScene *scene = nullptr);
    ~SchematicWire();

    SchematicDevice* StartDevice() const  { return m_startDev; }
    SchematicDevice* EndDevice()   const  { return m_endDev; }
    TerminalType     StartTerminal() const { return m_startTerminal; }
    TerminalType     EndTerminal()   const { return m_endTerminal; }

    int    type() const  { return Type; }
    QRectF boundingRect() const override;
    void   SetWirePathPoints(QVector<QPointF> points);
    // void CompactPathPoints();
    void   UpdatePosition(SchematicDevice *device, TerminalType terminal, const QPointF &newPos);
    void   SetAsBranch(bool branch);
    bool   IsBranch() const  { return m_isBranch; }
    void   SetShowBranchFlag(bool show) { m_showBranchFlag = show; }


protected:
    void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *widget = nullptr) override;

private:
    QColor           m_color;
    SchematicDevice *m_startDev;
    SchematicDevice *m_endDev;
    TerminalType     m_startTerminal;
    TerminalType     m_endTerminal;
    QVector<QPointF> m_wirePathPoints;

    /* For ASG */
    bool             m_isBranch;
    bool             m_showBranchFlag;
};

#endif // NETLISTVIZ_SCHEMATIC_SCHEMATICWIRE_H