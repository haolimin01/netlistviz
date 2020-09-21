#include "SchematicScene.h"
#include <QGraphicsSceneMouseEvent>
#include <QTextCursor>
#include <QApplication>
#include <QMap>
#include <QDebug>
#include <QGraphicsView>
#include "SchematicTerminal.h"
#include "SchematicWire.h"


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
    m_textColor = Qt::black;
    m_deviceType = RESISTOR;
    m_deviceColor = Qt::black;
    m_showTerminal = false;
    m_showBackground = true;

    m_startDevice = nullptr;
    m_startTerminal = nullptr;
    m_endDevice = nullptr;
    m_endTerminal = nullptr;
    m_line = nullptr;

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
    if (mouseEvent->button() == Qt::RightButton) {
        switch (m_mode) {
            case BaseMode:
                break;
            case InsertWireMode:
                m_mode = BaseMode;
                m_startDevice = nullptr;
                m_startTerminal = nullptr;
                m_startPoint = QPointF(0, 0);
                m_currWirePathPoints.clear();
                removeItem(m_line);
                if (m_line) {
                    delete m_line;
                    m_line = nullptr;
                }
                break;
            case InsertTextMode:
                break;
            case InsertDeviceMode:
                break;
            default:;
        }
        return;
    }

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

    SchematicTextItem *text = new SchematicTextItem();
    text->setFont(m_font);
    text->setTextInteractionFlags(Qt::TextEditorInteraction);
    text->setScale(m_itemScale);
    text->setZValue(0);
    connect(text, &SchematicTextItem::LostFocus,
            this, &SchematicScene::EditorLostFocus);
    addItem(text);
    text->setDefaultTextColor(m_textColor);
    text->setPos(pos);
    emit TextInserted(text);

    return text;
}

SchematicWire* SchematicScene::InsertSchematicWire(SchematicDevice *sd, SchematicDevice *ed,
    SchematicTerminal *st, SchematicTerminal *et, const QVector<QPointF> &wirePoints)
{
    if (NOT sd || NOT ed || NOT st || NOT et) return nullptr;
    SchematicWire *newWire = new SchematicWire(sd, ed, st, et);
    newWire->SetWirePathPoints(wirePoints);
    newWire->SetScale(m_itemScale);
    addItem(newWire);

    st->AddWire(newWire);
    et->AddWire(newWire);

    return newWire;
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

/*
    When the mouse is pressed in base mode, this function differentiates whether
    it is clicked on
    1) A device terminal, then start drawing a wire towards its destination
    2) A device (not a terminal), then the device is selected.
*/
void SchematicScene::ProcessMousePress(const QPointF &scenePos)
{
    QGraphicsItem *item = nullptr;
    SchematicDevice *device = nullptr;
    QPainterPath path;

    item = itemAt(scenePos, QTransform());
    if (NOT item)  return;

    if (item->type() == SchematicDevice::Type) {
        device = qgraphicsitem_cast<SchematicDevice *>(item);
        if (NOT device) return;

        const STerminalTable &terTable = device->GetTerminalTable();
        STerminalTable::const_iterator cit = terTable.constBegin();
        for (; cit != terTable.constEnd(); ++ cit) {
            if (cit.value()->Rect().contains(device->mapFromScene(scenePos))) {
                m_startDevice = device;
                m_startTerminal = cit.value();
                m_startPoint = cit.value()->ScenePos();

                /* Change mode to InsertWireMode */
                m_mode = InsertWireMode;
                if (m_line) delete m_line;

                m_line = new QGraphicsLineItem(QLineF(m_startPoint, m_startPoint));
                m_line->setPen(QPen(Qt::cyan, 1));
                addItem(m_line);
                m_line->setZValue(-1000);

                m_currWirePathPoints.clear();
                m_currWirePathPoints.append(m_startPoint);
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
        const STerminalTable &terTable = device->GetTerminalTable();
        STerminalTable::const_iterator cit = terTable.constBegin();
        for (; cit != terTable.constEnd(); ++ cit) {
            if (cit.value()->Rect().contains(device->mapFromScene(scenePos))) {
                m_endDevice = device;
                m_endTerminal = cit.value();
                if ((m_startDevice == m_endDevice) && (m_startTerminal == m_endTerminal)) {
                    m_currWirePathPoints.clear();
                    m_mode = BaseMode;
                    views().first()->setCursor(Qt::ArrowCursor);
                    return;                    
                }
                QPointF endPoint = cit.value()->ScenePos();
                m_currWirePathPoints.append(endPoint);
                m_mode = BaseMode;

                // InsertSchematicWire
                InsertSchematicWire(m_startDevice, m_endDevice, m_startTerminal, m_endTerminal, m_currWirePathPoints);

                if (m_line) {
                    delete m_line;
                    m_line = nullptr;
                }

                m_currWirePathPoints.clear();
                views().first()->setCursor(Qt::ArrowCursor);
                return;
            }
        } 
    }
}
