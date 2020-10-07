#ifndef NETLISTVIZ_SCHEMATIC_SCHEMATIC_DOT_H
#define NETLISTVIZ_SCHEMATIC_SCHEMATIC_DOT_H

/*
 * @filename : SchematicDot.h
 * @author   : Hao Limin
 * @date     : 2020.10.06
 * @email    : haolimin01@sjtu.edu.cn
 * @desp     : Wire Cross Point
 */

#include <QGraphicsPathItem>
#include "Define/Define.h"

class SchematicTerminal;

class SchematicDot : public QGraphicsPathItem
{
public:
    enum { Type = UserType + 8 };

public:
    SchematicDot();
    ~SchematicDot();

    int       type() const override { return Type; }
    QRectF    boundingRect() const override;
    void      SetScale(qreal newScale);

    void      SetTerminal(SchematicTerminal *terminal) { m_terminal = terminal; }
    void      SetTrackCount(int count)   { m_trackCount = count; }
    int       TrackCount() const         { return m_trackCount; }
    void      SetTrack(int track)        { m_track = track; }
    int       Track() const              { return m_track; }
    void      SetGeometricalCol(int col) { m_geoCol = col; }
    int       GeometricalCol() const     { return m_geoCol; }
    void      SetHoldColCount(int count) { m_holdColCount = count; }
    int       HoldColCount() const       { return m_holdColCount; }
    SchematicTerminal* GetSchematicTerminal() const { return m_terminal; }
    void      SetScenePos(int col, int row) { m_sceneCol = col; m_sceneRow = row; }

    void      Print() const;

protected:
    void         paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                   QWidget *widget = nullptr) override;
    void         contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
    QPainterPath shape() const override;
    void         mousePressEvent(QGraphicsSceneMouseEvent *event) override;

private:
    DISALLOW_COPY_AND_ASSIGN(SchematicDot);

    void      DrawDot();
    QRectF    DashRect() const;

    SchematicTerminal *m_terminal;
    int                m_trackCount;
    int                m_track;
    int                m_geoCol;
    qreal              m_dotLen;
    int                m_holdColCount;
    int                m_sceneCol;
    int                m_sceneRow;
};

#endif // NETLISTVIZ_SCHEMATIC_SCHEMATIC_DOT_H
