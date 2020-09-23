#include "ASG.h"
#include <QDebug>
#include "Channel.h"
#include "SchematicWire.h"
#include "Schematic/SchematicScene.h"

int ASG::GeometricalRouting(SchematicScene *scene)
{
#ifdef TRACE
    qInfo() << LINE_INFO << endl;
#endif

    int error = CreateSchematicWires();
    if (error)
        return ERROR;

    DestroyLogicalData();

    error = RenderSchematicWires(scene);
    if (error)
        return ERROR;

    return OKAY;
}

int ASG::CreateSchematicWires()
{
#ifdef TRACE
    qInfo() << LINE_INFO << endl;
#endif

    m_swireList.clear();

    Channel *channel = nullptr;
    Wire *wire = nullptr;
    SchematicWire *swire = nullptr;

    foreach (channel, m_channels) {
        foreach (wire, channel->Wires()) {
            swire = new SchematicWire(wire, nullptr);
            swire->SetThisChannelTrackCount(channel->TrackCount());
            m_swireList.push_back(swire);
        }
    }

#ifdef DEBUG
    foreach (SchematicWire *w, m_swireList) {
        w->Print();
    }
#endif    

    return OKAY;
}

int ASG::RenderSchematicWires(SchematicScene *scene)
{
    return scene->RenderSchematicWires(m_swireList);
}