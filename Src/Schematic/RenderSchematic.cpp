/*
 * @filename : RenderSchematic.cpp
 * @author   : Hao Limin
 * @date     : 2020.09.02
 * @email    : haolimin01@sjtu.edu.cn
 * @desp     : render schematic from ASG result, a part of SchematicScene class
 */

#include "SchematicScene.h"
#include "Circuit/CktNode.h"
#include <QDebug>

const static int DIS = 15;
const static int GND_DIS = 15;


void SchematicScene::RenderSchematic(const QVector<DevLevelDescriptor*> &devices,
    const QVector<WireDescriptor*> &wireDesps)
{
#ifdef TRACE
    qInfo() << LINE_INFO << endl;
#endif

    m_pointPairs.clear();

    /* Firstly, we do not consider cap */
    if (devices.size() == 0)  return;
    
    int segment = 0;
    qreal sceneHeight = height();
    int maxRowCount = sceneHeight / Grid_H;
    int perY = 0;
    int x = 0, y = 0;
    DevLevelDescriptor *dldesp = nullptr;
    SchematicDevice *device = nullptr;
    CktNode *node = nullptr;
    DeviceList curDeviceList;

    for (int i = 0; i < devices.size(); ++ i) {
        dldesp = devices.at(i);
        segment = dldesp->DeviceCntWithoutCap() + 1;
        perY = maxRowCount / segment;
        curDeviceList = dldesp->AllDevicesWithoutCap();
        for (int j = 0; j < curDeviceList.size(); ++ j) {
            device = curDeviceList.at(j);
            device->SetShowNodeFlag(m_showNodeFlag);
            x = i;
            y = (j + 1) * perY - 1;
            /* set device position */
            SetDeviceAt(x, y, device);
            /* set device orientation */
            DecideDeviceOrientation(x, y, device);
            /* set all node position */
            device->UpdateNodeScenePos();
            /* add GND and connect to devices */
            // RenderGND(x, y, device);
        }
    }

    CktNode *posNode = nullptr, *negNode = nullptr;
    qreal xPos = 0, yPos = 0;
    QPointF itemPos(0, 0);
    /* Secondly, we consider cap */
    for (int i = 0; i < devices.size(); ++ i) {

        dldesp = devices.at(i);
        curDeviceList = dldesp->Devices(C_Priority);
        for (int j = 0; j < curDeviceList.size(); ++ j) {
            device = curDeviceList.at(j);
            device->SetShowNodeFlag(m_showNodeFlag);
            posNode = device->Terminal(Positive);
            negNode = device->Terminal(Negative);

            if (posNode->IsGnd()) { // ground cap

                xPos = negNode->ScenePos().x();
                yPos = negNode->ScenePos().y() + DIS;
                itemPos = device->ScenePosByTerminalScenePos(Negative, QPointF(xPos, yPos));

            } else if (negNode->IsGnd()) { // ground cap

                xPos = posNode->ScenePos().x();
                yPos = posNode->ScenePos().y() + DIS;
                itemPos = device->ScenePosByTerminalScenePos(Positive, QPointF(xPos, yPos));

            } else {
                xPos = (posNode->ScenePos().x() + negNode->ScenePos().x()) / 2;
                yPos = (posNode->ScenePos().y() + negNode->ScenePos().y()) / 2;
                itemPos.rx() = xPos;
                itemPos.ry() = yPos;
            }

            SetDeviceAt(itemPos, device);
            DecideDeviceOrientation(-1, -1, device);
            // RenderGND(-1, -1, device);
        }
    }

    /* Thirdly, we connect devices by wires */
    QVector<QPointF> pathPoints;
    WireDescriptor *desp = nullptr;
    for (int i = 0; i < wireDesps.size(); ++ i) {
        desp = wireDesps.at(i);
        QPointF startPoint = desp->startDev->TerminalScenePos(desp->startTerminal);
        QPointF endPoint = desp->endDev->NodeScenePos(desp->endTerminal);
        if (startPoint == endPoint) {
            endPoint = desp->endDev->TerminalScenePos(desp->endTerminal);
        }

        /* If exists, return true; else insert it and return false */
        if (ContainsWire(startPoint, endPoint)) {
            pathPoints.clear();
            continue;
        }

        pathPoints.push_back(startPoint);
        pathPoints.push_back(endPoint);

        desp->pathPoints = pathPoints;
        InsertSchematicWire(desp);
        // qDebug() << desp->startDev->Name() << " " << desp->endDev->Name() << desp->startTerminal << " " << desp->endTerminal;
        // qDebug() << pathPoints << endl;
        pathPoints.clear();
    }

    /* Fourthly, we add gnd and connect them to other devices */
    for (int i = 0; i < devices.size(); ++ i) {
        dldesp = devices.at(i);
        curDeviceList = dldesp->AllDevices();
        foreach (SchematicDevice *dev, curDeviceList) {
            RenderGND(dev);
        }
    }

    /* Tag device on branch */
    TagDeviceOnBranch();
}

void SchematicScene::SetDeviceAt(int x, int y, SchematicDevice *device)
{
    qreal centerX = x * Grid_W + Grid_W * 0.5;
    qreal centerY = y * Grid_H + Grid_H * 0.5;

    addItem(device);

    /* If x == 0, device is on first level */
    /* First level device is Vsrc or Isrc now */
    QPointF itemPos(centerX, centerY);
    if (x == 0) {
        itemPos = device->ScenePosByTerminalScenePos(Positive, QPointF(centerX, centerY));
    }

    device->setPos(itemPos);
    device->SetSceneXY(x, y);
    device->SetPlaced(true);
}

void SchematicScene::SetDeviceAt(const QPointF &pos, SchematicDevice *device)
{
    addItem(device);
    device->setPos(pos);
    device->SetPlaced(true);
    device->SetSceneXY(pos.x() / Grid_W, pos.y() / Grid_H);
}

void SchematicScene::DecideDeviceOrientation(int x, int y, SchematicDevice* device)
{
    Q_UNUSED(y);
    /* first column */
    CktNode *negNode = nullptr;
    switch (device->GetDeviceType()) {
        case SchematicDevice::Resistor:
        case SchematicDevice::Capacitor:
        case SchematicDevice::Inductor:
        case SchematicDevice::Isrc:
        case SchematicDevice::Vsrc:
            negNode = device->Terminal(Negative);
        default:;
    }

    SchematicDevice::Orientation orien = SchematicDevice::Horizontal;


    if (negNode->IsGnd()) {
        orien = SchematicDevice::Vertical;
    }

    if (x == 0) {
        orien = SchematicDevice::Vertical;
    }

    device->SetOrientation(orien);
}

void SchematicScene::RenderGND(SchematicDevice *device)
{
    SchematicDevice *gnd = nullptr;
    CktNode *node = nullptr;
    int gndX = 0, gndY = 0;
    QPointF gndPos, terminalPos;
    QPointF startPoint, endPoint;
    QVector<QPointF> pathPoints;
    switch (device->GetDeviceType()) {
        case SchematicDevice::Resistor:
        case SchematicDevice::Capacitor:
        case SchematicDevice::Inductor:
        case SchematicDevice::Isrc:
        case SchematicDevice::Vsrc:
            node = device->Terminal(Positive);
            if (node->IsGnd()) {
                terminalPos = device->TerminalScenePos(Positive);
                if (device->GetOrientation() == SchematicDevice::Horizontal) {
                    gndPos.setX(terminalPos.x() + GND_DIS);
                    gndPos.setY(terminalPos.y());
                    gnd = InsertSchematicDevice(SchematicDevice::GND, gndPos);
                    gnd->SetOrientation(SchematicDevice::Horizontal);
                }
                if (device->GetOrientation() == SchematicDevice::Vertical) {
                    gndPos.setX(terminalPos.x());
                    gndPos.setY(terminalPos.y() + GND_DIS);
                    gnd = InsertSchematicDevice(SchematicDevice::GND, gndPos);
                    gnd->SetOrientation(SchematicDevice::Vertical);
                }
                gnd->SetShowNodeFlag(m_showNodeFlag);
                startPoint = terminalPos;
                endPoint = gnd->TerminalScenePos(General);
                if (NOT ContainsWire(startPoint, endPoint)) {
                    pathPoints.push_back(startPoint);
                    pathPoints.push_back(endPoint);
                    InsertSchematicWire(device, gnd, Positive, General, pathPoints);
                }
            }
            pathPoints.clear();

            node = device->Terminal(Negative);
            if (node->IsGnd()) {
                terminalPos = device->TerminalScenePos(Negative);
                if (device->GetOrientation() == SchematicDevice::Horizontal) {
                    gndPos.setX(terminalPos.x() + GND_DIS);
                    gndPos.setY(terminalPos.y());
                    gnd = InsertSchematicDevice(SchematicDevice::GND, gndPos);
                    gnd->SetOrientation(SchematicDevice::Horizontal);
                }
                if (device->GetOrientation() == SchematicDevice::Vertical) {
                    gndPos.setX(terminalPos.x());
                    gndPos.setY(terminalPos.y() + GND_DIS);
                    gnd = InsertSchematicDevice(SchematicDevice::GND, gndPos);
                    gnd->SetOrientation(SchematicDevice::Vertical);
                }
                gnd->SetShowNodeFlag(m_showNodeFlag);
                startPoint = terminalPos;
                endPoint = gnd->TerminalScenePos(General);
                if (NOT ContainsWire(startPoint, endPoint)) {
                    pathPoints.push_back(startPoint);
                    pathPoints.push_back(endPoint);
                    InsertSchematicWire(device, gnd, Negative, General, pathPoints);
                }
            }
            pathPoints.clear();
            break;
        default:;
    }
}

/* If terminals contain branch wire, and it's not capacitor,
 * we tag this device on branch.
 */
void SchematicScene::TagDeviceOnBranch()
{
    SchematicDevice *dev = nullptr;
    foreach (QGraphicsItem *item, items()) {
        if (item->type() == SchematicDevice::Type) {
            dev = qgraphicsitem_cast<SchematicDevice*> (item);
            if (dev->GetDeviceType() == SchematicDevice::Capacitor)
                continue;
            if (dev->TerminalsContainBranchWire()) {
                dev->SetOnBranch(true);
                dev->update();
            }
        }
    }
}

/* smaller as key */
bool SchematicScene::ContainsWire(const MyPointF &p1, const MyPointF &p2)
{
    MyPointF kP = qMin(p1, p2);
    MyPointF vP = qMax(p1, p2);
    
    QList<MyPointF> values = m_pointPairs.values(kP);
    foreach (const MyPointF &value, values) {
        if (value == vP) 
            return true; // found !!!
    }

    /* not found */
    m_pointPairs.insert(kP, vP);
    return false;
}