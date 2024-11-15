#ifndef NETLISTVIZ_ASG_DOT_H
#define NETLISTVIZ_ASG_DOT_H

/*
 * @filename : Dot.h
 * @author   : Hao Limin
 * @date     : 2020.10.06
 * @email    : haolimin01@sjtu.edu.cn
 * @desp     : wire cross point
 */


#include "Define/Define.h"
#include "Define/TypeDefine.h"
#include <QMap>

class QString;
class Terminal;
class SchematicTerminal;

class Dot
{
public:
    Dot(int channelId, int track, Terminal *terminal);
    Dot(const Dot& otherDot);
    ~Dot();

    int       Track() const { return m_track; }
    int       TerminalId() const;
    QString   DeviceName() const;
    void      SetGeometricalCol(int col) { m_geoCol = col; }
    int       GeometricalCol() const     { return m_geoCol; }
    void      AddWire(Wire *wire);
    WireList  Wires() const;
    SchematicTerminal* STerminal() const;

private:

    int                 m_channelId;
    int                 m_geoCol;
    Terminal           *m_terminal;
    QMap<Wire*, Wire*>  m_wires; // deduplication

    int                 m_track;
};


#endif // NETLISTVIZ_ASG_DOT_H
