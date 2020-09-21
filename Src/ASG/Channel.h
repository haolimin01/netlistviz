#ifndef NETLISTVIZ_ASG_CHANNEL_H
#define NETLISTVIZ_ASG_CHANNEL_H

/*
 * @filename : Channel.h
 * @date     : 2020.09.14
 * @author   : Hao Limin
 * @email    : haolimin01@sjtu.edu.cn
 * @desp     : Channel represents space between Level, contains some Wires.
 */

#include "Define/Define.h"
#include "Define/TypeDefine.h"

class Wire;

class Channel
{
public:
    explicit Channel(int id);
    ~Channel();

    void      AddWire(Wire *wire);
    void      AddWires(const WireList &wires);
    void      AssignTrackNumber(bool ignoreGroundCap = false);
    WireList  Wires() const { return m_wires; }
    int       TrackCount() const { return m_trackCount; }

    void Print() const;

private:
    WireList   m_wires;
    int        m_trackCount;
    int        m_id;
};

#endif // NETLISTVIZ_ASG_CHANNEL_H
