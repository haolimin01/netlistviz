#include "ASG.h"
#include <cassert>
#include <QDebug>
#include "Matrix.h"
#include "Schematic/SchematicData.h"
#include "Schematic/SchematicDevice.h"
#include "Circuit/CktNode.h"
#include "MatrixElement.h"
#include "Utilities/MyString.h"
#include "TablePlotter.h"


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
}

ASG::ASG()
{
    m_ckt = nullptr;
    m_matrix = nullptr;
    m_bubblingFlag = false;
    m_levellingFlag = false;
    m_buildMatrixFlag = false;
}

ASG::~ASG()
{
    if (m_matrix) delete m_matrix;
    if (m_levelPlotter) delete m_levelPlotter;
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
}

void ASG::Levelling()
{
    m_levelDeviceList.clear();
    m_matrix->SetAllVisited(false);

    /* level 0 */
    m_levelDeviceList.push_back(m_ckt->m_firstLevelDeviceList);

    ASG::DeviceList tDeviceList = m_ckt->m_firstLevelDeviceList;

    int totalDeviceNumber = m_ckt->m_deviceList.size();
    int curDeviceNumber = 0;
    curDeviceNumber += tDeviceList.size();

    while (tDeviceList.size() > 0) {
        tDeviceList = FillNextLevelDeviceList(tDeviceList);
        if (tDeviceList.size() == 0)  break;
        m_levelDeviceList.push_back(tDeviceList);
        curDeviceNumber += tDeviceList.size();
    }

    if (curDeviceNumber != totalDeviceNumber) {
        qDebug() << LINE_INFO << "Levelling failedd.";
        EXIT;
    }

    PrintLevelDeviceList();
    PlotLevelDeviceList();

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
    CktNode *negNode = device->Node(SchematicDevice::Negative);
    assert(negNode);
    if (negNode->IsGnd())  return;

    foreach (SchematicDevice *tDev, negNode->m_devices) {
        if (tDev->GetNodeType(negNode) == SchematicDevice::Positive) {
            int row = device->Id();
            int col = tDev->Id();
            if (row == col)  continue;
            m_matrix->InsertElement(row, col, device, tDev);
        }
    }
}

/* Vsrc current flows from N+ to N- */
/* Isrc current flows from N+ to N- */
void ASG::InsertVI(SchematicDevice *device)
{
    CktNode *posNode = device->Node(SchematicDevice::Positive);
    assert(posNode);
    if (posNode->IsGnd())  return;

    foreach (SchematicDevice *tDev, posNode->m_devices) {
        if (tDev->GetNodeType(posNode) == SchematicDevice::Positive) {
            int row = device->Id();
            int col = tDev->Id();
            if (row == col)  continue;
            m_matrix->InsertElement(row, col, device, tDev);
        }
    }
}

ASG::DeviceList ASG::FillNextLevelDeviceList(
    const DeviceList curLevelDeviceList ) const
{
    SchematicDevice *device = nullptr;
    ASG::DeviceList nextLevelDeviceList;

    int id = 0;
    MatrixElement *element = nullptr;
    foreach (device, curLevelDeviceList) {
        id = device->Id();
        element = m_matrix->RowHead(id).head;
        while (element) {
            if (element->Visited()) {
                element = element->NextInRow();
                continue;
            }
            device = element->ToDevice();
            nextLevelDeviceList.push_back(device);
            element->SetVisited();
            element = element->NextInRow();
        }
    }

    return nextLevelDeviceList;
}

void ASG::PrintLevelDeviceList() const
{
    printf("--------------- Level Device List ---------------\n");

    int totalLevel = m_levelDeviceList.size();
    SchematicDevice *device = nullptr;

    ASG::DeviceList tDeviceList;
    for (int i = 0; i < totalLevel; ++ i) {
        tDeviceList = m_levelDeviceList.at(i);
        printf("L%-7d", i);
        
        foreach (device, tDeviceList) {
            printf("%-8s", CString(device->Name()));
        }
        printf("\n");
    }

    printf("-------------------------------------------------\n\n");
}

void ASG::PlotLevelDeviceList()
{
#ifdef TRACE
    qInfo() << LINE_INFO << endl;
#endif

    if (m_levelDeviceList.size() < 1) {
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
    int totalLevel = m_levelDeviceList.size();

    /* row count */
    int maxDeviceNumberInLevel = -1;
    ASG::DeviceList tDevList;

    foreach (tDevList, m_levelDeviceList) {
        if (tDevList.size() > maxDeviceNumberInLevel)
            maxDeviceNumberInLevel = tDevList.size();
    }
    m_levelPlotter->SetTableRowColCount(maxDeviceNumberInLevel, totalLevel);

    /* header */
    QStringList headerText;
    for (int i = 0; i < totalLevel; ++ i) {
        QString tmp = "Level" + QString::number(i);
        headerText << tmp;
    }
    m_levelPlotter->SetColHeaderText(headerText);

    /* content */
    for (int i = 0; i < m_levelDeviceList.size(); ++ i) {
        tDevList = m_levelDeviceList.at(i);
        for (int j = 0; j < tDevList.size(); ++ j) {
            m_levelPlotter->AddItem(j, i, tDevList.at(j)->Name());
        }
    }

    m_levelPlotter->Display();
}