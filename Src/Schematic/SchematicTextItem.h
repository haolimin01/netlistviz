#ifndef NETLISTVIZ_SCHEMATIC_SCHEMATICTEXTITEM_H
#define NETLISTVIZ_SCHEMATIC_SCHEMATICTEXTITEM_H

#include <QGraphicsTextItem>

QT_BEGIN_NAMESPACE
class QGraphicsSceneMouseEvent;
QT_END_NAMESPACE


class SchematicTextItem : public QGraphicsTextItem
{
    Q_OBJECT

public:
    enum { Type = UserType + 3 };

    explicit SchematicTextItem(QGraphicsItem *parent = nullptr);

    int type() const override { return Type; }

signals:
    void LostFocus(SchematicTextItem *item);
    void SelectedChange(QGraphicsItem *item);

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
    void focusOutEvent(QFocusEvent *event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
};


#endif // NETLISTVIZ_SCHEMATIC_SCHEMATICTEXTITEM_H
