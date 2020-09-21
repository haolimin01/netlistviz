#include "SchematicScene.h"
#include <QDebug>
#include "SchematicTerminal.h"
#include "SchematicWire.h"

int SchematicScene::RenderSchematicDevices(const SDeviceList &devices,
                    int colCount, int rowCount, bool ignoreGroundCap)
{
#ifdef TRACE
    qInfo() << LINE_INFO << endl;
#endif

#ifdef DEBUG
    qDebug() << LINE_INFO << "colCount(" << QString::number(colCount) << "), "
             << "rowCount(" << rowCount << ")";
#endif

    clear();

    /* 1. Change device scale according to col and row count */
    ChangeDeviceScale(colCount, rowCount);

    /* 2. Change device orientation (BUG) */ 
    ChangeDeviceOrientation(devices);

    /* 3. In order to place all devices at scene center position,
     *    calculate start col and row.
     */

    int startRow = CalStartRow(rowCount);
    int startCol = CalStartCol(colCount);

    /* 4. Set device geometrical position and add to scene */
    SDeviceList gCaps;
    int geoCol = 0, geoRow = 0;
    SchematicDevice *device = nullptr;
    foreach (device, devices) {
        if (ignoreGroundCap && device->GroundCap()) {
            gCaps.push_back(device);
            continue;
        }
        geoRow = device->LogicalRow() + startRow;
        geoCol = device->LogicalCol() * 2 + startCol;
        device->SetGeometricalPos(/*col*/geoCol, /*row*/geoRow);
        SetDeviceAt(geoCol, geoRow, device);
    }


    /* 5. Render extra widgets, such as gnd, groundCap */
    if (ignoreGroundCap) RenderGroundCaps(gCaps);
    RenderFixedGnds(devices); // not device

    return OKAY;
}

/* DO NOT change scene rect, instead of rescaling device and grid */
void SchematicScene::ChangeDeviceScale(int colCount, int rowCount)
{
    qreal newScale = CalDeviceScale(colCount, rowCount);
    if (newScale < 1) {
        m_itemScale = newScale;
        m_gridW *= newScale;
        m_gridH *= newScale;
        UpdateDeviceScale(newScale);
        qDebug() << LINE_INFO << "newScale " << newScale << endl; 
    }
}

qreal SchematicScene::CalDeviceScale(int colCount, int rowCount)
{
    Q_ASSERT(colCount > 0 && rowCount > 0);
    qreal currWidth = width() - m_margin * 2;   // col
    qreal currHeight = height() - m_margin * 2; // row

    qreal currColCount = currWidth / m_gridW;
    qreal currRowCount = currHeight / m_gridH;
    qreal widthScale = currColCount / colCount;
    qreal heightScale = currRowCount / rowCount;

    qreal scale = (widthScale < heightScale)? widthScale : heightScale;
    return scale;
}

int SchematicScene::CalStartRow(int rowCount) const
{
    int h = height();
    int totalRowCount = (h - 2 * m_margin) / m_gridH;
    if (totalRowCount <= rowCount)
        return 0;

    return (totalRowCount - rowCount) / 2;
}

int SchematicScene::CalStartCol(int colCount) const
{
    int w = width();
    int totalColCount = (w - 2 * m_margin) / m_gridW;
    if (totalColCount <= colCount)
        return 0;

    return (totalColCount - colCount) / 2;
}

void SchematicScene::UpdateDeviceScale(qreal newScale)
{
    SchematicDevice *device = nullptr;
    foreach (QGraphicsItem *item, items()) {
        if (item->type() == SchematicDevice::Type) {
            device = qgraphicsitem_cast<SchematicDevice*> (item);
            device->SetScale(newScale);
            device->update(); 
        }
    }
}

void SchematicScene::UpdateWireScale(qreal newScale)
{
    SchematicWire *wire = nullptr;
    foreach (QGraphicsItem *item, items()) {
        if (item->type() == SchematicWire::Type) {
            wire = qgraphicsitem_cast<SchematicWire*> (item);
            wire->SetScale(newScale);
            wire->update();
        }
    }
}

void SchematicScene::ChangeDeviceOrientation(const SDeviceList &devices)
{
    SchematicTerminal *negTer = nullptr;
    // Orientation orien = Horizontal;

    foreach (SchematicDevice *dev, devices) {
        switch (dev->GetDeviceType()) {
            case RESISTOR:
            case CAPACITOR:
            case INDUCTOR:
            case ISRC:
            case VSRC:
                negTer = dev->GetTerminal(Negative);
            default:;
        }
        if (NOT negTer) continue;

        // if (dev->GroundCap())
        //     orien = Vertical;
        
        // dev->SetOrientation(orien);

        dev->SetOrientation(Horizontal);
    }
}

/* row, col is the geometrical's pos, not logical col and row */
/* consider scene margin */
void SchematicScene::SetDeviceAt(int col, int row, SchematicDevice *device)
{
    qreal xPos = col * m_gridW + m_margin;
    qreal yPos = row * m_gridH + m_margin;
    device->setPos(xPos, yPos);
    // device->SetGeometricalPos(/*col*/col, /*row*/row);
    device->SetShowTerminal(m_showTerminal);
    device->SetScale(m_itemScale);
    addItem(device);
}

void SchematicScene::SetDeviceAt(const QPointF &pos, SchematicDevice *device)
{
    device->setPos(pos);
    device->SetShowTerminal(m_showTerminal);
    device->SetScale(m_itemScale);
    addItem(device);
}

void SchematicScene::RenderFixedGnds(const SDeviceList &devices)
{
    SchematicDevice *gnd = nullptr;
    SchematicTerminal *terminal = nullptr;
    QPointF devTerPos, gndPos;
    SchematicWire *wire = nullptr;
    SchematicTerminal *gndTer = nullptr;
    
    m_extraWires.clear();

    foreach (SchematicDevice *device, devices) {
        switch (device->GetDeviceType()) {
            case RESISTOR:
            case CAPACITOR:
            case INDUCTOR:
            case ISRC:
            case VSRC:
                terminal = device->GetTerminal(Positive);
                if (terminal->ConnectToGnd()) {
                    devTerPos = terminal->ScenePos();
                    gndPos.rx() = devTerPos.x();
                    gndPos.ry() = devTerPos.y() + DFT_DIS * m_itemScale;
                    gnd = InsertSchematicDevice(GND, gndPos);
                    gnd->SetOrientation(Vertical);

                    gndTer = gnd->GetTerminal(General);
                    wire = new SchematicWire(device, gnd,
                            terminal, gndTer);
                    m_extraWires.push_back(wire);
                }

                terminal = device->GetTerminal(Negative);
                if (terminal->ConnectToGnd()) {
                    devTerPos = terminal->ScenePos();
                    gndPos.rx() = devTerPos.x();
                    gndPos.ry() = devTerPos.y() + DFT_DIS * m_itemScale;
                    gnd = InsertSchematicDevice(GND, gndPos);
                    gnd->SetOrientation(Vertical);

                    gndTer = gnd->GetTerminal(General);
                    wire = new SchematicWire(device, gnd,
                            terminal, gndTer);
                    m_extraWires.push_back(wire);
                }
                break;
            default:;
        }
    }
}

void SchematicScene::RenderGroundCaps(const SDeviceList &gcaps)
{
    SchematicDevice *cap = nullptr;
    SchematicTerminal *capTer = nullptr, *connectTer = nullptr;
    QPointF capPos, capTerPos, connectTerPos;

    foreach (cap, gcaps) {

        capTer = cap->GetTerminal(Positive);
        if (NOT capTer->ConnectToGnd()) {
            connectTer = cap->GroundCapConnectTerminal();
            connectTerPos = connectTer->ScenePos();
            capTerPos.rx() = connectTerPos.x();
            capTerPos.ry() = connectTerPos.y() + 2 * DFT_DIS * m_itemScale;
            // capPos = cap->ScenePosByTerminalScenePos(capTer, capTerPos);
            capPos = capTerPos;
            cap->SetOrientation(Vertical);
            SetDeviceAt(capPos, cap);
        }

        capTer = cap->GetTerminal(Negative);
        if (NOT capTer->ConnectToGnd()) {
            connectTer = cap->GroundCapConnectTerminal();
            connectTerPos = connectTer->ScenePos();
            capTerPos.rx() = connectTerPos.x();
            capTerPos.ry() = connectTerPos.y() + 2 * DFT_DIS * m_itemScale;
            // capPos = cap->ScenePosByTerminalScenePos(capTer, capTerPos);
            capPos = capTerPos;
            cap->SetOrientation(Vertical);
            SetDeviceAt(capPos, cap);
        }
    }
}

int SchematicScene::RenderSchematicWires(const SWireList &wires)
{
#ifdef TRACE
    qInfo() << LINE_INFO << endl;
#endif

    QGraphicsItem *item = nullptr;
    foreach (item, items()) {
        if (item->type() == SchematicWire::Type) {
            removeItem(item);
            delete item;
        }
    }

    /* DO NOT consider track now */
    /* 1. assign geometrical col. */
    SchematicWire *wire = nullptr;
    foreach (wire, wires) {
        int geoCol = wire->LogicalCol() * 2 + 1;
        wire->SetGeometricalCol(geoCol);
    }

    /* 2. add to scene */
    QVector<QPointF> wirePoints;
    SchematicTerminal *terminal = nullptr;
    foreach (wire, wires) {
        terminal = wire->StartTerminal();
        wirePoints.push_back(terminal->ScenePos());
        terminal->AddWire(wire);

        terminal = wire->EndTerminal();
        wirePoints.push_back(terminal->ScenePos());
        terminal->AddWire(wire);

        wire->SetWirePathPoints(wirePoints);
        wire->SetScale(m_itemScale);
        addItem(wire);
        wirePoints.clear();
    }


    /* 3. add wires to gnd */
    foreach (wire, m_extraWires) {
        terminal = wire->StartTerminal();
        wirePoints.push_back(terminal->ScenePos());
        terminal->AddWire(wire);

        terminal = wire->EndTerminal();
        wirePoints.push_back(terminal->ScenePos());
        terminal->AddWire(wire);

        wire->SetWirePathPoints(wirePoints);
        wire->SetScale(m_itemScale);
        addItem(wire);
        wirePoints.clear();
    }

    return OKAY;
}
