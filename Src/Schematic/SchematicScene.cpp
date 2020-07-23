#include "SchematicScene.h"

#include <QGraphicsSceneMouseEvent>
#include <QTextCursor>
#include <QApplication>
#include <QMap>
#include <QDebug>
#include <QGraphicsView>

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
    m_device = nullptr;
    m_textColor = Qt::black;
    m_deviceType = SchematicDevice::Resistor;
    m_deviceColor = Qt::black;
    m_deviceNumber = 0;
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


void SchematicScene::SetDeviceType(SchematicDevice::DeviceType type)
{
    m_deviceType = type;
}


// BUG
void SchematicScene::RenderSchematicData(SchematicData *data)
{
#ifdef TRACE
    qInfo() << LINE_INFO << endl;
#endif
    Q_ASSERT(data);
    InitVariables();

    /* Update node and device number */
    // m_nodeNumber = data->m_nodeList.size();
    m_deviceNumber = data->m_deviceList.size();

    /* Remove and Delete all items */
    clear();

    m_schLayout->GeneratePos(data, SchematicLayout::Square);

    /* t => temp */
    // SchematicNode *tNode = nullptr;
    SchematicDevice *tDev = nullptr;

    // for (int i = 0; i < data->m_nodeList.size(); ++ i) {
    //     tNode = data->m_nodeList.at(i);
    //     tNode->SetContextMenu(m_itemMenu);
    //     addItem(tNode);
    // }

    for (int i = 0; i < data->m_deviceList.size(); ++ i) {
        tDev = data->m_deviceList.at(i);
        tDev->SetContextMenu(m_itemMenu);
        addItem(tDev);
    }
}


// BUG
void SchematicScene::WriteSchematicToStream(QTextStream &stream) const
{
    /* Node   : type name id isGnd color size pos*/
    /* Device : type device_type name start_node_id end_node_id value */
    /* Text   : type text color (family x) bold italic underline size pos */

    /* Device must handle lastly for loading */

#ifdef TRACE
    qInfo() << LINE_INFO << endl;
#endif

    QApplication::setOverrideCursor(Qt::WaitCursor);
    // SchematicNode *tNode = nullptr;
    SchematicDevice *tDev = nullptr;
    SchematicTextItem *tText = nullptr;

    /* Node and TextItem */
    foreach (QGraphicsItem *item, items()) {
        // if (item->type() == SchematicNode::Type) {
        //     stream << item->type() << ' ';
        //     tNode = qgraphicsitem_cast<SchematicNode *>(item);
        //     stream << tNode->GetName() << ' ';
        //     stream << tNode->GetId() << ' ';
        //     stream << tNode->IsGnd() << ' ';
        //     stream << tNode->GetColorName() << ' ';
        //     stream << tNode->GetSize() << ' ';
        //     stream << tNode->x() << ' ' << tNode->y() << '\n';
        // } else if (item->type() == SchematicTextItem::Type) {
        if (item->type() == SchematicTextItem::Type) {
            stream << item->type() << ' ';
            tText = qgraphicsitem_cast<SchematicTextItem *>(item);
            stream << tText->GetText() << ' ';
            stream << tText->GetColorName() << ' '; 
            // stream << tText->GetFontFamily() << ' ';
            stream << tText->IsBold() << ' ';
            stream << tText->IsItalic() << ' ';
            stream << tText->IsUnderline() << ' ';
            stream << tText->GetSize() << ' ';
            stream << tText->x() << ' ' << tText->y() << '\n';
        }
    }

    /* Device */
    foreach (QGraphicsItem *item, items()) {
        if (item->type() == SchematicDevice::Type) {
            stream << item->type() << ' ';
            tDev = qgraphicsitem_cast<SchematicDevice *>(item);
            stream << tDev->GetDeviceType() << ' ';
            stream << tDev->GetName() << ' ';
            stream << tDev->GetPosNodeId() << ' ';
            stream << tDev->GetNegNodeId() << ' ';
            stream << tDev->GetValue() << ' ' << '\n';
        }
    }

    QApplication::setOverrideCursor(Qt::ArrowCursor);
}


// BUG
void SchematicScene::LoadSchematicFromStream(QTextStream &stream)
{
    /* Node   : type name id isGnd color size pos*/
    /* Device : type device_type name start_node_id end_node_id value */
    /* Text   : type text color (family x) bold italic underline size pos */

#ifdef TRACE
    qInfo() << LINE_INFO << endl;
#endif

    InitVariables();

    /* Remove and Delete all items */
    clear();

    QString name, text, fontFamily, colorName;
    qreal x = 0, y = 0, size = 0, value = 0;
    int itemType = 0, deviceType = 0, id = 0;
    int startNodeId = 0, endNodeId = 0;
    int bold, italic, underline, isGnd;

    SchematicNode *tNode = nullptr;
    SchematicDevice *tDev = nullptr;
    SchematicTextItem *tText = nullptr;

    SchematicNode *tStartNode = nullptr, *tEndNode = nullptr;

    /* Used to store node id and node ptr temporarily */
    QMap<int, SchematicNode*> tNodeMap;

    while (NOT stream.atEnd()) {
        stream >> itemType;
        if (itemType == SchematicNode::Type) {
#ifdef DEBUG
            qDebug() << "Read SchematicNode";
#endif
            stream >> name >> id >> isGnd >> colorName >> size >> x >> y;

            tNode = new SchematicNode(m_itemMenu);
            tNode->SetName(name);
            tNode->SetId(id);
            tNode->SetId(isGnd);
            tNode->SetColor(QColor(colorName));
            tNode->SetSize(size);
            tNode->setPos(x, y);
            addItem(tNode);
            // m_nodeNumber++;

            tNodeMap.insert(id, tNode);

        } else if (itemType == SchematicDevice::Type) {
#ifdef DEBUG
            qDebug() << "Read SchematicDevice";
#endif
            stream >> deviceType >> name >> startNodeId >> endNodeId >> value;

            tStartNode = tNodeMap.value(startNodeId, /* default */nullptr);
            tEndNode = tNodeMap.value(endNodeId, /* default */nullptr);
            Q_ASSERT(tStartNode AND tEndNode);

            // tDev = new SchematicDevice((SchematicDevice::DeviceType)deviceType, tStartNode, tEndNode);
            tDev = new SchematicDevice((SchematicDevice::DeviceType)deviceType, m_itemMenu);
            tDev->SetName(name);
            tDev->SetValue(value);

            addItem(tDev);

            tStartNode->AddDevice(tDev);
            tEndNode->AddDevice(tDev);

            m_deviceNumber++;

        } else if (itemType == SchematicTextItem::Type) {
#ifdef DEBUG
            qDebug() << "Read SchematicTextItem";
#endif
            // stream >> text >> colorName >> fontFamily
            //        >> bold >> italic >> underline >> size >> x >> y;
            stream >> text >> colorName
                   >> bold >> italic >> underline >> size >> x >> y;
            
            tText = new SchematicTextItem();
            tText->setPlainText(text);
            tText->setDefaultTextColor(QColor(colorName));

            /* TextItem font */
            QFont font;
            font.setFamily(fontFamily);
            font.setBold(bold);
            font.setItalic(italic);
            font.setUnderline(underline);
            font.setPointSize(size);

            tText->setFont(font);
            tText->setPos(x, y);
            
            addItem(tText);

        }
        // else {
        //     qCritical() << "[ERROR] Unknown item type: " << itemType << endl;
        //     EXIT;
        // }
    }
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
            InsertSchematicDevice(m_deviceType, mouseEvent->scenePos());
            break;

        case InsertTextMode:
            InsertSchematicTextItem(mouseEvent->scenePos());
            break;

        default:;
    }

    QGraphicsScene::mousePressEvent(mouseEvent);
}


void SchematicScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
#ifdef TRACE
    qInfo() << LINE_INFO << endl;
#endif

    /* do something */
    QPointF scenePos = mouseEvent->scenePos();

    switch (m_mode) {
        case BaseMode:
            SenseDeviceTerminal(scenePos);
            break;
    }

    QGraphicsScene::mouseMoveEvent(mouseEvent);
}


void SchematicScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    /* do something */
    QGraphicsScene::mouseReleaseEvent(mouseEvent);
}


void SchematicScene::InsertSchematicDevice(SchematicDevice::DeviceType type,
                                           const QPointF &pos)
{
    SchematicDevice *dev = new SchematicDevice(type, m_itemMenu);
    dev->setPos(pos);
    addItem(dev);

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


/* Check whether the mouse is right at a device port.
   If yes, this function changes the cursor to a CROSS "+" and returns'
   Otherwise, it restores the cursor to an ARROW.
   Called by mouseMoveEvent() to detect a device port */
void SchematicScene::SenseDeviceTerminal(const QPointF &scenePos) const
{
#ifdef TRACE
    qInfo() << LINE_INFO << endl;
#endif
    QGraphicsItem *item;
    SchematicDevice *device;

    item = itemAt(scenePos, QTransform());
    if (item) {
        if (item->type() == SchematicDevice::Type) {
            device = qgraphicsitem_cast<SchematicDevice *>(item);
            if (NOT device)  return;

            QVector<QRectF> portRects = device->GetTerminalRects();
            for (int i = 0; i < portRects.size(); ++ i) {
                if (portRects[i].contains(device->mapFromScene(scenePos))) {
                    views().first()->setCursor(Qt::CrossCursor);
                    return;
                }
            }
        }
    }

    views().first()->setCursor(Qt::ArrowCursor);
}