#ifndef NETLISTVIZ_ASG_ASG_H
#define NETLISTVIZ_ASG_ASG_H

/*
 * @filename : ASG.h
 * @author   : Hao Limin
 * @date     : 2020.07.31
 * @desp     : Automatic Schematic Generator.
 */

#include "Define/Define.h"

class Matrix;
class SchematicData;
class SchematicDevice;

class ASG
{
public:
    explicit ASG(SchematicData *data);
    ~ASG();

    void GeneratePos();

private:
    DISALLOW_COPY_AND_ASSIGN(ASG);

    void BuildIncidenceMatrix();
    void Levelling();
    void Bubbling();

    void InsertRLC(SchematicDevice *device);
    void InsertVI(SchematicDevice *device);

    Matrix        *m_matrix;      // incidence matrix
    int            m_matrixSize;  // matrix size
    SchematicData *m_ckt;         // circuit infomation
};

#endif // NETLISTVIZ_ASG_ASG_H