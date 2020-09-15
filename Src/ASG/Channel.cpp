#include "Channel.h"
#include "Wire.h"
#include <QDebug>
#include "Circuit/Device.h"
#include "Circuit/Terminal.h"

Channel::Channel(int id)
{
    m_id = id;
    m_trackCount = 0;
}

Channel::~Channel()
{
    /* just clear WireList, deleing wires in SchematicWire */
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
void Channel::AssignTrackNumber()
{
    if (m_wires.size() < 1)
        return;

    /* First, we assign -1 to horizontal line */
    Wire *wire = nullptr;
    Device *fromDevice = nullptr, *toDevice = nullptr;
    
    int baseline = 0;
    int fromDeviceRowSum = 0;
    int fromDeviceCount = 0;

    foreach (wire, m_wires) {
        fromDevice = wire->m_fromDevice;
        toDevice = wire->m_toDevice;
        if (fromDevice->Row() == toDevice->Row())
            wire->SetTrack(-1);
        fromDeviceRowSum += fromDevice->Row();
        fromDeviceCount++;
    }

    baseline = fromDeviceRowSum / fromDeviceCount;

    /* sort m_wires by toDevice row */
    qSort(m_wires.begin(), m_wires.end(), [](Wire *w1, Wire *w2) {return w1->m_toDevice->Row() < w2->m_toDevice->Row();});

    int number = 0;

    /* Second, assign number to lines, which are above on baseline */
    foreach (wire, m_wires) {
        if (wire->TrackGiven()) continue;
        toDevice = wire->m_toDevice;
        if (toDevice->Row() <= baseline) {
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
        if (toDevice->Row() > baseline) {
            wire->SetTrack(number);
            number++;
        } else {
            break;
        }
    }

    m_trackCount = number;
}

void Channel::Print() const
{
    printf("--------------- Channel %d ---------------\n", m_id);
    QString tmp;
    foreach (Wire *wire, m_wires) {
        tmp += (wire->m_fromDevice->Name() + "(" + QString::number(wire->m_fromDevice->Row())+ "), ");
        tmp += (wire->m_toDevice->Name() + "(" + QString::number(wire->m_toDevice->Row()) + "), ");
        tmp += ("track(" + QString::number(wire->m_track) + ")");
        qInfo() << tmp;
        tmp = "";
    }
    printf("------------------------------------------\n");
}