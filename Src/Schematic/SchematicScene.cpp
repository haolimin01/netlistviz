#include "SchematicScene.h"

#include <QGraphicsSceneMouseEvent>
#include <QTextCursor>
#include <QApplication>
#include <QMap>
#include <QDebug>
#include <QGraphicsView>

#include "SchematicData.h"
#include "SchematicWire.h"
#include "Circuit/CktNode.h"

#if 0
const static int Grid_W = 80;
const static int Grid_H = 80;
const static int DIS = 15;
const static int GND_DIS = 15;
#endif

SchematicScene::SchematicScene(QMenu *itemMenu, QObject *parent)
    : QGraphicsScene(parent)
{
    m_itemMenu = itemMenu;

    InitVariables();
}

SchematicScene::~SchematicScene()
{
    m_pointPairs.clear();
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

void SchematicScene::SetShowBranchFlag(bool show)
{
    m_showBranchFlag = show;

    SchematicDevice *device = nullptr;
    SchematicWire *wire = nullptr;
    foreach (QGraphicsItem *item, items()) {
        if (item->type() == SchematicDevice::Type) {
            device = qgraphicsitem_cast<SchematicDevice*> (item);
            device->SetShowOnBranchFlag(show);
            device->update();
        } else if (item->type() == SchematicWire::Type) {
            wire = qgraphicsitem_cast<SchematicWire*> (item);
            wire->SetShowBranchFlag(show);
            wire->update();
        }
    }
}

SchematicDevice* SchematicScene::InsertSchematicDevice(SchematicDevice::DeviceType type,
                                 const QPointF &pos)
{
    SchematicDevice *dev = new SchematicDevice(type, m_itemMenu);
    dev->setPos(pos);
    dev->SetShowNodeFlag(m_showNodeFlag);
    dev->SetShowOnBranchFlag(m_showBranchFlag);
    addItem(dev);

    m_deviceNumber++;
    emit DeviceInserted(dev);

    return dev;
}
SchematicTextItem* SchematicScene::InsertSchematicTextItem(const QPointF &pos)
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

    return m_text;
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

        QMap<TerminalType, QRectF> terRects = device->TerminalRects();
        QMap<TerminalType, QRectF>::const_iterator cit;
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
        QMap<TerminalType, QRectF> terRects = device->TerminalRects();
        QMap<TerminalType, QRectF>::const_iterator cit;
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

SchematicWire* SchematicScene::InsertSchematicWire(SchematicDevice *startDev, SchematicDevice *endDev,
                                TerminalType startTer, TerminalType endTer, const QVector<QPointF> &wirePoints, bool branch)
{
    if (NOT startDev || NOT endDev)  return nullptr;
    // newWire
    SchematicWire *newWire = new SchematicWire(startDev, endDev, startTer, endTer);
    newWire->SetWirePathPoints(wirePoints);
    addItem(newWire);
    if (branch)
        newWire->SetAsBranch(true);
    newWire->SetShowBranchFlag(m_showBranchFlag);

    startDev->AddWire(newWire, startTer);
    endDev->AddWire(newWire, endTer);

    return newWire;
}

SchematicWire* SchematicScene::InsertSchematicWire(const WireDescriptor *desp)
{
    if (NOT desp)  return nullptr;
    // new wire
    SchematicWire *newWire = new SchematicWire(desp->startDev,
        desp->endDev, desp->startTerminal, desp->endTerminal);
    newWire->SetWirePathPoints(desp->pathPoints);
    if (desp->isBranch)
        newWire->SetAsBranch(true);
    newWire->SetShowBranchFlag(m_showBranchFlag);
    addItem(newWire);

    desp->startDev->AddWire(newWire, desp->startTerminal);
    desp->endDev->AddWire(newWire, desp->endTerminal);

    return newWire;
}