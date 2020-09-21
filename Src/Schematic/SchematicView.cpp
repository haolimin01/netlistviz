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

    qreal zoomInFactor = 1.1;
    qreal zoomOutFactor = 1/ zoomInFactor;

    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setResizeAnchor(QGraphicsView::NoAnchor);

    // old pos
    QPointF oldPos = mapToScene(event->pos());
    qreal zoomFactor = 1;

    // zooom
    if (event->angleDelta().y() > 0)
        zoomFactor = zoomInFactor;
    else
        zoomFactor = zoomOutFactor;
    
    scale(zoomFactor, zoomFactor);

    // new pos
    QPointF newPos = mapToScene(event->pos());

    QPointF delta = newPos - oldPos;
    translate(delta.x(), delta.y());
}
