#include "Channel.h"
#include "Wire.h"
#include <QDebug>
#include "Circuit/Device.h"
#include "Circuit/Terminal.h"
#include "Dot.h"

Channel::Channel(int id)
{
    m_id = id;
}

Channel::Channel()
{
    m_id = 0;
}

Channel::~Channel()
{
    /* delete wires here */
    foreach (Wire *wire, m_wires)
        delete wire;
    m_wires.clear();

    /* delete dots here */
    foreach (Dot *dot, m_dots)
        delete dot;
    m_dots.clear();
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
 * horizontal line: track is -1
 */
void Channel::AssignTrackNumber(IgnoreCap ignore)
{
    Q_UNUSED(ignore);

    if (m_wires.size() < 1)
        return;

    /* First, tag horizontal wire */
    int id = 0;
    Wire *wire = nullptr;
    foreach (wire, m_wires) {
        if (wire->IsHorizontal()) {
            wire->SetTrack(-1);
        }
    }

    /* Second, sort wires by toDevice row */
    qSort(m_wires.begin(), m_wires.end(),
            [](Wire *w1, Wire *w2) {return w1->m_toDevice->LogicalRow() > w2->m_toDevice->LogicalRow();});

    /* Third, put could be merged wires together */
    QVector<WireList> mergedWireList;
    bool merged = false;

    foreach (wire, m_wires) {
        if (wire->TrackGiven()) continue;

        for (int i = 0; i < mergedWireList.size(); ++ i) {
            WireList &wl = mergedWireList[i];
            if (wire->CouldBeMerged(wl.front())) {
                wl.push_back(wire);
                merged = true;
                break;
            }
        }

        if (NOT merged) {
            WireList newWl;
            newWl.push_back(wire);
            mergedWireList.push_back(newWl);
        }
        merged = false;
    }

#ifdef DEBUGx
    printf("--------------- Channel %d ---------------\n", m_id);

    foreach (const WireList &wl, mergedWireList) {
        printf("********* Merged *******\n");

        foreach (Wire *w, wl)
            qInfo() << w->Name();

        printf("************************\n");
    }
#endif

    /* Fourth, combine wirelist with no cross */
    QVector<WireList> sameTrackWireList;
    bool haveCross = true;
    foreach (const WireList &wl, mergedWireList) {
        for (int i = 0; i < sameTrackWireList.size(); ++ i) {
            WireList &target = sameTrackWireList[i];
            if (NOT HaveCrossBetweenWires(wl, target)) {
                target.append(wl);
                haveCross = false;
                break;
            }
        }

        if (haveCross) {
            sameTrackWireList.push_back(wl);
        }

        haveCross = true;
    }

#ifdef DEBUGx
    foreach (const WireList &wl, sameTrackWireList) {
        printf("********* SameTrack *******\n");

        foreach (Wire *w, wl)
            qInfo() << w->Name();

        printf("***************************\n");
    }

    printf("------------------------------------------\n");
#endif

    /* Fifth, assign track number */
    int trackIndex = 0;
    foreach (const WireList wl, sameTrackWireList) {
        foreach (Wire *w, wl)
            w->SetTrack(trackIndex);
        trackIndex++;
    }

    m_trackCount = trackIndex;

    /* Sixth, create dots */
    CreateDots(mergedWireList);
}

void Channel::CreateDots(const QVector<WireList> &mergedWireList)
{
    m_dots.clear();
    std::set<int> dotSet;
    Terminal *terminal = nullptr;

    /* First, cross points with horizontal wires */
    foreach (Wire *thisWire, m_wires) {
        if (thisWire->Track() != -1) continue; // not horizontal wire
        foreach (Wire *otherWire, m_wires) {
            if (otherWire->Track() == -1) continue; // horizontal wire

            if (thisWire->CouldBeMerged(otherWire)) {
                terminal = thisWire->SameTerminal(otherWire);
                if (NOT SetContains(dotSet, terminal->Id())) {
                    // new dot
                    Dot *newDot = new Dot(m_id, otherWire->Track(), terminal);
                    m_dots.push_back(newDot);
                }
            }
        }
    }

#ifdef DEBUGx
    printf("--------------- Channel %d ---------------\n", m_id);

    // std::set<int>::const_iterator cit = dotSet.cbegin();
    // for (; cit != dotSet.cend(); ++ cit)
    //     qInfo() << *cit;

    printf("========== Dots with horizontal wires ==========\n");
    foreach (Dot *dot, m_dots) {
        qInfo() << dot->Track() << dot->DeviceName() << dot->TerminalId();
    }
#endif

    /* Second, cross points between mergedWireList */
    Wire *thisWire = nullptr, *otherWire = nullptr;
    foreach (const WireList &wl, mergedWireList) {
        for (int i = 0; i < wl.size(); ++ i) {
            thisWire = wl.at(i);
            for (int j = i + 1; j < wl.size(); ++ j) {
                otherWire = wl.at(j);

                /* No horizontal wires, they MUST be merged */
                Q_ASSERT(thisWire->Track() == otherWire->Track());

                terminal = thisWire->SameTerminal(otherWire);
                if (NOT SetContains(dotSet, terminal->Id())) {
                    // new dot
                    Dot *newDot = new Dot(m_id, thisWire->Track(), terminal);
                    m_dots.push_back(newDot);
                }
            }
        }
    }

#ifdef DEBUGx
    printf("=================== All Dots ===================\n");
    // std::set<int>::const_iterator cit = dotSet.cbegin();
    // cit = dotSet.cbegin();
    // for (; cit != dotSet.cend(); ++ cit)
    //     qInfo() << *cit;
    foreach (Dot *dot, m_dots) {
        qInfo() << dot->Track() << dot->DeviceName() << dot->TerminalId();
    }

    printf("------------------------------------------\n");
#endif

}

bool Channel::SetContains(std::set<int> &Set, int key) const
{
    std::set<int>::const_iterator finder = Set.find(key);
    if (finder != Set.cend())
        return true;

    Set.insert(key);
    return false;
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

bool Channel::HaveCrossBetweenWires(const WireList &wl1, const WireList &wl2) const
{
    foreach (Wire *w1, wl1) {
        foreach (Wire *w2, wl2) {
            if (w1->HasCross(w2))
                return true;
        }
    }

    return false;
}

int Channel::HoldColCount() const
{
    int count = m_trackCount * 1.0 / MAX_ONE_COL_WIRE_COUNT + 0.5;
    if (count < 1)
        count = 1;
    return count;
}

void Channel::AssignDotGeometricalCol(int col)
{
    foreach (Dot *dot, m_dots)
        dot->SetGeometricalCol(col);
}

void Channel::Print() const
{
    printf("--------------- Channel %d ---------------\n", m_id);
    QString tmp;
    foreach (Wire *wire, m_wires) {
        tmp += (wire->m_fromDevice->Name() + "(" + QString::number(wire->m_fromDevice->LogicalRow())+ "), ");
        tmp += (wire->m_toDevice->Name() + "(" + QString::number(wire->m_toDevice->LogicalRow()) + "), ");
        tmp += ("track(" + QString::number(wire->m_track) + "), ");
        qInfo() << tmp;
        tmp = "";
    }
    qInfo() << "trackCount(" << m_trackCount << ")";
    qInfo() << "holdColCount(" << HoldColCount() << ")";
    qInfo() << "wireCount(" << m_wires.size() << ")";

    printf("------------------------------------------\n");
}
