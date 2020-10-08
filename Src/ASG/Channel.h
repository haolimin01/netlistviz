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
#include <QMap>

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
    int       HoldColCount() const; // hold colCount on scene this channel
    void      AssignDotGeometricalCol(int col);
    DotList   Dots() const { return m_dots; }

    void Print() const;

private:
    DISALLOW_COPY_AND_ASSIGN(Channel);

    bool      CouldBeSameTrackWithWires(const WireList &wires, Wire *wire) const;
    bool      HaveCrossBetweenWires(const WireList &wl1, const WireList &wl2) const;
    void      CreateDots(const QVector<WireList> &mergedWireList);

    WireList   m_wires;
    int        m_id;
    int        m_trackCount;

    DotList    m_dots;
};

#endif // NETLISTVIZ_ASG_CHANNEL_H
