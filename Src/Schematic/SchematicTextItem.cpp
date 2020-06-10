#include "SchematicTextItem.h"
#include "SchematicScene.h"


SchematicTextItem::SchematicTextItem(QGraphicsItem *parent)
    : QGraphicsTextItem(parent)
{
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
}


QVariant SchematicTextItem::itemChange(GraphicsItemChange change,
                                     const QVariant &value)
{
    if (change == QGraphicsItem::ItemSelectedHasChanged)
        emit SelectedChange(this);
    return value;
}


void SchematicTextItem::focusOutEvent(QFocusEvent *event)
{
    setTextInteractionFlags(Qt::NoTextInteraction);
    emit LostFocus(this);
    QGraphicsTextItem::focusOutEvent(event);
}


void SchematicTextItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if (textInteractionFlags() == Qt::NoTextInteraction)
        setTextInteractionFlags(Qt::TextEditorInteraction);
    QGraphicsTextItem::mouseDoubleClickEvent(event);
}

