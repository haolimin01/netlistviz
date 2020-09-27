#ifndef NETLISTVIZ_SCHEMATIC_SCHEMATIC_TERMINAL_H
#define NETLISTVIZ_SCHEMATIC_SCHEMATIC_TERMINAL_H

/*
 * @filename : SchematicTerminal.h
 * @date     : 2020.09.15
 * @author   : Hao Limin
 * @email    : haolimin01@sjtu.edu.cn
 * @desp     : SchematicDevice's Terminal.
 *             NO need to render on SchematicScene.
 */

#include "Define/Define.h"
#include "Define/TypeDefine.h"
#include <QRectF>

class Node;
class SchematicDevice;
class SchematicWire;

class SchematicTerminal
{
public:
    /* ASG entrance */
    SchematicTerminal();

    /* CreateTerminalsBySelf (in SchematicDevice) entrance*/
    SchematicTerminal(TerminalType type, SchematicDevice *device);
    ~SchematicTerminal();

    void         SetDevice(SchematicDevice *device);
    void         SetTerminalType(TerminalType type) { m_type = type; }
    TerminalType GetTerminalType() const { return m_type; }
    void         SetNode(Node *node) { m_node = node; }
    Node*        GetNode() const { return m_node; }
    void         SetId(int id) { m_id = id; }
    int          Id() const { return m_id; }
    void         SetRect(const QRectF &rect) { m_rect = rect; }
    QRectF       Rect() const { return m_rect; }
    bool         ConnectToGnd() const;
    QPointF      ScenePos() const;
    void         AddWire(SchematicWire *wire);
    void         RemoveWire(SchematicWire *wire);
    SWireList    Wires() const { return m_wires; }

    /* Print */
    QString      PrintString() const;

private:
    DISALLOW_COPY_AND_ASSIGN(SchematicTerminal);

    /* Copy from Terminal */
    Node              *m_node;
    int                m_id;
    TerminalType       m_type;    
    SchematicDevice   *m_device;

    QRectF             m_rect;
    SWireList          m_wires;
};

#endif // NETLISTVIZ_SCHEMATIC_SCHEMATIC_TERMINAL_H
