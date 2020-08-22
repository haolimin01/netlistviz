#ifndef NETLISTVIZ_ASG_MATRIXELEMENT_H
#define NETLISTVIZ_ASG_MATRIXELEMENT_H

/*
 * @filename : MatrixElement.h
 * @ahthor   : Hao Limin
 * @date:    : 2020.07.30
 * @desp     : MatrixElement for Matrix.
 */

#include "Define/Define.h"
#include "Schematic/SchematicDevice.h"

class Matrix;


class MatrixElement
{
public:
    MatrixElement(int row, int col, SchematicDevice *fromDevice,
        SchematicDevice *toDevice, TerminalType fromTer, TerminalType toTer);
    ~MatrixElement();

public:
    void  SetRowIndex(int row) { m_rowIndex = row; }
    void  SetColIndex(int col) { m_colIndex = col; }
    int   RowIndex() const { return m_rowIndex; }
    int   ColIndex() const { return m_colIndex; }

    // bool  Visited() const { return m_visited; }
    // void  SetVisited(bool visited = true) { m_visited = visited; }

    SchematicDevice* FromDevice()   const { return m_fromDevice; }
    TerminalType     FromTerminal() const { return m_fromTerminal; }
    SchematicDevice* ToDevice()     const { return m_toDevice; }
    TerminalType     ToTerminal()   const { return m_toTerminal; }
    MatrixElement*   NextInRow()    const { return m_nextInRow; }
    MatrixElement*   NextInCol()    const { return m_nextInCol; }

    /* for Matrix Insert Element */
    MatrixElement**  NextInRowPtr()    { return &m_nextInRow; }
    MatrixElement**  NextInColPtr()    { return &m_nextInCol; }

    void             SetNextInRow(MatrixElement *element)
                     { m_nextInRow = element; }
    void             SetNextInCol(MatrixElement *element)
                     { m_nextInCol = element; }

private:
    DISALLOW_COPY_AND_ASSIGN(MatrixElement);

    int              m_rowIndex;
    int              m_colIndex;

    SchematicDevice *m_fromDevice;
    TerminalType     m_fromTerminal;
    SchematicDevice *m_toDevice;
    TerminalType     m_toTerminal;
    
    MatrixElement   *m_nextInRow;
    MatrixElement   *m_nextInCol;

    // bool             m_visited;
};

#endif // NETLISTVIZ_ASG_MATRIXELEMENT_H