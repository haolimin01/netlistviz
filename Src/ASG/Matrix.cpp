#include "Matrix.h"
#include "MatrixElement.h"
#include <QDebug>

Matrix::Matrix(int size)
{
    m_size = size;
    m_totalElement = 0;

    m_rowHead = new HeadElement[size];
    m_colHead = new HeadElement[size];
}

void Matrix::InsertElement(int row, int col, SchematicDevice *device)
{
    if ((row >= m_size) || (col >= m_size) || (row < 0) || (col < 0)) {
#ifdef TRACE
        qInfo() << LINE_INFO << endl;
#endif
        qDebug() << QObject::tr("[ERROR] : Matrix out of bound.") << endl;
        return;
    }

    MatrixElement *rowPtr = m_rowHead[row].head;

    /* If exists, return */
    while (rowPtr) {
        if (rowPtr->ColIndex() == col) {
            return;
        }
        rowPtr = rowPtr->NextInRow();
    }

    /* Create and insert it */
    MatrixElement *newElement = new MatrixElement(row, col, device);

    /* Insert element in row */
    MatrixElement **rowPtrPtr = &(m_rowHead[row].head);
    while (*rowPtrPtr) {
        if ((*rowPtrPtr)->ColIndex() > col)  break;
        rowPtrPtr = (*rowPtrPtr)->NextInRowPtr();
    }
    newElement->SetNextInRow(*rowPtrPtr);
    *rowPtrPtr = newElement;

    m_rowHead[row].seqElementNum++;

    /* Insert element in col */
    MatrixElement **colPtrPtr = &(m_colHead[col].head);
    while (*colPtrPtr) {
        if ((*colPtrPtr)->RowIndex() > row)  break;
        colPtrPtr = (*colPtrPtr)->NextInColPtr();
    }
    newElement->SetNextInCol(*colPtrPtr);
    *colPtrPtr = newElement;

    m_colHead[col].seqElementNum++;
}

void Matrix::Print() const
{

}