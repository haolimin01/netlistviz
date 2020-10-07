#include "ASG.h"
#include <QDebug>
#include "Circuit/Device.h"
#include "Circuit/Terminal.h"
#include "Circuit/CircuitGraph.h"
#include "Schematic/SchematicDevice.h"
#include "Schematic/SchematicTerminal.h"
#include "Schematic/SchematicScene.h"
#include "SchematicWire.h"
#include "Level.h"
#include "Channel.h"

int ASG::GeometricalPlacement(SchematicScene *scene)
{
#ifdef TRACE
    qInfo() << LINE_INFO << endl;
#endif
    int error = 0;

    error = CalGeometricalCol();
    if (error)
        return ERROR;

    error = CalGeometricalRow();
    if (error)
        return ERROR;

#ifdef DEBUGx
    foreach (Level *level, m_levels)
        level->PrintGeometricalPos();
#endif

    error = CreateSchematicDevices();
    if (error)
        return ERROR;

    error = RenderSchematicDevices(scene);
    if (error)
        return ERROR;

    return OKAY;
}


int ASG::CalGeometricalCol()
{
    int colIndex = 0;
    Level *level = nullptr;
    Channel *ch = nullptr;

    Q_ASSERT(m_levels.size() == m_channels.size() + 1);

    for (int i = 0; i < m_channels.size(); ++ i) {
        level = m_levels.at(i);
        level->AssignDeviceGeometricalCol(colIndex);
        colIndex++;
        ch = m_channels.at(i);
        /* Assign dots geometrical col, as to m_dots (same pointer) */
        ch->AssignDotGeometricalCol(colIndex);
        colIndex += ch->HoldColCount();
    }

    m_levels.back()->AssignDeviceGeometricalCol(colIndex);

    return OKAY;
}

int ASG::CalGeometricalRow()
{
    if (m_ckt->GetDeviceList().size() < 1)
        return OKAY;

    const DeviceList &devices = m_ckt->GetDeviceList();
    Device *dev = devices.front();
    int minLogRow = dev->LogicalRow();

    for (int i = 1; i < devices.size(); ++ i) {
        dev = devices.at(i);
        if (dev->LogicalRow() < minLogRow)
            minLogRow = dev->LogicalRow();
    }

#ifdef DEBUGx
    qInfo() << "Min LogicalRow =" << minLogRow;
#endif

    if (minLogRow >= 0)
        return OKAY;

    int shiftDown = -1 * minLogRow;

    int geoRow = 0;
    foreach (Device *dev, devices) {
        geoRow = dev->LogicalRow() + shiftDown;
        dev->SetGeometricalRow(geoRow);
    }

    return OKAY;
}

int ASG::CreateSchematicDevices()
{
    m_sdeviceList.clear();

    SchematicDevice *sdevice = nullptr;
    SchematicTerminal *sterminal = nullptr;
    Device *device = nullptr;
    Terminal *terminal = nullptr;

    foreach (device, m_ckt->GetDeviceList()) {
        sdevice = CreateSchematicDevice(device);
        foreach (terminal, device->GetTerminalList()) {
            sterminal = CreateSchematicTerminal(terminal);
            sdevice->AddTerminal(sterminal->GetTerminalType(), sterminal);
        }
        /* Initialize schematicDevice (Draw device shape, set annotation and terminals pos) */
        sdevice->Initialize();
        m_sdeviceList.push_back(sdevice);
    }

#ifdef DEBUGx
    foreach (sdevice, m_sdeviceList)
        sdevice->Print();
#endif

    return OKAY;
}

SchematicDevice* ASG::CreateSchematicDevice(Device *dev) const
{
    Q_ASSERT(dev);

    SchematicDevice *sdev = new SchematicDevice();
    sdev->SetName(dev->Name());
    sdev->SetId(dev->Id());
    sdev->SetDeviceType(dev->GetDeviceType());
    sdev->SetReverse(dev->Reverse());
    sdev->SetGeometricalPos(/*col*/dev->GeometricalCol(), /*row*/dev->GeometricalRow());
    sdev->SetOrientation(dev->GetOrientation()); 
    dev->SetSchematicDevice(sdev);

    return sdev;
}

SchematicTerminal* ASG::CreateSchematicTerminal(Terminal *ter) const
{
    Q_ASSERT(ter);

    SchematicTerminal *ster = new SchematicTerminal();
    ster->SetId(ter->Id());
    ster->SetNode(ter->GetNode());
    ster->SetTerminalType(ter->GetTerminalType());
    ter->SetSchematicTerminal(ster);

    return ster;
}

int ASG::RenderSchematicDevices(SchematicScene *scene)
{
    Q_ASSERT(scene);

    int maxGeoCol = 0, maxGeoRow = 0;
    foreach (Device *dev, m_ckt->GetDeviceList()) {
        if (dev->GeometricalCol() > maxGeoCol)
            maxGeoCol = dev->GeometricalCol();
        if (dev->GeometricalRow() > maxGeoRow)
            maxGeoRow = dev->GeometricalRow();
    }

    int colCount = maxGeoCol + 1;
    int rowCount = maxGeoRow + 1;

    int error = scene->RenderSchematicDevices(m_sdeviceList, colCount, rowCount, m_ignoreCap);

    return error;
}


