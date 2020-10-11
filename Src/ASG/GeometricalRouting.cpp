#include "ASG.h"
#include <QDebug>
#include "Channel.h"
#include "SchematicWire.h"
#include "Schematic/SchematicScene.h"
#include "Schematic/SchematicDot.h"
#include "Wire.h"
#include "Dot.h"
#include "Level.h"

int ASG::GeometricalRouting(SchematicScene *scene)
{
#ifdef TRACE
    qInfo() << LINE_INFO << endl;
#endif

    int error = CreateSchematicWires();
    if (error)
        return ERROR;

    error = RenderSchematicWires(scene);
    if (error)
        return ERROR;

    error = CreateSchematicDots();
    if (error)
        return ERROR;

    error = RenderSchematicDots(scene);
    if (error)
        return ERROR;

    DestroyLogicalData();

    return OKAY;
}

int ASG::CreateSchematicWires()
{
#ifdef TRACE
    qInfo() << LINE_INFO << endl;
#endif

    /* In Channel */
    m_inChannelSWireList.clear();
    Channel *channel = nullptr;
    Wire *wire = nullptr;
    SchematicWire *swire = nullptr;

    foreach (channel, m_channels) {
        foreach (wire, channel->Wires()) {
            swire = CreateSchematicWire(wire);
            swire->SetTrackCount(channel->TrackCount());
            swire->SetHoldColCount(channel->HoldColCount());
            m_inChannelSWireList.push_back(swire);
        }
    }

#ifdef DEBUGx
    printf("---------- Wires in Channel ----------\n");
    foreach (SchematicWire *w, m_inChannelSWireList) {
        w->Print();
    }
    printf("--------------------------------------\n");
#endif

    /* In Level */
    m_inLevelSWireList.clear();
    Level *level = nullptr;
    foreach (level, m_levels) {
        foreach (wire, level->Wires()) {
            swire = CreateSchematicWire(wire);
            m_inLevelSWireList.push_back(swire);
        }
    }

#ifdef DEBUGx
    printf("---------- Wires in Level ----------\n");
    foreach (SchematicWire *w, m_inLevelSWireList)
        w->Print();
    printf("------------------------------------\n");
#endif

    return OKAY;
}

SchematicWire* ASG::CreateSchematicWire(Wire *wire) const
{
    Q_ASSERT(wire);

    SchematicWire *swire = new SchematicWire(wire->FromSDevice(), wire->ToSDevice(),
            wire->FromSTerminal(), wire->ToSTerminal());
    swire->SetTrack(wire->Track());
    wire->SetSchematicWire(swire);

    return swire;
}

int ASG::RenderSchematicWires(SchematicScene *scene)
{
    int error = scene->RenderSchematicWiresInChannel(m_inChannelSWireList);
    if (error)
        return ERROR;

    error = scene->RenderSchematicWiresInLevel(m_inLevelSWireList);
    return error;
}

int ASG::CreateSchematicDots()
{
#ifdef TRACE
    qInfo() << LINE_INFO << endl;
#endif

    m_sdotList.clear();

    SchematicDot *sdot = nullptr;
    Dot *dot = nullptr;
    Channel *ch = nullptr;

    foreach (ch, m_channels) {
        foreach (dot, ch->Dots()) {
            sdot = CreateSchematicDot(dot);
            sdot->SetTrackCount(ch->TrackCount());
            sdot->SetHoldColCount(ch->HoldColCount());
            m_sdotList.push_back(sdot);
        }
    }

#ifdef DEBUGx
    printf("--------------- Schematic Dots --------------\n");

    foreach (SchematicDot *sdot, m_sdotList)
        sdot->Print();

    printf("---------------------------------------------\n");
#endif

    return OKAY;
}

SchematicDot* ASG::CreateSchematicDot(Dot *dot) const
{
    SchematicDot *sdot = new SchematicDot();
    sdot->SetTerminal(dot->STerminal());
    sdot->SetTrack(dot->Track());
    sdot->SetGeometricalCol(dot->GeometricalCol());

    foreach (Wire *wire, dot->Wires()) {
        sdot->AddWire(wire->GetSchematicWire());
    }

    return sdot;
}

int ASG::RenderSchematicDots(SchematicScene *scene)
{
    return scene->RenderSchematicDots(m_sdotList);
}
