#ifndef NETLISTVIZ_ASG_ASG_H
#define NETLISTVIZ_ASG_ASG_H

/*
 * @filename : ASG.h
 * @author   : Hao Limin
 * @date     : 2020.09.12
 * @desp     : Automatic Schematic Generator.
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
    int  LogicalPlacement();
    int  LogicalRouting();
    int  GeometricalPlacement(SchematicScene *scene);
    int  GeometricalRouting(SchematicScene *scene);

    /* Destroy Logical Part Data */
    void DestroyLogicalData();
    bool DataDestroyed() const { return m_logDataDestroyed; }

private:
    DISALLOW_COPY_AND_ASSIGN(ASG);

    /* ---------- Logical Placement ---------- */
    int    BuildIncidenceMatrix(); // undirected graph
    int    CalLogicalCol();        // calculate logical column by depth
    int    CalLogicalRow();        // calculate logical row by bubble sort
    int    InsertBasicDevice(Device *device);
    Level* CreateNextLevel(Level *prevLevel) const;
    int    BubbleSort();
    int    BubbleSortIgnoreNoCap();
    int    BubbleSortIgnoreGCap();
    int    BubbleSortIgnoreGCCap();
    /* --------------------------------------- */

    /* ----------- Logical Routing ----------- */
    int  CreateChannels();       // to m_channels
    int  AssignTrackNumber();    // assign track number to vertical line segment
    /* --------------------------------------- */

    /* -------- Geometrical Placement -------- */
    int  DecideDeviceWhetherToReverse();
    int  DecideDeviceWhetherToReverseIgnoreNoCap();
    int  DecideDeviceWhetherToReverseIgnoreGCap();
    int  DecideDeviceWhetherToReverseIgnoreGCCap();
    int  LinkDeviceForGCCap();     // Ground and Coupled Cap
    int  CreateSchematicDevices(); // create schematicdevices and schematicterminals
    int  RenderSchematicDevices(SchematicScene *scene); // render devices to scene
    /* --------------------------------------- */

    /* --------- Geometrical Routing --------- */
    int  CreateSchematicWires();   // create schematicwires
    int  RenderSchematicWires(SchematicScene *scene);   // render wires to scene
    /* --------------------------------------- */


    /* Print and Plot */
    void PlotLevels(const QString &title);


    /* ASG members */
    CircuitGraph      *m_ckt;
    Matrix            *m_matrix;
    int               *m_visited;

    QVector<Level*>    m_levels;
    QVector<Channel*>  m_channels;
    TablePlotter      *m_levelsPlotter;

    SDeviceList        m_sdeviceList;
    SWireList          m_swireList;

    bool               m_logDataDestroyed;
    IgnoreCap          m_ignoreCap;
};

#endif // NETLISTVIZ_ASG_ASG_H
