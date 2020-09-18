#include "SchematicScene.h"
#include <QDebug>
#include "SchematicTerminal.h"

int SchematicScene::RenderSchematicDevices(const SDeviceList &devices,
                    int colCount, int rowCount)
{
#ifdef TRACE
    qInfo() << LINE_INFO << endl;
#endif

    qDebug() << LINE_INFO << "colCount(" << QString::number(colCount) << "), "
             << "rowCount(" << rowCount << ")";

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
    int geoCol = 0, geoRow = 0;
    SchematicDevice *device = nullptr;
    foreach (device, devices) {
        geoRow = device->LogicalRow() + startRow;
        geoCol = device->LogicalCol() * 2 + startCol;
        device->SetGeometricalPos(/*col*/geoCol, /*row*/geoRow);
        SetDeviceAt(geoCol, geoRow, device);
    }

    /* 5. Render extra widgets, such as gnd */
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

void SchematicScene::RenderFixedGnds(const SDeviceList &devices)
{
    SchematicDevice *gnd = nullptr;
    SchematicTerminal *terminal = nullptr;
    QPointF devTerPos, gndPos;

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
                }

                terminal = device->GetTerminal(Negative);
                if (terminal->ConnectToGnd()) {
                    devTerPos = terminal->ScenePos();
                    gndPos.rx() = devTerPos.x();
                    gndPos.ry() = devTerPos.y() + DFT_DIS * m_itemScale;
                    gnd = InsertSchematicDevice(GND, gndPos);
                    gnd->SetOrientation(Vertical);
                }
                break;
            default:;
        }
    }
}
