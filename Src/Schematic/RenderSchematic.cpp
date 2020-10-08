#include "SchematicScene.h"
#include <QDebug>
#include "SchematicTerminal.h"
#include "SchematicWire.h"
#include "SchematicDot.h"
#include "SConnector.h"

int SchematicScene::RenderSchematicDevices(const SDeviceList &devices, int colCount,
                                           int rowCount, IgnoreCap ignore)
{
#ifdef TRACE
    qInfo() << LINE_INFO << endl;
#endif

#ifdef DEBUG
    qDebug() << LINE_INFO << "colCount(" << QString::number(colCount) << "), "
             << "rowCount(" << rowCount << ")";
#endif

    clear();
    m_gCapDeviceList.clear();
    m_cCapDeviceList.clear();
    m_gndList.clear();
    m_hasGndWireList.clear();

    /* 1. Change device scale according to col and row count */
    ChangeDeviceScale(colCount, rowCount);

    /* 2. In order to place all devices at scene center position,
     *    calculate start col and row.
     */
    int startRow = CalStartRow(rowCount);
    int startCol = CalStartCol(colCount);

    /* 3. Set device geometrical position and add to scene */
    SDeviceList gCaps;
    int sceneCol = 0, sceneRow = 0;
    SchematicDevice *device = nullptr;
    foreach (device, devices) {
        if (device->GroundCap()) {
            m_gCapDeviceList.push_back(device);
            if ((ignore == IgnoreGCap) || (ignore == IgnoreGCCap)) continue;
        }
        if (device->CoupledCap()) {
            m_cCapDeviceList.push_back(device);
            if (ignore == IgnoreGCCap) continue;
        }

        sceneRow = device->GeometricalRow() + startRow;
        sceneCol = device->GeometricalCol() + startCol;
        device->SetScenePos(/*col*/sceneCol, /*row*/sceneRow);
        SetDeviceAt(sceneCol, sceneRow, device);
    }

    /* 4. Render extra widgets, such as gnd, groundCap, coupledCap */
    if (ignore == IgnoreGCCap) RenderCoupledCaps(m_cCapDeviceList);
    if (ignore == IgnoreGCap || ignore == IgnoreGCCap)
        RenderGroundCaps(m_gCapDeviceList);
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
    qreal h = height();
    int totalRowCount = (h - 2 * m_margin) / m_gridH;
    if (totalRowCount <= rowCount)
        return 0;

    return (totalRowCount - rowCount) / 2;
}

int SchematicScene::CalStartCol(int colCount) const
{
    qreal w = width();
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

/* row, col is the geometrical's pos, not logical col and row */
/* consider scene margin */
void SchematicScene::SetDeviceAt(int col, int row, SchematicDevice *device)
{
    qreal xPos = (col + 0.5) * m_gridW + m_margin;
    qreal yPos = (row + 0.5) * m_gridH + m_margin;
    device->setPos(xPos, yPos);
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
    SConnector *scd = nullptr;
    bool reverse = false;

    m_gndList.clear();
    m_hasGndWireList.clear();

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

                    if (device->GetOrientation() == Vertical && device->Reverse()) {
                        gndPos.rx() = devTerPos.x();
                        gndPos.ry() = devTerPos.y() - DFT_GND_DIS * m_itemScale;
                        reverse = true;
                    } else {
                        gndPos.rx() = devTerPos.x();
                        gndPos.ry() = devTerPos.y() + DFT_GND_DIS * m_itemScale;
                        reverse = false;
                    }

                    gnd = InsertSchematicDevice(GND, gndPos);

                    scd = new SConnector(terminal, gnd->GetTerminal(General), gnd);
                    device->AddConnector(scd);
                    scd = new SConnector(gnd->GetTerminal(General), terminal, device);
                    gnd->AddConnector(scd);

                    gnd->SetOrientation(Vertical);
                    gnd->SetReverse(reverse);
                    gnd->SetScenePos(device->SceneCol(), 0);

                    gndTer = gnd->GetTerminal(General);
                    wire = new SchematicWire(device, gnd, terminal, gndTer);
                    wire->SetTrack(-1);
                    m_gndList.push_back(gnd);
                    m_hasGndWireList.push_back(wire);
                }

                terminal = device->GetTerminal(Negative);
                if (terminal->ConnectToGnd()) {
                    devTerPos = terminal->ScenePos();

                    if (device->GetOrientation() == Vertical && device->Reverse()) {
                        gndPos.rx() = devTerPos.x();
                        gndPos.ry() = devTerPos.y() - DFT_GND_DIS * m_itemScale;
                        reverse = true;
                    } else {
                        gndPos.rx() = devTerPos.x();
                        gndPos.ry() = devTerPos.y() + DFT_GND_DIS * m_itemScale;
                        reverse = false;
                    }

                    gnd = InsertSchematicDevice(GND, gndPos);

                    scd = new SConnector(terminal, gnd->GetTerminal(General), gnd);
                    device->AddConnector(scd);
                    scd = new SConnector(gnd->GetTerminal(General), terminal, device);
                    gnd->AddConnector(scd);

                    gnd->SetOrientation(Vertical);
                    gnd->SetReverse(reverse);
                    gnd->SetScenePos(device->SceneCol(), 0);

                    gndTer = gnd->GetTerminal(General);
                    wire = new SchematicWire(device, gnd, terminal, gndTer);
                    wire->SetTrack(-1);
                    m_gndList.push_back(gnd);
                    m_hasGndWireList.push_back(wire);
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
            connectTer = cap->ConnectTerminal();
            connectTerPos = connectTer->ScenePos();
            capTerPos.rx() = connectTerPos.x();
            capTerPos.ry() = connectTerPos.y() + 2 * DFT_DIS * m_itemScale;
            capPos = cap->ScenePosByTerminalScenePos(capTer, capTerPos);
            capPos = capTerPos;
            cap->SetOrientation(Vertical);
            SetDeviceAt(capPos, cap);
        }

        capTer = cap->GetTerminal(Negative);
        if (NOT capTer->ConnectToGnd()) {
            connectTer = cap->ConnectTerminal();
            connectTerPos = connectTer->ScenePos();
            capTerPos.rx() = connectTerPos.x();
            capTerPos.ry() = connectTerPos.y() + 2 * DFT_DIS * m_itemScale;
            capPos = cap->ScenePosByTerminalScenePos(capTer, capTerPos);
            capPos = capTerPos;
            cap->SetOrientation(Vertical);
            SetDeviceAt(capPos, cap);
        }
    }
}

void SchematicScene::RenderCoupledCaps(const SDeviceList &ccaps)
{
    SchematicDevice *cap = nullptr;
    QPointF capPos, cntPosTerPos, cntNegTerPos;

    foreach (cap, ccaps) {
        cntPosTerPos = cap->ConnectTerminal(Positive)->ScenePos();
        cntNegTerPos = cap->ConnectTerminal(Negative)->ScenePos();
        capPos.rx() = (cntPosTerPos.x() + cntNegTerPos.x()) / 2;
        capPos.ry() = (cntPosTerPos.y() + cntNegTerPos.y()) / 2;
        cap->SetOrientation(Vertical);
        SetDeviceAt(capPos, cap);
    }
}

int SchematicScene::RenderSchematicWires(const SWireList &wires)
{
#ifdef TRACE
    qInfo() << LINE_INFO << endl;
#endif

    m_hasGCapWireList.clear();
    m_hasCCapWireList.clear();

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
        if (wire->HasGroundCap())  m_hasGCapWireList.push_back(wire);
        if (wire->HasCoupledCap()) m_hasCCapWireList.push_back(wire);
    }

    /* 2. add to scene */
    AddWiresToScene(wires);

    /* 3. add wires to gnd */
    AddWiresToScene(m_hasGndWireList);

    return OKAY;
}

void SchematicScene::AddWiresToScene(const SWireList &wires)
{
    SchematicWire *wire = nullptr;
    SchematicTerminal *terminal = nullptr;

    foreach (wire, wires) {
        terminal = wire->StartTerminal();
        terminal->AddWire(wire);

        terminal = wire->EndTerminal();
        terminal->AddWire(wire);

        wire->SetWirePathPoints(CreateWirePathPoints(wire));
        wire->SetScale(m_itemScale);
        addItem(wire);
    }
}

QVector<QPointF> SchematicScene::CreateWirePathPoints(SchematicWire *wire) const
{
    Q_ASSERT(wire);

    QVector<QPointF> points;
    QPointF startPoint, endPoint;
    startPoint = wire->StartTerminal()->ScenePos();
    endPoint = wire->EndTerminal()->ScenePos();
    int track = wire->Track();

    if (track == -1) { // horizontal wire or connect to gnd
        points.push_back(startPoint);
        points.push_back(endPoint);
        return points;
    }

    qreal totalWidth = m_gridW * wire->HoldColCount();
    int trackCount = wire->TrackCount();

#ifdef DEBUGx
    qDebug() << "track=" << track << "trackCount=" << trackCount
             << wire->StartDevice()->Name() << wire->EndDevice()->Name() << endl;
#endif

    Q_ASSERT(trackCount > 0);

    qreal gap = totalWidth / (trackCount + 1);
    int sceneCol = wire->StartDevice()->SceneCol() + 1;

#ifdef DEBUGx
    qDebug() << "wire sceneCol=" << sceneCol;
    qDebug() << "startDevSceneCol=" << wire->StartDevice()->SceneCol();
    qDebug() << "endDevSceneCol=" << wire->EndDevice()->SceneCol();
    qDebug() << "holdColCount=" << wire->HoldColCount();
#endif

    wire->SetSceneCol(sceneCol);

    qreal startX = sceneCol * m_gridW + m_margin; 
    qreal sceneX = startX + (track + 1) * gap;

    QPointF segPoint1 = QPointF(sceneX, startPoint.y());
    QPointF segPoint2 = QPointF(sceneX, endPoint.y());

#ifdef DEBUGx
    qDebug() << "start=" << startPoint << ", end=" << endPoint
             << "startX=" << startX << ", gridW=" << m_gridW << endl;
#endif

    points.push_back(startPoint);
    points.push_back(segPoint1);
    points.push_back(segPoint2);
    points.push_back(endPoint);

    return points;
}

void SchematicScene::HideGroundCaps(bool hide)
{
#ifdef TRACEx
    qInfo() << LINE_INFO << endl;
#endif

#if 0
    if (hide) {

        foreach (SchematicDevice *gcap, m_gCapDeviceList)
            removeItem(gcap);

        foreach (SchematicWire *gwire, m_hasGCapWireList)
            removeItem(gwire);

    } else {

        // RenderGroundCaps(m_gCapDeviceList);
        foreach (SchematicDevice *cap, m_gCapDeviceList)
            SetDeviceAt(cap->scenePos(), cap);

        AddWiresToScene(m_hasGCapWireList);

    }
#endif

    foreach (SchematicDevice *gcap, m_gCapDeviceList) {
        gcap->setVisible(NOT hide);
        gcap->SetAnnotationVisible(NOT hide);
    }

    foreach (SchematicWire *gwire, m_hasGCapWireList)
        gwire->setVisible(NOT hide);
}

void SchematicScene::HideCoupledCaps(bool hide)
{
#ifdef TRACEx
    qInfo() << LINE_INFO << endl;
#endif

#if 0
    if (hide) {

        foreach (SchematicDevice *ccap, m_cCapDeviceList)
            removeItem(ccap);

        foreach (SchematicWire *cwire, m_hasCCapWireList)
            removeItem(cwire);

    } else {

        // RenderCoupledCaps(m_cCapDeviceList);
        foreach (SchematicDevice *cap, m_cCapDeviceList)
            SetDeviceAt(cap->scenePos(), cap);

        AddWiresToScene(m_hasCCapWireList);

    }
#endif
    foreach (SchematicDevice *ccap, m_cCapDeviceList) {
        ccap->setVisible(NOT hide);
        ccap->SetAnnotationVisible(NOT hide);
    }

    foreach (SchematicWire *cwire, m_hasCCapWireList)
        cwire->setVisible(NOT hide);
}

void SchematicScene::HideGnds(bool hide)
{
#ifdef TRACEx
    qInfo() << LINE_INFO << endl;
#endif

#if 0
    if (hide) {

        foreach (SchematicDevice *gnd, m_gndList)
            removeItem(gnd);

        foreach (SchematicWire *wire, m_hasGndWireList)
            removeItem(wire);

    } else {

        QPointF terPos, gndPos;
        foreach (SchematicDevice *gnd, m_gndList) {
            terPos = gnd->ConnectTerminal()->ScenePos();
            gndPos.rx() = terPos.x();
            gndPos.ry() = terPos.y() + DFT_DIS * m_itemScale;
            SetDeviceAt(gndPos, gnd);
        }

        AddWiresToScene(m_hasGndWireList);

    }
#endif

    foreach (SchematicDevice *gnd, m_gndList)
        gnd->setVisible(NOT hide);

    foreach (SchematicWire *wire, m_hasGndWireList)
        wire->setVisible(NOT hide);
}

int SchematicScene::RenderSchematicDots(const SDotList &dots)
{
    QPointF pos;

    foreach (SchematicDot *dot, dots) {
        pos = SeekDotScenePos(dot);
        dot->SetScale(m_itemScale);
        dot->setPos(pos);
        addItem(dot);
    }

    return OKAY;
}

QPointF SchematicScene::SeekDotScenePos(SchematicDot *dot) const
{
    int trackCount = dot->TrackCount();
    int track = dot->Track();

    qreal totalWidth = m_gridW * dot->HoldColCount();

    Q_ASSERT(trackCount > 0);

    qreal gap = totalWidth / (trackCount + 1);
    int geoCol = dot->GeometricalCol();

    SchematicTerminal *ter = dot->GetSchematicTerminal();
    int sceneCol = ter->SceneCol() + geoCol - ter->GeometricalCol();
    int sceneRow = ter->SceneRow();
    dot->SetScenePos(sceneCol, sceneRow);

    qreal startX = sceneCol * m_gridW + m_margin; 
    qreal sceneX = startX + (track+1) * gap;
    qreal sceneY = ter->ScenePos().y();

    return QPointF(sceneX, sceneY);
}
