#include "SchematicScene.h"

#include <QGraphicsSceneMouseEvent>
#include <QTextCursor>


SchematicScene::SchematicScene(QMenu *itemMenu, QObject *parent)
    : QGraphicsScene(parent)
{
    m_itemMenu = itemMenu;
    m_mode = BaseMode;
    m_text = nullptr;
    m_node = nullptr;
    m_device = nullptr;
    m_line = nullptr;
    m_textColor = Qt::black;
    m_nodeColor = Qt::black;
    m_deviceType = SchematicDevice::Resistor;
}


void SchematicScene::SetTextColor(const QColor &color)
{
    m_textColor = color;
    if (IsItemChange(SchematicTextItem::Type)) {
        SchematicTextItem *item = qgraphicsitem_cast<SchematicTextItem *>(selectedItems().first());
        item->setDefaultTextColor(m_textColor);
    }
}


void SchematicScene::SetNodeColor(const QColor &color)
{
    m_nodeColor = color;
    if (IsItemChange(SchematicNode::Type)) {
        SchematicNode *item = qgraphicsitem_cast<SchematicNode *>(selectedItems().first());
        item->setBrush(m_nodeColor);
    }
}


void SchematicScene::SetFont(const QFont &font)
{
    m_font = font;

    if (IsItemChange(SchematicTextItem::Type)) {
        QGraphicsTextItem *item = qgraphicsitem_cast<SchematicTextItem *>(selectedItems().first());
        //At this point the selection can change so the first selected item might not be a SchematicTextItem
        if (item)
            item->setFont(m_font);
    }
}


void SchematicScene::SetDeviceType(SchematicDevice::DeviceType type)
{
    m_deviceType = type;
}


void SchematicScene::SetMode(Mode mode)
{
    m_mode = mode;
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
    case InsertDeviceMode:
        m_line = new QGraphicsLineItem(QLineF(mouseEvent->scenePos(),
                                            mouseEvent->scenePos()));
        m_line->setPen(QPen(Qt::black, 2));
        addItem(m_line);
        break;

    case InsertTextMode:
        m_text = new SchematicTextItem();
        m_text->setFont(m_font);
        m_text->setTextInteractionFlags(Qt::TextEditorInteraction);
        m_text->setZValue(1000.0);
        connect(m_text, &SchematicTextItem::LostFocus,
                this, &SchematicScene::EditorLostFocus);
        addItem(m_text);
        m_text->setDefaultTextColor(m_textColor);
        m_text->setPos(mouseEvent->scenePos());
        emit TextInserted(m_text);
        break;

    case InsertNodeMode:
        m_node = new SchematicNode(m_itemMenu);
        m_node->setBrush(m_nodeColor);
        addItem(m_node);
        m_node->setPos(mouseEvent->scenePos());
        emit NodeInserted(m_node);
        break;

    default:;
    }

    QGraphicsScene::mousePressEvent(mouseEvent);
}


void SchematicScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (m_mode == InsertDeviceMode && m_line != nullptr) {
        QLineF newLine(m_line->line().p1(), mouseEvent->scenePos());
        m_line->setLine(newLine);
    } else if (m_mode == BaseMode) {
        QGraphicsScene::mouseMoveEvent(mouseEvent);
    }
}


void SchematicScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (m_line != nullptr && m_mode == InsertDeviceMode) {
        QList<QGraphicsItem *> startItems = items(m_line->line().p1());
        if (startItems.count() && startItems.first() == m_line)
            startItems.removeFirst();
        QList<QGraphicsItem *> endItems = items(m_line->line().p2());
        if (endItems.count() && endItems.first() == m_line)
            endItems.removeFirst();

        removeItem(m_line);
        delete m_line;

        if (startItems.count() > 0 && endItems.count() > 0 &&
            startItems.first()->type() == SchematicNode::Type &&
            endItems.first()->type() == SchematicNode::Type &&
            startItems.first() != endItems.first()) {
                SchematicNode *startNode = qgraphicsitem_cast<SchematicNode *>(startItems.first());
                SchematicNode *endNode = qgraphicsitem_cast<SchematicNode *>(endItems.first());
                SchematicDevice *dev = new SchematicDevice(SchematicDevice::Resistor, startNode, endNode, nullptr);
                dev->setZValue(-1000.0);
                addItem(dev);
                dev->UpdatePosition();
                dev->GetStartNode()->AddDevice(dev);
                dev->GetEndNode()->AddDevice(dev);
        }
    }

    m_line = nullptr;
    QGraphicsScene::mouseReleaseEvent(mouseEvent);
}

bool SchematicScene::IsItemChange(int type) const
{
    const QList<QGraphicsItem *> items = selectedItems();
    const auto cb = [type](const QGraphicsItem *item) { return item->type() == type; };
    return std::find_if(items.begin(), items.end(), cb) != items.end();
}
