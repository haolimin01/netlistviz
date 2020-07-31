#include "MatrixElement.h"
#include "Schematic/SchematicDevice.h"

MatrixElement::MatrixElement(int row, int col, SchematicDevice *device)
{
    m_rowIndex = row;
    m_colIndex = col;
    m_device = device;
}


MatrixElement::~MatrixElement()
{

}