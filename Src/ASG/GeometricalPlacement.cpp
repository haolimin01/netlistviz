#include "ASG.h"
#include <QDebug>
#include "Circuit/Device.h"
#include "Circuit/Terminal.h"
#include "Circuit/CircuitGraph.h"
#include "HyperLevel.h"
#include "Schematic/SchematicDevice.h"
#include "Schematic/SchematicTerminal.h"
#include "Schematic/SchematicScene.h"
#include "SchematicWire.h"

int ASG::GeometricalPlacement(SchematicScene *scene)
{
#ifdef TRACE
    qInfo() << LINE_INFO << endl;
#endif
    int error = 0;
    error = DecideDeviceWhetherToReverse();
    if (error)
        return ERROR;

    error = CalGeometricalCol();
    if (error)
        return ERROR;

    error = CalGeometricalRow();
    if (error)
        return ERROR;

#ifdef DEBUG
    foreach (HyperLevel *hl, m_hyperLevels)
        hl->PrintGeometricalPos();
#endif

    // error = LinkDeviceForGCCap();
    // if (error)
    //     return ERROR;

    error = CreateSchematicDevices();
    if (error)
        return ERROR;

    error = RenderSchematicDevices(scene);
    if (error)
        return ERROR;

    return OKAY;
}

int ASG::DecideDeviceWhetherToReverse()
{
    switch (m_ignoreCap) {
        case IgnoreNoCap:
            return DecideDeviceWhetherToReverseIgnoreNoCap();
        case IgnoreGCap:
            return DecideDeviceWhetherToReverseIgnoreGCap();
        case IgnoreGCCap:
            return DecideDeviceWhetherToReverseIgnoreGCCap();
        default:
            qInfo() << "UNknown ignore cap type " << m_ignoreCap << endl;
    }
}


int ASG::DecideDeviceWhetherToReverseIgnoreNoCap()
{
    /* First, set maxDeviceCount HyperLevel as no reverse */
    int indexOfMaxDeviceCount = 0;
    HyperLevel *levelOfMaxDeviceCount = m_hyperLevels.front();
    int maxDeviceCountInLevel = levelOfMaxDeviceCount->AllDeviceCount();

    // Level *level = nullptr;
    HyperLevel *hl = nullptr;

    for (int i = 1; i < m_hyperLevels.size(); ++ i) {
        // level = m_levels.at(i);
        hl = m_hyperLevels.at(i);
        if (hl->AllDeviceCount() >= maxDeviceCountInLevel) {
            maxDeviceCountInLevel = hl->AllDeviceCount();
            indexOfMaxDeviceCount = i;
            levelOfMaxDeviceCount = hl;
        }
    }

    foreach (Device *dev, levelOfMaxDeviceCount->AllDevices()) {
        dev->SetReverse(false);
    }

    /* Second, deal with [0, indexOfMaxDeviceCount) */
    for (int i = indexOfMaxDeviceCount - 1; i >= 0; -- i) {
        hl = m_hyperLevels.at(i);
        foreach (Device *dev, hl->AllDevices())
            dev->DecideReverseBySuccessors(m_ignoreCap);
    }

    /* Third, deal with (indexOfMaxDeviceCount, last] */
    for (int i = indexOfMaxDeviceCount + 1; i < m_levels.size(); ++ i) {
        hl = m_hyperLevels.at(i);
        foreach (Device *dev, hl->AllDevices())
            dev->DecideReverseByPredecessors(m_ignoreCap);
    }

#ifdef DEBUG
    qDebug() << "--------------- Device Reverse ---------------";
    foreach (Device *dev, m_ckt->GetDeviceList()) {
        qDebug() << dev->Name() << " reverse(" << dev->Reverse() << ")";
    }
    qDebug() << "----------------------------------------------";
#endif

    return OKAY;
}

int ASG::DecideDeviceWhetherToReverseIgnoreGCap()
{
#ifdef TRACE
    qInfo() << LINE_INFO << endl;
#endif
#if 0
    /* ignore ground cap */
    int indexOfMaxDeviceCount = 0;
    Level *levelOfMaxDeviceCount = m_levels.front();
    int maxDeviceCountInLevel = levelOfMaxDeviceCount->DeviceCountWithoutGCap();

    Level *level = nullptr;

    for (int i = 1; i < m_levels.size(); ++ i) {
        level = m_levels.at(i);
        if (level->DeviceCountWithoutGCap() >= maxDeviceCountInLevel) {
            maxDeviceCountInLevel = level->DeviceCountWithoutGCap();
            indexOfMaxDeviceCount = i;
            levelOfMaxDeviceCount = level;
        }
    }

    foreach (Device *dev, levelOfMaxDeviceCount->AllDevices()) {
        dev->SetReverse(false);
    }

    /* Second, deal with [0, indexOfMaxDeviceCount) */
    for (int i = indexOfMaxDeviceCount - 1; i >= 0; -- i) {
        level = m_levels.at(i);
        foreach (Device *dev, level->AllDevices()) {
            if (dev->GroundCap()) continue;
            dev->DecideReverseBySuccessors(m_ignoreCap); // ignore ground cap
        }
    }

    /* Third, deal with (indexOfMaxDeviceCount, lastLevel] */
    for (int i = indexOfMaxDeviceCount + 1; i < m_levels.size(); ++ i) {
        level = m_levels.at(i);
        foreach (Device *dev, level->AllDevices()) {
            if (dev->GroundCap()) continue;
            dev->DecideReverseByPredecessors(m_ignoreCap); // ignore ground cap
        }
    }

#ifdef DEBUG
    qDebug() << "--------------- Device Reverse ---------------";
    foreach (Device *dev, m_ckt->GetDeviceList()) {
        qDebug() << dev->Name() << " reverse(" << dev->Reverse() << ")";
    }
    qDebug() << "----------------------------------------------";
#endif
#endif
    return OKAY;
}

int ASG::DecideDeviceWhetherToReverseIgnoreGCCap()
{
#ifdef TRACE
    qInfo() << LINE_INFO << endl;
#endif
#if 0
    /* ignore coupled and ground cap */
    int indexOfMaxDeviceCount = 0;
    Level *levelOfMaxDeviceCount = m_levels.front();
    int maxDeviceCountInLevel = levelOfMaxDeviceCount->DeviceCountWithoutGCCap();

    Level *level = nullptr;

    for (int i = 1; i < m_levels.size(); ++ i) {
        level = m_levels.at(i);
        if (level->DeviceCountWithoutGCCap() >= maxDeviceCountInLevel) {
            maxDeviceCountInLevel = level->DeviceCountWithoutGCCap();
            indexOfMaxDeviceCount = i;
            levelOfMaxDeviceCount = level;
        }
    }

    foreach (Device *dev, levelOfMaxDeviceCount->AllDevices()) {
        dev->SetReverse(false);
    }

    /* Second, deal with [0, indexOfMaxDeviceCount) */
    for (int i = indexOfMaxDeviceCount - 1; i >= 0; -- i) {
        level = m_levels.at(i);
        foreach (Device *dev, level->AllDevices()) {
            if (dev->GroundCap()) continue;
            if (dev->CoupledCap()) continue;
            dev->DecideReverseBySuccessors(m_ignoreCap); // ignore coupled and ground cap
        }
    }

    /* Third, deal with (indexOfMaxDeviceCount, lastLevel] */
    for (int i = indexOfMaxDeviceCount + 1; i < m_levels.size(); ++ i) {
        level = m_levels.at(i);
        foreach (Device *dev, level->AllDevices()) {
            if (dev->GroundCap()) continue;
            if (dev->CoupledCap()) continue;
            dev->DecideReverseByPredecessors(m_ignoreCap); // ignore coupled and ground cap
        }
    }

#ifdef DEBUG
    qDebug() << "--------------- Device Reverse ---------------";
    foreach (Device *dev, m_ckt->GetDeviceList()) {
        qDebug() << dev->Name() << " reverse(" << dev->Reverse() << ")";
    }
    qDebug() << "----------------------------------------------";
#endif
#endif
    return OKAY;
}

int ASG::CalGeometricalCol()
{
    HyperLevel *hl = nullptr;
    int colIndex = 0;
    Device *dev = nullptr;
    foreach (hl, m_hyperLevels) { 
        hl->AssignGeometricalCol(colIndex);
        colIndex++; // for channel between hyperlevels
    }

    return OKAY;
}

int ASG::CalGeometricalRow()
{
    /* geometricalRow = logicalRow now */

    Device *dev = nullptr;

    foreach (dev, m_ckt->GetDeviceList())
        dev->SetGeometricalRow(dev->LogicalRow());

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
        sdevice = new SchematicDevice(device);
        foreach (terminal, device->GetTerminalList()) {
            sterminal = new SchematicTerminal(terminal);
            sdevice->AddTerminal(sterminal->GetTerminalType(), sterminal);
            terminal->SetSchematicTerminal(sterminal);
        }
        /* Initialize schematicDevice (Draw device shape, set annotation and terminals pos) */
        sdevice->Initialize();
        sdevice->SetOrientation(device->GetOrientation()); // ugly!
        m_sdeviceList.push_back(sdevice);
        device->SetSchematicDevice(sdevice);
    }

#ifdef DEBUG
    foreach (sdevice, m_sdeviceList)
        sdevice->Print();
#endif

    return OKAY;
}

int ASG::LinkDeviceForGCCap()
{
#if 0
#ifdef TRACE
    qInfo() << LINE_INFO << endl;
#endif
    Node *node = nullptr;
    foreach (Device *dev, m_ckt->GetDeviceList()) {
        if ((NOT dev->GroundCap()) && (NOT dev->CoupledCap())) continue;
        foreach (Terminal *ter, dev->GetTerminalList()) {
            if (ter->NodeId() == 0) continue; // ground
            node = ter->GetNode();
            foreach (Device *cntDev, node->ConnectDeviceList()) {
                if (cntDev != dev) {
                    dev->AddConnectDevice(cntDev);
                    break;
                }
            }
        }
    }

#ifdef DEBUG
    QString tmp = "";
    foreach (Device *dev, m_ckt->GetDeviceList()) {
        if ((NOT dev->GroundCap()) && (NOT dev->CoupledCap())) continue;
        if (dev->GroundCap())
            tmp += "GroundCap(";
        else if (dev->CoupledCap())
            tmp += "CoupledCap(";
        tmp += (dev->Name() + ") ");
        tmp += ("ConnectTo ");
        foreach (Device *dev, dev->CapConnectDeviceList())
            tmp += ("(" + dev->Name() + ") ");
        
        qInfo() << tmp;
        tmp = "";
    }
#endif

    return OKAY;
#endif
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


