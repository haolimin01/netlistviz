#include "SchematicWire.h"

SchematicWire::SchematicWire(Wire *wire, QGraphicsItem *parent, QGraphicsScene *scene)
    : QGraphicsItem(parent)
{

}

SchematicWire::SchematicWire(SchematicDevice *startDevice, SchematicDevice *endDevice,
    SchematicTerminal *startTer, SchematicTerminal *endTer, QGraphicsItem *parent, QGraphicsScene *scene)
    : QGraphicsItem(parent)
{

}

SchematicWire::~SchematicWire()
{

}

void SchematicWire::Initialize()
{
    
}

QRectF SchematicWire::boundingRect() const
{

}

void SchematicWire::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{

}