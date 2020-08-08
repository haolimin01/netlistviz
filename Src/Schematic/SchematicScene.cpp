#include "SchematicScene.h"

#include <QGraphicsSceneMouseEvent>
#include <QTextCursor>
#include <QApplication>
#include <QMap>
#include <QDebug>
#include <QGraphicsView>

#include "SchematicData.h"
#include "SchematicWire.h"

const static int Grid_W = 80;
const static int Grid_H = 80;


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
    m_startDevice = nullptr;
    m_endDevice = nullptr;
    m_textColor = Qt::black;
    m_deviceType = SchematicDevice::Resistor;
    m_deviceColor = Qt::black;
    m_deviceNumber = 0;
    m_line = nullptr;
    m_showNodeFlag = false;
    m_backgroundFlag = true;
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

void SchematicScene::drawBackground(QPainter *painter, const QRectF &rect)
{
    Q_UNUSED(rect);
    if (NOT m_backgroundFlag)  return;

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
#ifdef TRACEx
    qInfo() << LINE_INFO << endl;
#endif

    if (mouseEvent->button() != Qt::LeftButton)
        return;

    switch (m_mode) {
        case BaseMode:
            ProcessMousePress(mouseEvent->scenePos());
            break;

        case InsertDeviceMode:
            InsertSchematicDevice(m_deviceType, mouseEvent->scenePos());
            break;

        case InsertTextMode:
            InsertSchematicTextItem(mouseEvent->scenePos());
            break;
        
        case InsertWireMode:
            FinishDrawingWireAt(mouseEvent->scenePos());
            break;

        default:;
    }

    QGraphicsScene::mousePressEvent(mouseEvent);
}

void SchematicScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
#ifdef TRACEx
    qInfo() << LINE_INFO << endl;
#endif

    /* do something */
    QPointF scenePos = mouseEvent->scenePos();

    switch (m_mode) {
        case BaseMode:
            SenseDeviceTerminal(scenePos);
            break;
        case InsertWireMode:
            SenseDeviceTerminal(scenePos);
            DrawWireTowardDeviceTerminal(scenePos);
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

void SchematicScene::SetShowNodeFlag(bool show)
{
    m_showNodeFlag = show;

    SchematicDevice *device = nullptr;
    foreach (QGraphicsItem *item, items()) {
        if (item->type() == SchematicDevice::Type) {
            device = qgraphicsitem_cast<SchematicDevice*> (item);
            device->SetShowNodeFlag(show);
            device->update(); 
        }
    }
}

void SchematicScene::InsertSchematicDevice(SchematicDevice::DeviceType type,
                                           const QPointF &pos)
{
    SchematicDevice *dev = new SchematicDevice(type, m_itemMenu);
    dev->setPos(pos);
    dev->SetShowNodeFlag(m_showNodeFlag);
    addItem(dev);

    m_deviceNumber++;
    emit DeviceInserted(dev);
}
void SchematicScene::InsertSchematicTextItem(const QPointF &pos)
{
    m_text = new SchematicTextItem();
    m_text->setFont(m_font);
    m_text->setTextInteractionFlags(Qt::TextEditorInteraction);
    m_text->setZValue(0);
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

/*
    Check whether the mouse is right at a device port.
    If yes, this function changes the cursor to a CROSS "+" and returns'
    Otherwise, it restores the cursor to an ARROW.
    Called by mouseMoveEvent() to detect a device port
 */
void SchematicScene::SenseDeviceTerminal(const QPointF &scenePos) const
{
#ifdef TRACEx
    qInfo() << LINE_INFO << endl;
#endif
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

/*
    When the mouse is pressed in base mode, this function differentiates whether
    it is clicked on
    1) A device terminal, then start drawing a wire towards its destination
    2) A device (not a terminal), then the device is selected.
*/
void SchematicScene::ProcessMousePress(const QPointF &scenePos)
{
#ifdef TRACEx
    qInfo() << LINE_INFO << endl;
#endif

    QGraphicsItem *item = nullptr;
    SchematicDevice *device = nullptr;
    QPainterPath path;

    item = itemAt(scenePos, QTransform());
    if (NOT item)  return;

    if (item->type() == SchematicDevice::Type) {
        device = qgraphicsitem_cast<SchematicDevice *>(item);
        if (NOT device)  return;

        QMap<NodeType, QRectF> terRects = device->TerminalRects();
        QMap<NodeType, QRectF>::const_iterator cit;
        for (cit = terRects.constBegin(); cit != terRects.constEnd(); ++ cit) {
            if (cit.value().contains(device->mapFromScene(scenePos))) {
                m_startDevice = device;
                m_startTerminal = cit.key();
                m_startPoint = device->mapToScene(cit.value().center());

                /* Change to InsertWireMode */
                m_mode = InsertWireMode;
                m_line = new QGraphicsLineItem(QLineF(m_startPoint, m_startPoint));
                m_line->setPen(QPen(Qt::cyan, 1));
                addItem(m_line);
                m_line->setZValue(-1000);

                m_curWirePathPoints.clear();
                m_curWirePathPoints.append(m_startPoint);
                break;
            }
        }
    }
}

void SchematicScene::DrawWireTowardDeviceTerminal(const QPointF &scenePos)
{
    if (NOT m_line)
        return;

   QLineF newLine(m_startPoint, scenePos);
   m_line->setLine(newLine); 
}

void SchematicScene::FinishDrawingWireAt(const QPointF &scenePos)
{
    QGraphicsItem *item = nullptr;
    SchematicDevice *device = nullptr;

    item = itemAt(scenePos, QTransform());
    if (NOT item)  return;
    if (item->type() == SchematicDevice::Type) {
        device = qgraphicsitem_cast<SchematicDevice *>(item);
        QMap<NodeType, QRectF> terRects = device->TerminalRects();
        QMap<NodeType, QRectF>::const_iterator cit;
        for (cit = terRects.constBegin(); cit != terRects.constEnd(); ++ cit) {
            if (cit.value().contains(device->mapFromScene(scenePos))) {
                m_endDevice = device;
                m_endTerminal = cit.key();
                if (m_startDevice == m_endDevice && m_startTerminal == m_endTerminal) {
                    m_curWirePathPoints.clear();
                    m_mode = BaseMode;
                    views().first()->setCursor(Qt::ArrowCursor);
                    return;
                }
                m_endPoint = device->mapToScene(cit.value().center());
                m_curWirePathPoints.append(m_endPoint);
                m_mode = BaseMode;

                InsertSchematicWire(m_startDevice, m_endDevice, m_startTerminal,
                    m_endTerminal, m_curWirePathPoints);
                
                if (m_line)  delete m_line;

                m_curWirePathPoints.clear();
                views().first()->setCursor(Qt::ArrowCursor);
                break;
            }
        }
    }
}

void SchematicScene::InsertSchematicWire(SchematicDevice *startDev, SchematicDevice *endDev,
                                NodeType startTer, NodeType endTer, const QVector<QPointF> &wirePoints)
{
    if (NOT startDev || NOT endDev)  return;
    // newWire
    SchematicWire *newWire = new SchematicWire(startDev, endDev, startTer, endTer);
    newWire->SetWirePathPoints(wirePoints);
    addItem(newWire);
    newWire->setPos(0, 0);

    startDev->AddWire(newWire, startTer);
    endDev->AddWire(newWire, endTer);
}
                    
/* Render Level Device List to SchematicScene, for ASG */
void SchematicScene::RenderSchematic(const QVector<QVector<SchematicDevice*>> &m_levels)
{
#ifdef TRACE
    qInfo() << LINE_INFO << endl;
#endif
    if (m_levels.size() == 0)  return;

    /* place device firstly */
    int segment = 0;
    int sceneHeight = height();
    int maxRowCount = sceneHeight / Grid_H;
    int perY = 0;
    int x = 0, y = 0;
    QVector<SchematicDevice*> curDevices;
    SchematicDevice *device = nullptr;

    for (int i = 0; i < m_levels.size(); ++ i) {
        curDevices = m_levels.at(i);
        segment = curDevices.size() + 1;
        perY = maxRowCount / segment;
        for (int j = 0; j < curDevices.size(); ++ j) {
            device = curDevices.at(j);
            x = i;
            y = (j + 1) * perY - 1;
            SetDeviceAt(x, y, device);
            if (x != 0)
                device->SetOrientation(SchematicDevice::Horizontal);
        }
    }

    /* connect devices by wires */
}

/* row and col start from 0 */
void SchematicScene::SetDeviceAt(int x, int y, SchematicDevice *device)
{
    qreal centerX = x * Grid_W + Grid_W * 0.5;
    qreal centerY = y * Grid_H + Grid_H * 0.5;

    addItem(device);
    device->setPos(centerX, centerY);
    device->SetSceneXY(x, y);
    device->SetPlaced(true);
}

/* Read and Write SchematicData */
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

#if 0
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
#endif
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

#if 0
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
#endif
}