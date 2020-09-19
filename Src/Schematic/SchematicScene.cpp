#include "SchematicScene.h"

#include <QGraphicsSceneMouseEvent>
#include <QTextCursor>
#include <QApplication>
#include <QMap>
#include <QDebug>
#include <QGraphicsView>


SchematicScene::SchematicScene(QMenu *itemMenu, QObject *parent)
    : QGraphicsScene(parent)
{
    m_itemMenu = itemMenu;

    InitVariables();
}

SchematicScene::~SchematicScene()
{
}

/* Do NOT handle m_data */
void SchematicScene::InitVariables()
{
    m_mode = BaseMode;
    m_text = nullptr;
    m_device = nullptr;
    m_textColor = Qt::black;
    m_deviceType = RESISTOR;
    m_deviceColor = Qt::black;
    m_showTerminal = false;
    m_showBackground = true;

    m_itemScale = 1;
    m_gridW = DFT_Grid_W;
    m_gridH = DFT_Grid_H;
    m_margin = Scene_Margin;
}

void SchematicScene::SetTextColor(const QColor &color)
{
    m_textColor = color;
    if (IsItemChange(SchematicTextItem::Type)) {
        SchematicTextItem *item = qgraphicsitem_cast<SchematicTextItem *>
                                  (selectedItems().first());
        item->setDefaultTextColor(m_textColor);
    }
}

void SchematicScene::SetFont(const QFont &font)
{
    m_font = font;

    if (IsItemChange(SchematicTextItem::Type)) {
        QGraphicsTextItem *item = qgraphicsitem_cast<SchematicTextItem *>
                                  (selectedItems().first());
        // At this point the selection can change so the first
        // selected item might not be a SchematicTextItem
        if (item)
            item->setFont(m_font);
    }
}

void SchematicScene::SetDeviceType(DeviceType type)
{
    m_deviceType = type;
}

void SchematicScene::SetShowTerminal(bool show)
{
    m_showTerminal = show;

    SchematicDevice *device = nullptr;
    foreach (QGraphicsItem *item, items()) {
        if (item->type() == SchematicDevice::Type) {
            device = qgraphicsitem_cast<SchematicDevice*> (item);
            device->SetShowTerminal(show);
            device->update(); 
        }
    }
}

// BUG
void SchematicScene::drawBackground(QPainter *painter, const QRectF &rect)
{
#if 0
    Q_UNUSED(rect);
    if (NOT m_showBackground)  return;

    qreal sceneWidth = width();
    qreal sceneHeight = height();
    QPointF startPos = QPointF(0, 0);
    painter->setPen(QPen(Qt::gray, 1, Qt::DashLine));

    qreal curX = startPos.x();
    qreal curY = startPos.y();
    qreal endX = sceneWidth;
    qreal endY = sceneHeight;

    /* horizontal line */
    while (curY < endY) {
        painter->drawLine(startPos.x(), curY, endX, curY);
        curY += Grid_H;
    }

    /* vertical line */
    while (curX < endX) {
        painter->drawLine(curX, startPos.y(), curX, endY);
        curX += Grid_W;
    }
#endif
}

void SchematicScene::EditorLostFocus(SchematicTextItem *item)
{
    QTextCursor cursor = item->textCursor();
    cursor.clearSelection();
    item->setTextCursor(cursor);

    if (item->toPlainText().isEmpty()) {
        removeItem(item);
        item->deleteLater();
    }
}

void SchematicScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (mouseEvent->button() != Qt::LeftButton)
        return;
    
    switch (m_mode) {
        case BaseMode:
            // ProcessMousePress(mouseEvent->scenePos());
            break;

        case InsertDeviceMode:
            InsertSchematicDevice(m_deviceType, mouseEvent->scenePos());
            break;

        case InsertTextMode:
            InsertSchematicTextItem(mouseEvent->scenePos());
            break;
        
        case InsertWireMode:
            // FinishDrawingWireAt(mouseEvent->scenePos());
            break;

        default:;
    }

    QGraphicsScene::mousePressEvent(mouseEvent);
}

void SchematicScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    QPointF scenePos = mouseEvent->scenePos();

    switch (m_mode) {
        case BaseMode:
            SenseDeviceTerminal(scenePos);
            break;
        case InsertWireMode:
            break;
        case InsertDeviceMode:
            break;
        case InsertTextMode:
            break;
        default:;
    }

    QGraphicsScene::mouseMoveEvent(mouseEvent);
}

void SchematicScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    /* do something */
    QGraphicsScene::mouseReleaseEvent(mouseEvent);
}

SchematicDevice* SchematicScene::InsertSchematicDevice(DeviceType type,
                                 const QPointF &pos)
{
    SchematicDevice *dev = new SchematicDevice(type, m_itemMenu);
    dev->setPos(pos);
    dev->SetShowTerminal(m_showTerminal);
    dev->setScale(m_itemScale);
    addItem(dev);
    emit DeviceInserted(dev);

    return dev;
}

SchematicTextItem* SchematicScene::InsertSchematicTextItem(const QPointF &pos)
{
    m_text = new SchematicTextItem();
    m_text->setFont(m_font);
    m_text->setTextInteractionFlags(Qt::TextEditorInteraction);
    m_text->setScale(m_itemScale);
    m_text->setZValue(0);
    connect(m_text, &SchematicTextItem::LostFocus,
            this, &SchematicScene::EditorLostFocus);
    addItem(m_text);
    m_text->setDefaultTextColor(m_textColor);
    m_text->setPos(pos);
    emit TextInserted(m_text);

    return m_text;
}

bool SchematicScene::IsItemChange(int type) const
{
    const QList<QGraphicsItem *> items = selectedItems();
    /* lambda */
    const auto cb = [type](const QGraphicsItem *item) { return item->type() == type; };
    return std::find_if(items.begin(), items.end(), cb) != items.end();
}

QPointF SchematicScene::Center() const
{
    qreal xPosSum = 0, yPosSum = 0;
    qreal deviceCount = 0;
    foreach (QGraphicsItem *item, items()) {
        if (item->type() == SchematicDevice::Type) {
            deviceCount++;
            xPosSum += item->x();
            yPosSum += item->y();
        }
    }

    if (deviceCount == 0)
        return QPointF(0, 0);
    else
        return QPointF(xPosSum / deviceCount, yPosSum / deviceCount);
}

void SchematicScene::SenseDeviceTerminal(const QPointF &scenePos) const
{
    QGraphicsItem *item;
    SchematicDevice *device;

    item = itemAt(scenePos, QTransform());
    if (item) {
        if (item->type() == SchematicDevice::Type) {
            device = qgraphicsitem_cast<SchematicDevice *>(item);
            if (NOT device)  return;
            if (device->TerminalsContain(scenePos)) {
                views().first()->setCursor(Qt::CrossCursor);
                return;
            }
        }
    }

    views().first()->setCursor(Qt::ArrowCursor);
}
