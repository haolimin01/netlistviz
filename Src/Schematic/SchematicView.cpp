#include "SchematicView.h"
#include <QDebug>
#include <QWheelEvent>

SchematicView::SchematicView(QGraphicsScene *parent)
    : QGraphicsView(parent)
{
    m_scaleByWheel = false;
}

void SchematicView::wheelEvent(QWheelEvent *event)
{
    if (NOT m_scaleByWheel)
        return QGraphicsView::wheelEvent(event);

#ifdef TRACEx
    qInfo() << LINE_INFO << endl;
#endif

    QMatrix currMatrix = matrix();
    qreal currHScale = currMatrix.m11();
    qreal currVScale = currMatrix.m22();

    qreal dScale = 0.05;
    qreal newHScale, newVScale;

    if (event->delta() > 0) { // zoom in
        newHScale = currHScale + dScale;
        newVScale = currVScale + dScale;
    } else { // zoom out
        newHScale = (currHScale - dScale) > 0? currHScale - dScale : currHScale;
        newVScale = (currVScale - dScale) > 0? currVScale - dScale : currVScale;
    }

    resetMatrix();
    translate(currMatrix.dx(), currMatrix.dy());
    scale(newHScale, newVScale);
}
