#ifndef NETLISTVIZ_ASG_ASG_H
#define NETLISTVIZ_ASG_ASG_H

/*
 * @filename : ASG.h
 * @author   : Hao Limin
 * @date     : 2020.09.12
 * @desp     : Automatic Schematic Generator.
 * @modified : Hao Limin, 2020.09.24
 */

#include "Define/Define.h"
#include "Define/TypeDefine.h"

class Matrix;
class TablePlotter;
class CircuitGraph;
class Level;
class Wire;
class Channel;
class SchematicDevice;
class SchematicScene;
class SchematicWire;


class ASG
{
public:
    explicit ASG(CircuitGraph *ckt);
    ASG();
    ~ASG();

    void SetCircuitgraph(CircuitGraph *ckt);
    void SetIgnoreCapType(IgnoreCap type) { m_ignoreCap = type; }
    int  Prepare();
    int  LogicalPlacement();
    int  LogicalRouting();
    int  GeometricalPlacement(SchematicScene *scene);
    int  GeometricalRouting(SchematicScene *scene);

    /* Destroy Logical Part Data */
    void DestroyLogicalData();
    bool DataDestroyed() const { return m_logDataDestroyed; }

private:
    DISALLOW_COPY_AND_ASSIGN(ASG);

    /* --------------- Prepare --------------- */
    int    LinkDevice();
    /* --------------------------------------- */


    /* ---------- Logical Placement ---------- */
    int         BuildIncidenceMatrix();
    int         CalLogicalCol();
    int         CalLogicalRow();
    int         InsertBasicDevice(Device *device);
    HyperLevel* CreateNextHyperLevel(HyperLevel *preHyperLevel) const;
    int         ClassifyConnectDeviceByHyperLevel();
    int         BubbleSort();
    int         BubbleSortIgnoreNoCap();
    int         BubbleSortIgnoreGCap();
    int         BubbleSortIgnoreGCCap();
    int         AdjustHyperLevelInside();
    /* --------------------------------------- */


    /* ----------- Logical Routing ----------- */
    int  CreateChannels();
    int  AssignTrackNumber();
    /* --------------------------------------- */


    /* -------- Geometrical Placement -------- */
    int  DecideDeviceWhetherToReverse();
    int  DecideDeviceWhetherToReverseIgnoreNoCap();
    int  DecideDeviceWhetherToReverseIgnoreGCap();
    int  DecideDeviceWhetherToReverseIgnoreGCCap();
    int  CalGeometricalCol();
    int  CalGeometricalRow();
    int  LinkDeviceForGCCap();
    int  CreateSchematicDevices();
    int  RenderSchematicDevices(SchematicScene *scene);
    /* --------------------------------------- */


    /* --------- Geometrical Routing --------- */
    int  CreateSchematicWires();
    int  RenderSchematicWires(SchematicScene *scene);
    /* --------------------------------------- */


    /* Print and Plot */
    // void PlotLevels(const QString &title);
    void PlotHyperLevels(const QString &title);


    /* ASG members */
    CircuitGraph      *m_ckt;
    Matrix            *m_matrix;
    int               *m_visited;

    QVector<Level*>    m_levels;
    HyperLevelList     m_hyperLevels;
    QVector<Channel*>  m_channels;
    // TablePlotter      *m_levelsPlotter;
    TablePlotter      *m_levelPlotter;

    SDeviceList        m_sdeviceList;
    SWireList          m_swireList;

    bool               m_logDataDestroyed;
    IgnoreCap          m_ignoreCap;
};

#endif // NETLISTVIZ_ASG_ASG_H
