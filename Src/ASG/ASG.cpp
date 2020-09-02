#include "ASG.h"
#include <cassert>
#include <QDebug>
#include "Matrix.h"
#include "Schematic/SchematicData.h"
#include "Schematic/SchematicDevice.h"
#include "Schematic/SchematicWire.h"
#include "Circuit/CktNode.h"
#include "MatrixElement.h"
#include "Utilities/MyString.h"
#include "TablePlotter.h"

using std::set;

/* =============== DevLevelDescriptor class =============== */
DevLevelDescriptor::DevLevelDescriptor(int level)
{
    Q_ASSERT(level >= 0);

    m_level = level;
    m_deviceCntWithoutCap = 0;
    m_capCnt = 0;
}

DevLevelDescriptor::~DevLevelDescriptor()
{
    m_levelDevices.clear();
}

void DevLevelDescriptor::AddDevice(SchematicDevice *device)
{
    int priority = device->Priority();
    if (m_levelDevices.contains(priority)) {
        m_levelDevices[priority].push_back(device);
    } else {
        DeviceList devices;
        devices.push_back(device);
        m_levelDevices.insert(priority, devices);
    }

    if (device->GetDeviceType() == SchematicDevice::Capacitor)
        m_capCnt += 1;
    else
        m_deviceCntWithoutCap += 1;
}

void DevLevelDescriptor::AddDevices(const DeviceList &devList)
{
    foreach (SchematicDevice *dev, devList) {
        AddDevice(dev);
    }
}

DeviceList DevLevelDescriptor::Devices(int priority) const
{
    if (m_levelDevices.contains(priority))
        return m_levelDevices[priority];
    else 
        return DeviceList();  // empty
}

DeviceList DevLevelDescriptor::AllDevices() const
{
    DeviceList devices;

    foreach (DeviceList dev, m_levelDevices.values()) {
        devices.append(dev);
    }

    return devices;
}

DeviceList DevLevelDescriptor::AllDevicesWithoutCap() const
{
    DeviceList devices;
    foreach (int priority, m_levelDevices.keys()) {
        if (priority != C_Priority)
            devices.append(m_levelDevices[priority]);
    }

    return devices;
}

void DevLevelDescriptor::PrintAllDevices() const
{
    printf("--------------- Device Level %d ----------------\n", m_level);

    QMap<int, DeviceList>::const_iterator cit;
    cit = m_levelDevices.constBegin();
    for (; cit != m_levelDevices.constEnd(); ++ cit) {
        foreach (SchematicDevice *device, cit.value())
            qInfo() << device->Name();
    }

    printf("------------------------------------------------\n");
}


/* =============== ASG class =============== */
ASG::ASG(SchematicData *data)
{
    assert(data);
    m_ckt = data;

    m_matrix = nullptr;

    m_buildMatrixFlag = false;
    m_levellingFlag = false;
    m_bubblingFlag = false;

    m_levelPlotter = nullptr;
    m_bubblePlotter = nullptr;

    m_visited = new int[m_ckt->DeviceCnt()];
    memset(m_visited, 0, sizeof(int) * m_ckt->DeviceCnt());
}

ASG::ASG()
{
    m_ckt = nullptr;
    m_matrix = nullptr;
    m_bubblingFlag = false;
    m_levellingFlag = false;
    m_buildMatrixFlag = false;

    m_levelPlotter = nullptr;
    m_bubblePlotter = nullptr;

    m_visited = nullptr;
}

ASG::~ASG()
{
    if (m_matrix) delete m_matrix;
    if (m_levelPlotter) delete m_levelPlotter;
    m_wireDesps.clear();
    foreach (DevLevelDescriptor *desp, m_devices)
        delete desp;
    m_devices.clear();

    delete []m_visited;

}

void ASG::SetSchematicData(SchematicData *data)
{
    if (m_matrix) {
        delete m_matrix;
        m_matrix = nullptr;
    }

    m_ckt = data;
    m_bubblingFlag = false;
    m_levellingFlag = false;
    m_buildMatrixFlag = false;

    m_visited = new int[m_ckt->DeviceCnt()];
    memset(m_visited, 0, sizeof(int) * m_ckt->DeviceCnt());
}

void ASG::BuildIncidenceMatrix()
{
    if (m_matrix)  delete m_matrix;
    int size = m_ckt->m_deviceList.size();
    m_matrix = new Matrix(size);

    int row = 0, col = 0;
    foreach (SchematicDevice *device, m_ckt->m_deviceList) {
        row = device->Id();
        col = device->Id();
        m_matrix->SetRowHeadDevice(row, device);
        m_matrix->SetColHeadDevice(col, device);
    }
    
    
    foreach (SchematicDevice *dev, m_ckt->m_deviceList) {
        switch (dev->GetDeviceType()) {
            case SchematicDevice::Resistor:
            case SchematicDevice::Capacitor:
            case SchematicDevice::Inductor:
                InsertRLC(dev);
                break;
            case SchematicDevice::Isrc:
            case SchematicDevice::Vsrc:
                InsertVI(dev);
                break;
            default:;
        }
    }

#ifdef DEBUG
    m_matrix->Print();
    m_matrix->Plot();
#endif

    m_buildMatrixFlag = true;
    m_levellingFlag = false;
    m_bubblingFlag = false;

    GenerateWireDesps();
    // TagDeviceOnBranch();
}

void ASG::Levelling()
{
    m_devices.clear();

    DevLevelDescriptor *dldesp = nullptr;
    int curLevel = 0;
    SchematicDevice *device = nullptr;

    /* level 0 */
    dldesp = new DevLevelDescriptor(curLevel);
    dldesp->AddDevices(m_ckt->m_firstLevelDeviceList);

    /* set first level devices as visited */
    foreach (device, m_ckt->m_firstLevelDeviceList) {
        m_visited[device->Id()] = 1;
    }


#ifdef DEBUG
        dldesp->PrintAllDevices();
#endif

    m_devices.push_back(dldesp);

    curLevel++;

    DeviceList tDeviceList = m_ckt->m_firstLevelDeviceList;
    int totalDeviceNumber = m_ckt->m_deviceList.size();
    int curDeviceNumber = 0;
    curDeviceNumber += tDeviceList.size();

    while (tDeviceList.size() > 0) {
        tDeviceList = FillNextLevelDeviceList(tDeviceList);
        if (tDeviceList.size() == 0)  break;
        dldesp = new DevLevelDescriptor(curLevel);
        dldesp->AddDevices(tDeviceList);

#ifdef DEBUG
        dldesp->PrintAllDevices();
#endif

        m_devices.push_back(dldesp);
        curLevel++;
        curDeviceNumber += tDeviceList.size();
    }


    if (curDeviceNumber != totalDeviceNumber) {
        qDebug() << LINE_INFO << "Levelling failedd." << endl;
        qDebug() << "total device number" << totalDeviceNumber;
        qDebug() << "curr device number" << curDeviceNumber << endl;
        EXIT;
    }

    PrintAllDevices();
    PlotAllDevices();

    m_buildMatrixFlag = true;
    m_levellingFlag = true;
    m_bubblingFlag = false;
}

void ASG::Bubbling()
{

    m_buildMatrixFlag = true;
    m_levellingFlag = true;
    m_bubblingFlag = true;
}

/* RLC current flows from N+ to N- */
void ASG::InsertRLC(SchematicDevice *device)
{
    CktNode *negNode = device->Terminal(Negative);
    assert(negNode);
    if (negNode->IsGnd())  return;

    foreach (SchematicDevice *tDev, negNode->m_devices) {
        if (tDev->GetTerminalType(negNode) == Positive) {
            int row = device->Id();
            int col = tDev->Id();
            if (row == col)  continue;
            m_matrix->InsertElement(row, col, device, tDev, Negative, Positive);
        }
    }
}

/* Vsrc current flows from N+ to N- */
/* Isrc current flows from N+ to N- */
void ASG::InsertVI(SchematicDevice *device)
{
    CktNode *posNode = device->Terminal(Positive);
    assert(posNode);
    if (posNode->IsGnd())  return;

    foreach (SchematicDevice *tDev, posNode->m_devices) {
        if (tDev->GetTerminalType(posNode) == Positive) {
            int row = device->Id();
            int col = tDev->Id();
            if (row == col)  continue;
            m_matrix->InsertElement(row, col, device, tDev, Positive, Positive);
        }
    }
}

DeviceList ASG::FillNextLevelDeviceList(
    const DeviceList curLevelDeviceList ) const
{
    SchematicDevice *device = nullptr;
    DeviceList nextLevelDeviceList;

    int id = 0;
    MatrixElement *element = nullptr;
    foreach (device, curLevelDeviceList) {
        id = device->Id();
        element = m_matrix->RowHead(id).head;
        while (element) {
            device = element->ToDevice();
            if (m_visited[device->Id()]) {
                element = element->NextInRow();
                continue;
            }
            nextLevelDeviceList.push_back(device);
            element = element->NextInRow();
            m_visited[device->Id()] = 1;
        }
    }

    return nextLevelDeviceList;
}

void ASG::PrintAllDevices() const
{
    printf("--------------- Level Device List ---------------\n");

    int totalLevel = m_devices.size();
    SchematicDevice *device = nullptr;

    DeviceList tDeviceList;
    for (int i = 0; i < totalLevel; ++ i) {
        tDeviceList = m_devices.at(i)->AllDevices();
        printf("L%-7d", i);
        
        foreach (device, tDeviceList) {
            printf("%-8s", CString(device->Name()));
        }
        printf("\n");
    }

    printf("-------------------------------------------------\n\n");
}

void ASG::PlotAllDevices()
{
#ifdef TRACE
    qInfo() << LINE_INFO << endl;
#endif

    if (m_devices.size() < 1) {
        qInfo() << LINE_INFO << "Please levelling before plotting level device";
        return;
    }

    if (m_levelPlotter) {
        m_levelPlotter->close();
        m_levelPlotter->Clear();
    }
    else {
        m_levelPlotter = new TablePlotter();
    }

    qInfo() << LINE_INFO << endl;

    /* col count */
    int totalLevel = m_devices.size();

    /* row count */
    int maxDeviceNumberInLevel = -1;
    DeviceList tDevList;
    DevLevelDescriptor *dldesp = nullptr;

    foreach (dldesp, m_devices) {
        tDevList = dldesp->AllDevices();
        if (tDevList.size() > maxDeviceNumberInLevel)
            maxDeviceNumberInLevel = tDevList.size();
    }
    m_levelPlotter->SetTableRowColCount(maxDeviceNumberInLevel, totalLevel);

    /* header */
    QStringList headerText;
    for (int i = 0; i < totalLevel; ++ i) {
        QString tmp = "L" + QString::number(i);
        headerText << tmp;
    }
    m_levelPlotter->SetColHeaderText(headerText);

    /* content */
    for (int i = 0; i < m_devices.size(); ++ i) {
        tDevList = m_devices.at(i)->AllDevices();
        for (int j = 0; j < tDevList.size(); ++ j) {
            m_levelPlotter->AddItem(j, i, tDevList.at(j)->Name());
        }
    }

    m_levelPlotter->Display();
}

void ASG::GenerateWireDesps()
{
    m_wireDesps.clear();

    int fromTerId = 0, toTerId = 0;
    SchematicDevice *startDev = nullptr, *endDev = nullptr;

    int size = m_matrix->Size();
    MatrixElement *element = nullptr;
    for (int i = 0; i < size; ++ i) {
        element = m_matrix->RowHead(i).head;
        while (element) {

            startDev = element->FromDevice();
            endDev = element->ToDevice();
            fromTerId = startDev->TerminalId(element->FromTerminal());
            toTerId = endDev->TerminalId(element->ToTerminal());

            printf("from + to (%d, %d)\n", fromTerId, toTerId);

            WireDescriptor *wd = new WireDescriptor;
            wd->startDev = startDev;
            wd->endDev = endDev;
            /* set as branch */
            if (wd->startDev->GetDeviceType() != SchematicDevice::Capacitor &&
                wd->endDev->GetDeviceType() != SchematicDevice::Capacitor) {
                wd->isBranch = true;
            }
            wd->startTerminal = element->FromTerminal();
            wd->endTerminal = element->ToTerminal();

            m_wireDesps.push_back(wd);
            element = element->NextInRow();

            qInfo() << "wire: " << wd->startDev->Name() << " " << wd->endDev->Name() << endl;
        }
    }
}