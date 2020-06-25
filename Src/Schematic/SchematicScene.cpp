#include "SchematicScene.h"

#include <QGraphicsSceneMouseEvent>
#include <QTextCursor>
#include <QApplication>
#include <QDebug>

#include "SchematicData.h"
#include "SchematicLayout.h"


SchematicScene::SchematicScene(QMenu *itemMenu, QObject *parent)
    : QGraphicsScene(parent)
{
    m_itemMenu = itemMenu;

    InitVariables();
    m_schLayout = new SchematicLayout;
}


SchematicScene::~SchematicScene()
{
}


/* Do NOT handle m_data */
void SchematicScene::InitVariables()
{
    m_mode = BaseMode;
    m_text = nullptr;
    m_node = nullptr;
    m_device = nullptr;
    m_line = nullptr;
    m_textColor = Qt::black;
    m_nodeColor = Qt::black;
    m_deviceType = SchematicDevice::Resistor;
    m_nodeColor = SchematicDevice::GetColorFromDeviceType(m_deviceType);
    m_nodeNumber = 0;
    m_deviceNumber = 0;
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
        item->SetColor(m_nodeColor);
        item->update();
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


void SchematicScene::RenderSchematicData(SchematicData *data)
{
#ifdef TRACE
    qInfo() << LINE_INFO << endl;
#endif
    Q_ASSERT(data);
    InitVariables();

    /* Update node and device number */
    m_nodeNumber = data->m_nodeList.size();
    m_deviceNumber = data->m_deviceList.size();

    /* Remove and Delete all items */
    clear();

    m_schLayout->GeneratePos(data, SchematicLayout::Square);

    /* t => temp */
    SchematicNode *tNode = nullptr;
    SchematicDevice *tDev = nullptr;

    for (int i = 0; i < data->m_nodeList.size(); ++ i) {
        tNode = data->m_nodeList.at(i);
        tNode->SetContextMenu(m_itemMenu);
        addItem(tNode);
    }

    for (int i = 0; i < data->m_deviceList.size(); ++ i) {
        tDev = data->m_deviceList.at(i);
        tDev->SetContextMenu(m_itemMenu);
        tDev->UpdatePosition();
        addItem(tDev);
    }
}


void SchematicScene::WriteSchematicToStream(QTextStream &stream) const
{
    /* Node   : type name id isGnd color size pos*/
    /* Device : type device_type name start_node_id end_node_id value */
    /* Text   : type text color family bold italic underline size pos */

    QApplication::setOverrideCursor(Qt::WaitCursor);
    SchematicNode *tNode = nullptr;
    SchematicDevice *tDev = nullptr;
    SchematicTextItem *tText = nullptr;

    foreach (QGraphicsItem *item, items()) {
        if (item->type() == SchematicNode::Type) {
            stream << item->type() << ' ';
            tNode = qgraphicsitem_cast<SchematicNode *>(item);
            stream << tNode->GetName() << ' ';
            stream << tNode->GetId() << ' ';
            stream << tNode->IsGnd() << ' ';
            stream << tNode->GetColorName() << ' ';
            stream << tNode->GetSize() << ' ';
            stream << tNode->x() << ' ' << tNode->y() << '\n';
        }
        else if (item->type() == SchematicDevice::Type) {
            stream << item->type() << ' ';
            tDev = qgraphicsitem_cast<SchematicDevice *>(item);
            stream << tDev->GetDeviceType() << ' ';
            stream << tDev->GetName() << ' ';
            stream << tDev->GetStartNodeId() << ' ';
            stream << tDev->GetEndNodeId() << ' ';
            stream << tDev->GetValue() << ' ' << '\n';
        } else if (item->type() == SchematicTextItem::Type) {
            stream << item->type() << ' ';
            tText = qgraphicsitem_cast<SchematicTextItem *>(item);
            stream << tText->GetText() << ' ';
            stream << tText->GetColorName() << ' '; 
            stream << tText->GetFontFamily() << ' ';
            stream << tText->IsBold() << ' ';
            stream << tText->IsItalic() << ' ';
            stream << tText->IsUnderline() << ' ';
            stream << tText->GetSize() << ' ';
            stream << tText->x() << ' ' << tText->y() << '\n';
        }
    }
    QApplication::setOverrideCursor(Qt::ArrowCursor);
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
            /* Insert temp line */
            m_line = new QGraphicsLineItem(QLineF(mouseEvent->scenePos(),
                                                mouseEvent->scenePos()));
            m_line->setPen(QPen(SchematicDevice::GetColorFromDeviceType(m_deviceType), 2));
            addItem(m_line);
            break;

        case InsertTextMode:
            InsertSchematicTextItem(mouseEvent->scenePos());
            break;

        case InsertNodeMode:
            InsertSchematicNode(mouseEvent->scenePos());
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
                InsertSchematicDevice(m_deviceType, startNode, endNode);
        }
    }

    m_line = nullptr;
    QGraphicsScene::mouseReleaseEvent(mouseEvent);
}


void SchematicScene::InsertSchematicDevice(SchematicDevice::DeviceType type,
        SchematicNode *startNode, SchematicNode *endNode)
{
    SchematicDevice *dev = new SchematicDevice(type, startNode, endNode, nullptr);
    dev->setZValue(-1000.0);
    addItem(dev);
    dev->UpdatePosition();
    startNode->AddDevice(dev);
    endNode->AddDevice(dev);
    QString name;
    double value = 0;

    switch (type) {
        case SchematicDevice::Resistor:
            name = "R" + QString::number(m_deviceNumber); 
            value = 1000;
            break;
        case SchematicDevice::Capacitor:
            name = "C" + QString::number(m_deviceNumber);
            value = 1e-12;
            break;
        case SchematicDevice::Inductor:
            name = "L" + QString::number(m_deviceNumber);
            value = 1e-3; 
            break;
        case SchematicDevice::Isrc:
            name = "I" + QString::number(m_deviceNumber);
            value = 1;
            break;
        case SchematicDevice::Vsrc:
            name = "V" + QString::number(m_deviceNumber);
            value = 1;
            break;
        default:;
    }

    m_deviceNumber++;
    dev->SetName(name);
    dev->SetValue(value);
    emit DeviceInserted(dev);
}


void SchematicScene::InsertSchematicNode(const QPointF &pos)
{
    m_node = new SchematicNode(m_itemMenu);
    m_node->setBrush(m_nodeColor);
    addItem(m_node);
    m_node->setPos(pos);
    m_node->SetId(m_nodeNumber);
    /* N + m_nodeNumber => NodeName */
    QString nodeName = "N" + QString::number(m_nodeNumber);
    m_node->SetName(nodeName);
    m_nodeNumber++;
    emit NodeInserted(m_node);
}


void SchematicScene::InsertSchematicTextItem(const QPointF &pos)
{
    m_text = new SchematicTextItem();
    m_text->setFont(m_font);
    m_text->setTextInteractionFlags(Qt::TextEditorInteraction);
    m_text->setZValue(1000.0);
    connect(m_text, &SchematicTextItem::LostFocus,
            this, &SchematicScene::EditorLostFocus);
    addItem(m_text);
    m_text->setDefaultTextColor(m_textColor);
    m_text->setPos(pos);
    emit TextInserted(m_text);
}


bool SchematicScene::IsItemChange(int type) const
{
    const QList<QGraphicsItem *> items = selectedItems();
    const auto cb = [type](const QGraphicsItem *item) { return item->type() == type; };
    return std::find_if(items.begin(), items.end(), cb) != items.end();
}
