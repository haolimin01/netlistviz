#ifndef NETLISTVIZ_CIRCUIT_TERMINAL_H
#define NETLISTVIZ_CIRCUIT_TERMINAL_H

/*
 * @filename : Terminal.h
 * @author   : Hao Limin
 * @date     : 2020.09.11
 * @email    : haolimin01@sjtu.edu.cn
 * @desp     : Device's Terminal, contains logical and geometrical coordinates.
 *           : Some terminals are related to one Node.
 */

#include "Define/Define.h"
#include "Define/TypeDefine.h"

class Node;
class Device;
class Wire;
class SchematicTerminal;

class Terminal
{
public:
    explicit Terminal(Node *node);
    ~Terminal();

    void         SetId(int id)   { m_id = id; }
    int          Id() const      { return m_id; }
    int          NodeId() const;
    bool         NodeIsGnd() const;
    Node*        GetNode() const { return m_node; }
    void         SetDevice(Device *device) { m_device = device; }
    Device*      GetDevice() const { return m_device; }
    void         SetTerminalType(TerminalType type) { m_type = type; }
    TerminalType GetTerminalType() const { return m_type; }
    
    /* For creating SchematicWire */
    void               SetSchematicTerminal(SchematicTerminal *sTerminal)
                        { m_sTerminal = sTerminal; }
    SchematicTerminal* GetSchematicTerminal() const { return m_sTerminal; }

    void Print() const;

private:
    DISALLOW_COPY_AND_ASSIGN(Terminal);

    Node              *m_node;
    int                m_id;
    Device            *m_device;
    TerminalType       m_type;
    SchematicTerminal *m_sTerminal; // for creating SchematicWire

    friend class SchematicTerminal;
};

#endif // NETLISTVIZ_CIRCUIT_TERMINAL_H
