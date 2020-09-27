#include "Channel.h"
#include "Wire.h"
#include <QDebug>
#include "Circuit/Device.h"
#include "Circuit/Terminal.h"

Channel::Channel(int id)
{
    m_id = id;
    m_trackCount = 0;
    m_geoCol = 0;
}

Channel::Channel()
{
    m_id = 0;
    m_trackCount = 0;
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
 * baseline = sum(from device row) / number (from device)
 * above on baseline    : the row is less, track is smaller
 * below under baseline : the row is greater, track is smaller
 * horizontal line      : track is -1
 */

void Channel::AssignTrackNumber(IgnoreCap ignore)
{
    if (m_wires.size() < 1)
        return;

    /* First, we assign -1 to horizontal line */
    Wire *wire = nullptr;
    Device *fromDevice = nullptr, *toDevice = nullptr;
    
    int baseline = 0;
    int fromDeviceLogicalRowSum = 0;
    int fromDeviceCount = 0;

    foreach (wire, m_wires) {
        if (ignore == IgnoreGCap && wire->HasGCap()) {
            wire->SetTrack(-1);
            continue;
        }
        if ((ignore == IgnoreGCCap) && (wire->HasGCap() || wire->HasCCap())) {
            wire->SetTrack(-1);
            continue;
        }

        fromDevice = wire->m_fromDevice;
        toDevice = wire->m_toDevice;
        if (fromDevice->LogicalRow() == toDevice->LogicalRow())
            wire->SetTrack(-1);
        fromDeviceLogicalRowSum += fromDevice->LogicalRow();
        fromDeviceCount++;
    }

    if (fromDeviceCount == 0)
        baseline = 0;
    else
        baseline = fromDeviceLogicalRowSum / fromDeviceCount;

    /* sort m_wires by toDevice row */
    qSort(m_wires.begin(), m_wires.end(), [](Wire *w1, Wire *w2) {return w1->m_toDevice->LogicalRow() < w2->m_toDevice->LogicalRow();});

    int number = 0;

    /* Second, assign number to lines, which are above on baseline */
    foreach (wire, m_wires) {
        if (wire->TrackGiven()) continue;
        toDevice = wire->m_toDevice;
        if (toDevice->LogicalRow() <= baseline) {
            wire->SetTrack(number);
            number++;
        } else {
            break;
        } 
    }

    /* Third, assign number to lines, which are below under baseline */
    for (int i = m_wires.size() - 1; i >= 0; -- i) {
        wire = m_wires.at(i);
        if (wire->TrackGiven()) continue;
        toDevice = wire->m_toDevice;
        if (toDevice->LogicalRow() > baseline) {
            wire->SetTrack(number);
            number++;
        } else {
            break;
        }
    }

    m_trackCount = number;
}

void Channel::SetGeometricalCol(int col)
{
    m_geoCol = col;
    foreach (Wire *wire, m_wires)
        wire->SetGeometricalCol(col);
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
    printf("------------------------------------------\n");
}
