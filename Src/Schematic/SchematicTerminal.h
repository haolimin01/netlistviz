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

class Terminal;
class Node;
class SchematicDevice;

class SchematicTerminal
{
public:
    /* ASG entrance */
    explicit SchematicTerminal(Terminal *terminal);

    /* CreateTerminalsBySelf (in SchematicDevice) entrance*/
    SchematicTerminal(TerminalType type, SchematicDevice *device);
    ~SchematicTerminal();

    void         SetDevice(SchematicDevice *device);
    TerminalType GetTerminalType() const { return m_type; }
    Node*        GetNode() const { return m_node; }
    void         SetRect(const QRectF &rect) { m_rect = rect; }
    QRectF       Rect() const { return m_rect; }
    bool         ConnectToGnd() const;
    QPointF      ScenePos() const;

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
};

#endif // NETLISTVIZ_SCHEMATIC_SCHEMATIC_TERMINAL_H
