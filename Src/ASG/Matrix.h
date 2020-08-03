#ifndef NETLIST_ASG_MATRIX_H
#define NETLIST_ASG_MATRIX_H

/*
 * @filename : Matrix.h
 * @author   : Hao Limin
 * @date     : 2020.07.30
 * @desp     : Incidence Matrix. Square. Sparse
 */

#include "Define/Define.h"

class MatrixElement;
class SchematicDevice;

struct HeadElement
{
    MatrixElement   *head;
    int              seqElementNum;
    SchematicDevice *device;

    HeadElement() {
        head = nullptr;
        seqElementNum = 0;
        device = nullptr;
    }
    ~HeadElement() {
        /* This elemnt is freed in Matrix */
    }
};

class Matrix
{
public:
    explicit Matrix(int size);
    ~Matrix();

public:
    int    Size() const { return m_size; }
    int    TotalElement() const { return m_totalElement; }

    void   SetRowHeadDevice(int row, SchematicDevice *device);
    void   SetColHeadDevice(int col, SchematicDevice *device);

    void   InsertElement(int row, int col, SchematicDevice *fromDevice,
                SchematicDevice *toDevice);

    void   Print() const;
    void   Plot() const;

private:
    DISALLOW_COPY_AND_ASSIGN(Matrix);

    void PrintNEmptyElement(int n) const;

    int    m_size;
    long   m_totalElement;

    HeadElement *m_rowHead;
    HeadElement *m_colHead;
};


#endif // NETLIST_ASG_MATRIX_H