#ifndef NETLISTVIZ_ASG_ASG_H
#define NETLISTVIZ_ASG_ASG_H

/*
 * @filename : ASG.h
 * @author   : Hao Limin
 * @date     : 2020.09.12
 * @desp     : Automatic Schematic Generator.
 * @modified : Hao Limin, 2020.09.24
 * @modified : Hao Limin, 2020.09.26
 */

#include "Define/Define.h"
#include "Define/TypeDefine.h"

class Matrix;
class TablePlotter;
class CircuitGraph;
class Level;
class Wire;
class Channel;
class Dot;
class SchematicDevice;
class SchematicScene;
class SchematicWire;
class SchematicDot;


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
    Level*      CreateNextLevel(Level *preLevel) const;
    int         ClassifyConnectDeviceByLevel();
    int         EstimateLogicalRowGap();
    int         DetermineFirstLevelLogicalRow();
    int         ForwardPropagateLogicalRow();   // level0 -> level1 -> level2 -> ... -> leveln
    int         DecideDeviceOrientation();
    int         DecideDeviceWhetherToReverse();
    /* --------------------------------------- */


    /* ----------- Logical Routing ----------- */
    int  CreateChannels();
    int  AssignTrackNumber();
    int  CreateDots();
    /* --------------------------------------- */


    /* -------- Geometrical Placement -------- */
    int                CalGeometricalCol();
    int                CalGeometricalRow();
    int                CreateSchematicDevices();
    int                RenderSchematicDevices(SchematicScene *scene);
    SchematicDevice*   CreateSchematicDevice(Device *dev) const;
    SchematicTerminal* CreateSchematicTerminal(Terminal *ter) const;
    /* --------------------------------------- */


    /* --------- Geometrical Routing --------- */
    int            CreateSchematicWires();
    int            RenderSchematicWires(SchematicScene *scene);
    SchematicWire* CreateSchematicWire(Wire *wire) const;
    int            CreateSchematicDots();
    SchematicDot*  CreateSchematicDot(Dot *dot) const;
    int            RenderSchematicDots(SchematicScene *scene);
    /* --------------------------------------- */


    /* Print and Plot */
    void PlotLevels(const QString &title);


    /* ASG members */
    CircuitGraph      *m_ckt;
    Matrix            *m_matrix;
    int               *m_visited;

    LevelList          m_levels;
    ChannelList        m_channels;
    TablePlotter      *m_levelPlotter;

    SDeviceList        m_sdeviceList;
    SWireList          m_inChannelSWireList;
    SWireList          m_inLevelSWireList;
    SDotList           m_sdotList;

    bool               m_logDataDestroyed;
    IgnoreCap          m_ignoreCap;
};

#endif // NETLISTVIZ_ASG_ASG_H
