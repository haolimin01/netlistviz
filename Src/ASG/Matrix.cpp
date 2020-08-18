#include "Matrix.h"
#include <QDebug>
#include "MatrixElement.h"
#include "Schematic/SchematicDevice.h"
#include "Utilities/MyString.h"
#include "TablePlotter.h"


Matrix::Matrix(int size)
{
    m_size = size;
    m_totalElement = 0;

    m_rowHead = new HeadElement[size];
    m_colHead = new HeadElement[size];

    m_plotter = nullptr;
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

    if (m_plotter)  delete m_plotter;
}

void Matrix::InsertElement(int row, int col, SchematicDevice *fromDevice,
    SchematicDevice *toDevice, TerminalType fromTer, TerminalType toTer)
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
    MatrixElement *newElement = new MatrixElement(row, col, fromDevice, toDevice, fromTer, toTer);

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

HeadElement Matrix::RowHead(int row) const
{
    Q_ASSERT(row >=0 && row < m_size);
    return m_rowHead[row];
}

HeadElement Matrix::ColHead(int col) const
{
    Q_ASSERT(col >= 0 && col < m_size);
    return m_colHead[col];
}

void Matrix::Print() const
{
    printf("---------------- Incidence Matrix ---------------\n");
    /* Print Row Head String */
    printf("%-8s", " ");

    SchematicDevice *device = nullptr;
    for (int i = 0; i < m_size; ++ i) {
        device = m_colHead[i].device;
        assert(device->Id() == i);
        printf("%-8s", CString(device->Name()));
    }
    printf("\n");

    int printCounter = -1;
    int id = 0;
    MatrixElement *element = nullptr;
    for (int i = 0; i < m_size; ++ i) {
        device = m_rowHead[i].device;
        assert(device->Id() == i);
        printf("%-8s", CString(device->Name()));

        printCounter = -1;
        element = m_rowHead[i].head;
        while (element) {
            id = element->ToDevice()->Id();
            PrintNEmptyElement(id - printCounter - 1);
            printf("%-8s", "1");
            element = element->NextInRow();
            printCounter = id;
        }
        PrintNEmptyElement(m_size - printCounter - 1);
        printf("\n");
    }

    printf("-------------------------------------------------\n\n");
}

void Matrix::PrintNEmptyElement(int n) const
{
    if (n <= 0)  return;
    for (int i = 0; i < n; ++ i) {
        printf("%-8s", "0");
    }
}

void Matrix::Plot()
{
    if (m_plotter) {
        m_plotter->close();
        m_plotter->Clear();
    } else {
        m_plotter = new TablePlotter;
    }

    m_plotter->SetTableRowColCount(m_size, m_size);

    QStringList headerText;
    SchematicDevice *device = nullptr;

    /* row header text */
    for (int i = 0; i < m_size; ++ i) {
        device = m_rowHead[i].device;
        Q_ASSERT(device);
        headerText << device->Name();
    }
    m_plotter->SetRowHeaderText(headerText);

    headerText.clear();
    /* col header text */
    for (int i = 0; i < m_size; ++ i) {
        device = m_colHead[i].device;
        Q_ASSERT(device);
        headerText << device->Name();
    }
    m_plotter->SetColHeaderText(headerText);

    /* table content */
    MatrixElement *element = nullptr;
    int row = 0, col = 0;
    for (int i = 0; i < m_size; ++ i) {
        element = m_rowHead[i].head;
        while (element) {
            row = element->RowIndex();
            col = element->ColIndex();
            m_plotter->AddItem(row, col);
            element = element->NextInRow();
        }
    }
    m_plotter->Display();
}

void Matrix::SetAllVisited(bool visited)
{
    MatrixElement *element = nullptr;

    for (int i = 0; i < m_size; ++ i) {
        element = m_rowHead[i].head;

        while (element) {
            element->SetVisited(visited);
            element = element->NextInRow();
        }
    }
}
