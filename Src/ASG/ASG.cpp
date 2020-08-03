#include "ASG.h"
#include <cassert>
#include <QDebug>
#include "Matrix.h"
#include "Schematic/SchematicData.h"
#include "Schematic/SchematicDevice.h"
#include "Circuit/CktNode.h"

ASG::ASG(SchematicData *data)
{
    assert(data);
    m_ckt = data;

    m_matrix = nullptr;
}

ASG::~ASG()
{
    if (m_matrix) delete m_matrix;
}

void ASG::GeneratePos()
{
    /* 1. BuildIncidenceMatrix */
    BuildIncidenceMatrix();
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
#endif
}

void ASG::Levelling()
{

}

void ASG::Bubbling()
{

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
            qDebug() << "insert (" << row << col << ")"; 
        }
    }
}
