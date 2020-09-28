#include "Channel.h"
#include "Wire.h"
#include <QDebug>
#include "Circuit/Device.h"
#include "Circuit/Terminal.h"

Channel::Channel(int id)
{
    m_id = id;
    m_geoCol = 0;
}

Channel::Channel()
{
    m_id = 0;
    m_geoCol = 0;
}

Channel::~Channel()
{
    /* delete wires here */
    foreach (Wire *wire, m_wires)
        delete wire;
    m_wires.clear();
}

void Channel::AddWire(Wire *wire)
{
    m_wires.push_back(wire);
    wire->SetChannelId(m_id);
}

void Channel::AddWires(const WireList &wires)
{
    foreach (Wire *wire, wires)
        AddWire(wire);
}

/*
 * horizontal line      : track is -1
 */

void Channel::AssignTrackNumber(IgnoreCap ignore)
{
    Q_UNUSED(ignore);

    if (m_wires.size() < 1)
        return;

    /* First, we assign -1 to horizontal line */
    Wire *wire = nullptr;

    foreach (wire, m_wires) {
#if 0
        if (ignore == IgnoreGCap && wire->HasGCap()) {
            wire->SetTrack(-1);
            continue;
        }
        if ((ignore == IgnoreGCCap) && (wire->HasGCap() || wire->HasCCap())) {
            wire->SetTrack(-1);
            continue;
        }
#endif

        if (wire->IsHorizontal())
            wire->SetTrack(-1);
    }

    /* sort m_wires by toDevice row */
    qSort(m_wires.begin(), m_wires.end(),
            [](Wire *w1, Wire *w2) {return w1->m_toDevice->LogicalRow() < w2->m_toDevice->LogicalRow();});

    int trackIndex = 0;
    bool hasRest = false;
    WireList sameTrackWires;
    Wire *otherWire = nullptr;

    while (true) {
        for (int i = 0; i < m_wires.size(); ++ i) {
            if (NOT m_wires.at(i)->TrackGiven()) {
                wire = m_wires.at(i);
                hasRest = true;
                break;
            }
        }
        if (NOT hasRest) break;

        sameTrackWires.push_back(wire);
        for (int i = 0; i < m_wires.size(); ++ i) {
            otherWire = m_wires.at(i);
            if (otherWire->TrackGiven()) continue;
            if (wire == otherWire) continue; // pointer
            if (wire->CouldBeMerged(otherWire))
                sameTrackWires.push_back(otherWire);
        }

        for (int i = 0; i < m_wires.size(); ++ i) {
            otherWire = m_wires.at(i);
            if (otherWire->TrackGiven()) continue;
            if (wire == otherWire) continue; // pointer
            if (CouldBeSameTrackWithWires(sameTrackWires, otherWire))
                sameTrackWires.push_back(otherWire);
        }

        foreach (wire, sameTrackWires)
            wire->SetTrack(trackIndex);
        trackIndex++;

#ifdef DEBUGx
        printf("xxxxxxxxxx Same Track Wires xxxxxxxxxx\n");
        foreach (Wire *w, sameTrackWires)
            qInfo() << "(" << w->m_fromDevice->Name() << "," << w->m_toDevice->Name();
        printf("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n");
#endif

        sameTrackWires.clear();
        hasRest = false;
    }

    m_trackCount = trackIndex;
}

void Channel::SetGeometricalCol(int col)
{
    m_geoCol = col;
    foreach (Wire *wire, m_wires)
        wire->SetGeometricalCol(col);
}

bool Channel::CouldBeSameTrackWithWires(const WireList &wires, Wire *wire) const
{
    Wire *thisWire = nullptr;
    foreach (thisWire, wires) {
        if (NOT thisWire->CouldBeSameTrack(wire)) {
            return false;
        }
    }

    return true;
}

void Channel::Print() const
{
    printf("--------------- Channel %d ---------------\n", m_id);
    QString tmp;
    foreach (Wire *wire, m_wires) {
        tmp += (wire->m_fromDevice->Name() + "(" + QString::number(wire->m_fromDevice->LogicalRow())+ "), ");
        tmp += (wire->m_toDevice->Name() + "(" + QString::number(wire->m_toDevice->LogicalRow()) + "), ");
        tmp += ("track(" + QString::number(wire->m_track) + ")");
        qInfo() << tmp;
        tmp = "";
    }
    qInfo() << "trackCount(" << m_trackCount << ")";

    printf("------------------------------------------\n");
}
