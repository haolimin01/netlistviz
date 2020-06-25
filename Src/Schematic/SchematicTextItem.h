#ifndef NETLISTVIZ_SCHEMATIC_SCHEMATICTEXTITEM_H
#define NETLISTVIZ_SCHEMATIC_SCHEMATICTEXTITEM_H

#include <QGraphicsTextItem>
#include <QFont>

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

    QString GetText() const { return toPlainText();}
    QString GetColorName() const { return defaultTextColor().name(); }
    QString GetFontFamily() const { return font().family(); }
    int     GetSize() const { return font().pointSize(); }
    bool    IsBold() const { return font().bold(); }
    bool    IsItalic() const { return font().italic(); }
    bool    IsUnderline() const { return font().underline(); }

signals:
    void LostFocus(SchematicTextItem *item);
    void SelectedChange(QGraphicsItem *item);

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
    void focusOutEvent(QFocusEvent *event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
};


#endif // NETLISTVIZ_SCHEMATIC_SCHEMATICTEXTITEM_H
