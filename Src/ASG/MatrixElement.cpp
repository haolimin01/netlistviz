#include "MatrixElement.h"

MatrixElement::MatrixElement(int row, int col, Device *fromDevice, Terminal *fromTerminal,
    Device *toDevice, Terminal* toTerminal)
{
    m_rowIndex = row;
    m_colIndex = col;
    m_fromDevice = fromDevice;
    m_toDevice = toDevice;
    m_fromTerminal = fromTerminal;
    m_toTerminal = toTerminal;
}

MatrixElement::~MatrixElement()
{

}