#ifndef NETLISTVIZ_ASG_MATRIXELEMENT_H
#define NETLISTVIZ_ASG_MATRIXELEMENT_H

/*
 * @filename : MatrixElement.h
 * @ahthor   : Hao Limin
 * @date:    : 2020.07.30
 * @desp     : MatrixElement for Matrix.
 */

#include "Define/Define.h"

class SchematicDevice;
class Matrix;


class MatrixElement
{
public:
    MatrixElement(int row, int col, SchematicDevice *device);
    ~MatrixElement();

public:
    void  SetRowIndex(int row) { m_rowIndex = row; }
    void  SetColIndex(int col) { m_colIndex = col; }
    int   RowIndex() const { return m_rowIndex; }
    int   ColIndex() const { return m_colIndex; }

    SchematicDevice* Device() const { return m_device; }
    MatrixElement*   NextInRow() const { return m_nextInRow; }
    /* for Matrix Insert Element */
    MatrixElement**  NextInRowPtr()    { return &m_nextInRow; }
    MatrixElement*   NextInCol() const { return m_nextInCol; }
    /* for Matrix Insert Element */
    MatrixElement**  NextInColPtr()    { return &m_nextInCol; }

    void             SetNextInRow(MatrixElement *element)
                     { m_nextInRow = element; }
    void             SetNextInCol(MatrixElement *element)
                     { m_nextInCol = element; }

private:
    DISALLOW_COPY_AND_ASSIGN(MatrixElement);

    int              m_rowIndex;
    int              m_colIndex;
    SchematicDevice *m_device;
    MatrixElement   *m_nextInRow;
    MatrixElement   *m_nextInCol;
};

#endif // NETLISTVIZ_ASG_MATRIXELEMENT_H