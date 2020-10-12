#ifndef NETLISTVIZ_SCHEMATIC_SCHEMATICWIRE_H
#define NETLISTVIZ_SCHEMATIC_SCHEMATICWIRE_H

/*
 * @filename : SchematicWire.h
 * @author   : Hao Limin
 * @date     : 2020.07.24
 * @email    : haolimin01@sjtu.edu.cn
 * @desp     : Wire to connect devices and gnds.
 * @modified : Hao Limin, 2020.09.17
 * @modified : Hao Limin, 2020.09.28
 */

#include <QGraphicsPathItem>

#include "Define/Define.h"

QT_BEGIN_NAMESPACE
class QRectF;
class QPointF;
QT_END_NAMESPACE;

class SchematicDevice;
class SchematicTerminal;


class SchematicWire : public QGraphicsItem
{
public:
    enum { Type = UserType + 6 };

public:
    /* For InsertSchematicWire in Scene */
    SchematicWire(SchematicDevice *, SchematicDevice *, SchematicTerminal *, SchematicTerminal *,
        QGraphicsItem *parent = nullptr, QGraphicsScene *scene = nullptr);
    ~SchematicWire();

    SchematicDevice*    StartDevice()   const { return m_startDevice; }
    SchematicDevice*    EndDevice()     const { return m_endDevice; }
    SchematicTerminal*  StartTerminal() const { return m_startTerminal; }
    SchematicTerminal*  EndTerminal()   const { return m_endTerminal; }

    int       type() const override { return Type; }
    QRectF    boundingRect() const override;
    void      SetWirePathPoints(const QVector<QPointF> &points);
    void      SetTrack(int track) { m_track = track; }
    int       Track() const { return m_track; }
    void      SetTrackCount(int n) { m_trackCount = n; }
    int       TrackCount() const { return m_trackCount; }
    void      SetSceneCol(int col) { m_sceneCol = col; }
    void      SetHoldColCount(int count) { m_holdColCount = count; }
    int       HoldColCount() const { return m_holdColCount; }
    int       SceneCol() const { return m_sceneCol; }
    void      UpdatePosition(SchematicTerminal *terminal); // terminal is unique
    void      SetScale(qreal scale) { m_lineWidth = DFT_Wire_W * scale; }
    bool      HasGroundCap() const;
    bool      HasCoupledCap() const;

    void      Print() const;

protected:
    void     paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *widget = nullptr) override;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override; 

private:
    DISALLOW_COPY_AND_ASSIGN(SchematicWire);

    void Initialize();

    QColor             m_color;
    SchematicDevice   *m_startDevice;
    SchematicDevice   *m_endDevice;
    SchematicTerminal *m_startTerminal;
    SchematicTerminal *m_endTerminal;
    QVector<QPointF>   m_wirePathPoints;
    qreal              m_lineWidth;

    /* For ASG */
    int                m_track;        // track number
    int                m_sceneCol;     // real col in scene
    int                m_trackCount;   // this channel track count
    int                m_holdColCount; // hold col count on scene
};

#endif // NETLISTVIZ_SCHEMATIC_SCHEMATICWIRE_H
