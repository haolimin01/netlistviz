#ifndef NETLISTVIZ_ASG_MATRIXELEMENT_H
#define NETLISTVIZ_ASG_MATRIXELEMENT_H

/*
 * @filename : MatrixElement.h
 * @ahthor   : Hao Limin
 * @date:    : 2020.07.30
 * @desp     : MatrixElement for Matrix.
 * @modified : Hao Limin, 2020.09.12
 */

#include "Define/Define.h"

class Device;
class Terminal;

class MatrixElement
{
public:
    MatrixElement(int row, int col, Device *fromDevice, Terminal *fromTerminal,
        Device *toDevice, Terminal *toTerminal);
    ~MatrixElement();

public:
    void  SetRowIndex(int row) { m_rowIndex = row; }
    void  SetColIndex(int col) { m_colIndex = col; }
    int   RowIndex() const { return m_rowIndex; }
    int   ColIndex() const { return m_colIndex; }

    Device*          FromDevice()   const { return m_fromDevice; }
    Terminal*        FromTerminal() const { return m_fromTerminal; }
    Device*          ToDevice()     const { return m_toDevice; }
    Terminal*        ToTerminal()   const { return m_toTerminal; }
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

    Device          *m_fromDevice;
    Terminal        *m_fromTerminal;
    Device          *m_toDevice;
    Terminal        *m_toTerminal;
    
    MatrixElement   *m_nextInRow;
    MatrixElement   *m_nextInCol;
};

#endif // NETLISTVIZ_ASG_MATRIXELEMENT_H