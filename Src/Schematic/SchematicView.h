#ifndef NETLISTVIZ_SCHEMATIC_SCHEMATICVIEW_H
#define NETLISTVIZ_SCHEMATIC_SCHEMATICVIEW_H

/*
 * @filename : SchematicView.h
 * @date     : 2020.09.17
 * @author   : Hao Limin
 * @email    : haolimin01@sjtu.edu.cn
 * @desp     : inherit QGraphicsView to reimplement wheelEvent.
 */

#include "Define/Define.h"
#include <QGraphicsView>

class SchematicView : public QGraphicsView
{
public:
    explicit SchematicView(QGraphicsScene *parent);

    void EnableScaleByWheel(bool enable) {m_scaleByWheel = enable; }

protected:
    void wheelEvent(QWheelEvent *event) override;

private:
    DISALLOW_COPY_AND_ASSIGN(SchematicView);

    bool    m_scaleByWheel;
};

#endif // NETLISTVIZ_SCHEMATIC_SCHEMATICVIEW_H