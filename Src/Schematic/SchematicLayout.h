#ifndef NETLISTVIZ_SCHEMATIC_SCHEMATIC_LAYOUT_H
#define NETLISTVIZ_SCHEMATIC_SCHEMATIC_LAYOUT_H

#include "Define/Define.h"
class SchematicData;


class SchematicLayout
{
public:
    enum LayoutMethod { Square = 0 };

public:
    SchematicLayout();
    ~SchematicLayout();

    void GeneratePos(SchematicData *, LayoutMethod);

private:
    void SquareLayout();

    SchematicData *m_data;
};


#endif // NETLISTVIZ_SCHEMATIC_SCHEMATIC_LAYOUT_H