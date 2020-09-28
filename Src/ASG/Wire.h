#ifndef NETLISTVIZ_ASG_WIRE_H
#define NETLISTVIZ_ASG_WIRE_H

/*
 * @filename : Wire.h
 * @date     : 2020.09.14
 * @author   : Hao Limin 
 * @email    : haolimin01@sjtu.edu.cn
 * @desp     : Logical Wire class, added to Channel.
 *           : Wire will be hholded by SchematicWire.
 */

#include "Define/Define.h"

class Device;
class Terminal;
class Channel;
class SchematicWire;
class SchematicDevice;
class SchematicTerminal;

class Wire
{
public:
    Wire(Device *fromDevice, Terminal *fromTerminal, Device *toDevice, Terminal *toTerminal);
    ~Wire();

    void SetTrack(int track)  { m_trackGiven = true; m_track = track; }
    int  Track() const        { return m_track; }
    void SetChannelId(int id) { m_channelId = id; }
    int  ChannelId() const    { return m_channelId; }
    void SetGeometricalCol(int col) { m_geoCol = col; }
    int  GeometricalCol() const  { return m_geoCol; }
    bool TrackGiven() const   { return m_trackGiven; }
    bool HasGCap() const;
    bool HasCCap() const;
    bool IsHorizontal() const;
    bool HasCross(Wire *otherWire) const;
    bool CouldBeMerged(Wire *otherWire) const;
    bool CouldBeSameTrack(Wire *otherWire) const;

    SchematicDevice*    FromSDevice() const;
    SchematicDevice*    ToSDevice() const;
    SchematicTerminal*  FromSTerminal() const;
    SchematicTerminal*  ToSTerminal() const;

private:
    DISALLOW_COPY_AND_ASSIGN(Wire);

    int         m_track;
    int         m_geoCol;

    Device     *m_fromDevice;
    Device     *m_toDevice;
    Terminal   *m_fromTerminal;
    Terminal   *m_toTerminal;

    int         m_channelId;
    bool        m_trackGiven;

    friend class Channel;
};

#endif //  NETLISTVIZ_ASG_WIRE_H
