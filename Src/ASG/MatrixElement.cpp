#include "MatrixElement.h"
#include "Schematic/SchematicDevice.h"

MatrixElement::MatrixElement(int row, int col, SchematicDevice *fromDevice,
    SchematicDevice *toDevice, TerminalType fromTer, TerminalType toTer)
{
    m_rowIndex = row;
    m_colIndex = col;
    m_fromDevice = fromDevice;
    m_toDevice = toDevice;
    m_fromTerminal = fromTer;
    m_toTerminal = toTer;

    m_visited = false;
}

MatrixElement::~MatrixElement()
{

}