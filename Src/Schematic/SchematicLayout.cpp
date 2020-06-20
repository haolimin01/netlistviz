#include "SchematicLayout.h"
#include <QtCore/qmath.h>
#include "Define/Define.h"
#include "SchematicData.h"
#include "SchematicNode.h"


SchematicLayout::SchematicLayout()
{
    m_data = nullptr;
}


SchematicLayout::~SchematicLayout()
{

}


void SchematicLayout::GeneratePos(SchematicData *data, LayoutMethod method)
{
    m_data = data;

    switch (method) {
        case Square: SquareLayout(); break;
        default:;
    }
}


void SchematicLayout::SquareLayout()
{
    int startX = StartX, startY = StartY;
    int width = Width, height = Height;

/*
 *  o o o o o
 *  o o o o o
 *  o o o o o
 *  o o o o o
 *  o o o o o
 * 
 */
    int nodeNumber = m_data->m_nodeList.size();
    int edgeNodeNumber = qCeil(qSqrt(nodeNumber)); 

    int wPerLen = qFloor(width / edgeNodeNumber);
    int hPerLen = qFloor(height / edgeNodeNumber);

    double x = 0, y = 0;
    SchematicNode *node = nullptr;

    for (int i = 0; i < nodeNumber; ++ i) {
        x = (i % edgeNodeNumber) * wPerLen;
        y = qFloor(i / edgeNodeNumber) * hPerLen;
        node = m_data->m_nodeList.at(i);
        node->setPos(startX + x, startY + y);
    }
}
