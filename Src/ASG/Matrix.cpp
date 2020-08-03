#include "Matrix.h"
#include "MatrixElement.h"
#include "Schematic/SchematicDevice.h"
#include <QDebug>
#include "Utilities/MyString.h"


Matrix::Matrix(int size)
{
    m_size = size;
    m_totalElement = 0;

    m_rowHead = new HeadElement[size];
    m_colHead = new HeadElement[size];
}

Matrix::~Matrix()
{
    MatrixElement *front, *rear;
    for (int i = 0; i < m_size; ++ i) {
        front = m_rowHead[i].head;
        while (front) {
            rear = front->NextInRow();
            delete front;
            front = rear;
        }
    }

    delete []m_rowHead;
    delete []m_colHead;
}

void Matrix::InsertElement(int row, int col,
                SchematicDevice *fromDevice, SchematicDevice *toDevice)
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
    MatrixElement *newElement = new MatrixElement(row, col, fromDevice, toDevice);

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

void Matrix::SetRowHeadDevice(int row, SchematicDevice *device)
{
    assert(row >= 0 && row < m_size);
    m_rowHead[row].device = device;
}

void Matrix::SetColHeadDevice(int col, SchematicDevice *device)
{
    assert(col >= 0 && col < m_size);
    m_colHead[col].device = device;
}

void Matrix::Print() const
{
    printf("---------------- Incidence Matrix ---------------\n");
    /* Print Row Head String */
    printf("%-6s", " ");

    SchematicDevice *device = nullptr;
    for (int i = 0; i < m_size; ++ i) {
        device = m_colHead[i].device;
        assert(device->Id() == i);
        printf("%-6s", CString(device->Name()));
    }
    printf("\n");

    int printCounter = -1;
    int id = 0;
    MatrixElement *element = nullptr;
    for (int i = 0; i < m_size; ++ i) {
        device = m_rowHead[i].device;
        assert(device->Id() == i);
        printf("%-6s", CString(device->Name()));

        printCounter = -1;
        element = m_rowHead[i].head;
        while (element) {
            id = element->ToDevice()->Id();
            PrintNEmptyElement(id - printCounter - 1);
            printf("%-6s", "1");
            element = element->NextInRow();
            printCounter = id;
        }
        PrintNEmptyElement(m_size - printCounter - 1);
        printf("\n");
    }

    printf("---------------- Incidence Matrix ---------------\n\n");
}

void Matrix::PrintNEmptyElement(int n) const
{
    if (n <= 0)  return;
    for (int i = 0; i < n; ++ i) {
        printf("%-6s", "0");
    }
}