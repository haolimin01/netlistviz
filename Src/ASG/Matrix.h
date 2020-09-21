#ifndef NETLIST_ASG_MATRIX_H
#define NETLIST_ASG_MATRIX_H

/*
 * @filename : Matrix.h
 * @author   : Hao Limin
 * @date     : 2020.07.30
 * @desp     : Incidence Matrix. Square. Sparse
 * @modified : Hao Limin, 2020.09.12
 */

#include "Define/Define.h"

class Device;
class Terminal;
class MatrixElement;
class TablePlotter;

struct HeadElement
{
    MatrixElement   *head;
    int              seqElementNum;
    Device      *device;

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

    void   SetRowHeadDevice(int row, Device *device);
    void   SetColHeadDevice(int col, Device *device);

    void   InsertElement(int row, int col, Device *fromDevice,
                Terminal *fromTerminal, Device *toDevice, Terminal *toTerminal);
    
    HeadElement RowHead(int row) const;
    HeadElement ColHead(int col) const;

    void   Print() const;
    void   Plot();

private:
    DISALLOW_COPY_AND_ASSIGN(Matrix);

    void PrintNEmptyElement(int n) const;

    int    m_size;
    long   m_totalElement;

    HeadElement *m_rowHead;
    HeadElement *m_colHead;

    TablePlotter *m_plotter;
};


#endif // NETLIST_ASG_MATRIX_H
