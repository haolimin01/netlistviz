#ifndef NETLISTVIZ_ASG_CHANNEL_H
#define NETLISTVIZ_ASG_CHANNEL_H

/*
 * @filename : Channel.h
 * @date     : 2020.09.14
 * @author   : Hao Limin
 * @email    : haolimin01@sjtu.edu.cn
 * @desp     : Channel represents space between Level, contains some Wires.
 * @modified : Hao Limin, 2020.09.24
 */

#include "Define/Define.h"
#include "Define/TypeDefine.h"

class Wire;

class Channel
{
public:
    explicit Channel(int id);
    Channel();
    ~Channel();

    void      SetId(int id) { m_id = id; }
    int       Id() const    { return m_id; }
    void      AddWire(Wire *wire);
    void      AddWires(const WireList &wires);
    void      AssignTrackNumber(IgnoreCap ignore);
    WireList  Wires() const { return m_wires; }
    int       TrackCount() const { return m_trackCount; }
    bool      Empty() const { return (m_wires.size() == 0); }

    void Print() const;

private:
    DISALLOW_COPY_AND_ASSIGN(Channel);

    WireList   m_wires;
    int        m_trackCount;
    int        m_id;
};

#endif // NETLISTVIZ_ASG_CHANNEL_H
