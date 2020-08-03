#ifndef NETLISTVIZ_ASG_ASG_H
#define NETLISTVIZ_ASG_ASG_H

/*
 * @filename : ASG.h
 * @author   : Hao Limin
 * @date     : 2020.07.31
 * @desp     : Automatic Schematic Generator.
 */

#include <QVector>
#include "Define/Define.h"

class Matrix;
class SchematicData;
class SchematicDevice;
class TablePlotter;

class ASG
{
public:
    typedef QVector<SchematicDevice *>  DeviceList;

public:
    explicit ASG(SchematicData *data);
    ASG();
    ~ASG();

    void SetSchematicData(SchematicData *data);
    void BuildIncidenceMatrix();
    void Levelling();
    void Bubbling();

    bool BuildMatrixFinished() const { return m_buildMatrixFlag; }
    bool LevellingFinished() const { return m_levellingFlag; }
    bool BubblingFinished() const { return m_bubblingFlag; }

private:
    DISALLOW_COPY_AND_ASSIGN(ASG);

    /* return next level device list. */
    DeviceList FillNextLevelDeviceList(const DeviceList) const;

    void InsertRLC(SchematicDevice *device);
    void InsertVI(SchematicDevice *device);

    void PrintLevelDeviceList() const;
    void PlotLevelDeviceList();

    Matrix        *m_matrix;      // incidence matrix
    int            m_matrixSize;  // matrix size
    SchematicData *m_ckt;         // circuit infomation

    /* level : DeviceList */
    QVector<DeviceList> m_levelDeviceList;

    bool           m_buildMatrixFlag;
    bool           m_levellingFlag;
    bool           m_bubblingFlag;

    TablePlotter  *m_levelPlotter;
    TablePlotter  *m_bubblePlotter;
};

#endif // NETLISTVIZ_ASG_ASG_H