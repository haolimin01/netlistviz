#include "Device.h"
#include <sstream>
#include <iostream>
#include <QtMath>
#include <QDebug>
#include "Node.h"
#include "Terminal.h"
#include "ASG/Wire.h"
#include "Connector.h"

Device::Device(DeviceType type, QString name)
{
    m_name = name;
    m_deviceType = type;
    m_id = 0;
    m_maybeAtFirstLevel = false;
    m_reverse = false;
    m_reverse = false;
    m_logRow = 0;
    m_levelId = 0;
    m_sDevice = nullptr;
    m_groundCap = false;
    m_orien = Horizontal;
    m_geoRow = 0;
    m_geoCol = 0;
}

Device::~Device()
{
#ifdef TRACEx
    qInfo() << LINE_INFO << "deleting " << m_name << " and terminals" << endl;
#endif
    TerminalTable::iterator it;
    it = m_terminals.begin();
    for (; it != m_terminals.end(); ++ it)
        delete it.value();
    m_terminals.clear();

    m_connectors.clear();
    m_predecessors.clear();
    m_fellows.clear();
    m_successors.clear();
    ClearConnectors();
}

int Device::AddTerminal(Terminal *terminal, TerminalType type)
{
    m_terminals[type] = terminal;
    terminal->SetTerminalType(type);
    return OKAY;
}

Terminal* Device::GetTerminal(TerminalType type) const
{
    Terminal *terminal = nullptr;
    TerminalTable::const_iterator finder;
    finder = m_terminals.find(type);
    if (finder != m_terminals.constEnd())
        terminal = finder.value();
    else
        terminal = nullptr;
    
    return terminal;
}

Terminal* Device::GetTerminal(Node *node) const
{
    Terminal *terminal = nullptr;
    foreach (terminal, m_terminals.values()) {
        if (terminal->GetNode()->Id() == node->Id())
            return terminal;
    }

    return nullptr;
}

void Device::AddConnectDevice(Device *dev)
{
    Q_ASSERT(dev);

    TerminalTable::const_iterator thisCit, otherCit;
    thisCit = m_terminals.constBegin();
    for (; thisCit != m_terminals.constEnd(); ++ thisCit) {
        otherCit = dev->m_terminals.constBegin();
        for (; otherCit != dev->m_terminals.constEnd(); ++ otherCit) {
            if (thisCit.value()->NodeId() == otherCit.value()->NodeId()) {
                Connector *cd = new Connector(thisCit.value(), otherCit.value(), dev);
                m_connectors.push_back(cd);
            }
        }
    }
}

bool Device::CoupledCap() const
{
    bool coupled = false;
    if ((m_deviceType == CAPACITOR) && (NOT m_groundCap))
        coupled = true;
    return coupled;
}

void Device::ClassifyConnectDeviceByLevel()
{
    m_predecessors.clear();
    m_fellows.clear();
    m_successors.clear();

    if (m_connectors.size() < 1)
        return;

    Device *cntDev = nullptr;
    int cntDevLevelId = 0;
    foreach (Connector *cd, m_connectors) {
        cntDev = cd->connectDevice;
        cntDevLevelId = cntDev->LevelId();
        if (cntDevLevelId == m_levelId)
            m_fellows.push_back(cntDev);
        else if (cntDevLevelId == m_levelId - 1)
            m_predecessors.push_back(cntDev);
        else if (cntDevLevelId == m_levelId + 1)
            m_successors.push_back(cntDev);
    }
}

WireList Device::WiresFromPredecessors() const
{
    WireList wires;
    Terminal *thisTer = nullptr, *cntTer = nullptr;
    Device *cntDev = nullptr;

    foreach (Connector *ct, m_connectors) {
        thisTer = ct->thisTerminal;
        if (thisTer->NodeIsGnd()) continue;
        cntDev = ct->connectDevice;
        if ((cntDev->LevelId() + 1) != m_levelId) continue;
        cntTer = ct->connectTerminal;
        // new wire
        Wire *newWire = new Wire(cntDev, cntTer, const_cast<Device*>(this), thisTer);
        wires.push_back(newWire);
    }

    return wires;
}

TerminalList Device::GetTerminalList() const
{
    TerminalList result;
    TerminalTable::const_iterator cit;
    cit = m_terminals.constBegin();
    for (; cit != m_terminals.constEnd(); ++ cit) {
        result.push_back(cit.value());
    }

    return result;
}

void Device::CalLogicalRowByPredecessors()
{
    if (m_predecessors.size() < 1) {
        m_logRow = 0;
        return;
    }

    int sum = 0, count = 0;
    foreach (Device *dev, m_predecessors) {
        sum += dev->LogicalRow();
        count++;
    }

    if (count == 0)
        m_logRow = 0;
    else
        m_logRow = (sum / count);
}

#if 0
/* just consider R, L, C, V, I */
void Device::DecideReverseByPredecessors(IgnoreCap ignore)
{
    int connectionsAtPos = 0;
    int connectionsAtNeg = 0;
    Device *device = nullptr;
    foreach (device, m_predecessors) {
        if (ignore == IgnoreGCap && device->GroundCap()) continue;
        if ((ignore == IgnoreGCCap) && (device->GroundCap() || device->CoupledCap())) continue;

        if (device->m_reverse) { // to predecessor's positive terminal
            Terminal *posTer = device->GetTerminal(Positive);
            if (HasConnection(posTer, Positive))
                connectionsAtPos++;
            else if (HasConnection(posTer, Negative))
                connectionsAtNeg++;
        } else { // to predecessor's negative terminal
            Terminal *negTer = device->GetTerminal(Negative);
            if (HasConnection(negTer, Positive))
                connectionsAtPos++;
            else if (HasConnection(negTer, Negative))
                connectionsAtNeg++;
        }
    }

    if (connectionsAtNeg > connectionsAtPos)
        m_reverse = true;
    else
        m_reverse = false;
}
#endif

/* Just consider R, L, C, V, I */
void Device::DecideReverseByPredecessors()
{
    qreal reverseLen = 0, noReverseLen = 0;
    Device *dev = nullptr;
    Terminal *posTer = nullptr, *negTer = nullptr;

    if (m_orien == Horizontal) {
        foreach (dev, m_predecessors) {
            posTer = dev->GetTerminal(Positive);
            if (HasConnection(posTer, Positive)) {
                noReverseLen += 0.5;
                reverseLen += 1.5;
            } else if (HasConnection(posTer, Negative)) {
                noReverseLen += 1.5;
                reverseLen += 0.5;
            }

            negTer = dev->GetTerminal(Negative);
            if (HasConnection(negTer, Positive)) {
                noReverseLen += 0.5;
                reverseLen += 1.5;
            } else if (HasConnection(negTer, Negative)) {
                noReverseLen += 1.5;
                reverseLen += 0.5;
            }
        }

        if (reverseLen < noReverseLen)
            m_reverse = true;
        else
            m_reverse = false;
        return;
    }

    /* vertical */
    /* First, we assume no reverse */
    m_reverse = false;
    foreach (dev, m_predecessors) {
        posTer = dev->GetTerminal(Positive);
        if (HasConnection(posTer, Positive)) {
            noReverseLen += RowDistance(posTer, Positive);
        } else if (HasConnection(posTer, Negative)) {
            noReverseLen += RowDistance(posTer, Negative);
        }

        negTer = dev->GetTerminal(Negative);
        if (HasConnection(negTer, Positive)) {
            noReverseLen += RowDistance(negTer, Positive);
        } else if (HasConnection(negTer, Negative)) {
            noReverseLen += RowDistance(negTer, Negative);
        }
    }

    /* Second, we assume reverse */
    m_reverse = true;
    foreach (dev, m_predecessors) {
        posTer = dev->GetTerminal(Positive);
        if (HasConnection(posTer, Positive)) {
            reverseLen += RowDistance(posTer, Positive);
        } else if (HasConnection(posTer, Negative)) {
            reverseLen += RowDistance(posTer, Negative); 
        }

        negTer = dev->GetTerminal(Negative);
        if (HasConnection(negTer, Positive)) {
            reverseLen += RowDistance(negTer, Positive); 
        } else if (HasConnection(negTer, Negative)) {
            reverseLen += RowDistance(negTer, Negative); 
        }
    }

    if (reverseLen < noReverseLen)
        m_reverse = true;
    else
        m_reverse = false;
}

void Device::DecideReverseBySuccessors()
{
    qreal reverseLen = 0, noReverseLen = 0;
    Device *dev = nullptr;
    Terminal *posTer = nullptr, *negTer = nullptr;

    if (m_orien == Horizontal) {
        foreach (dev, m_successors) {
            posTer = dev->GetTerminal(Positive);
            if (HasConnection(posTer, Positive)) {
                noReverseLen += 1.5;
                reverseLen += 0.5;
            } else if (HasConnection(posTer, Negative)) {
                noReverseLen += 0.5;
                reverseLen += 1.5;
            }

            negTer = dev->GetTerminal(Negative);
            if (HasConnection(negTer, Positive)) {
                noReverseLen += 1.5;
                reverseLen += 0.5;
            } else if (HasConnection(negTer, Negative)) {
                noReverseLen += 0.5;
                reverseLen += 1.5;
            }
        }

        if (reverseLen < noReverseLen)
            m_reverse = true;
        else
            m_reverse = false;
        return;
    }

    /* vertical */
    /* First, we assume no reverse */
    m_reverse = false;
    foreach (dev, m_predecessors) {
        posTer = dev->GetTerminal(Positive);
        if (HasConnection(posTer, Positive)) {
            noReverseLen += RowDistance(posTer, Positive);
        } else if (HasConnection(posTer, Negative)) {
            noReverseLen += RowDistance(posTer, Negative);
        }

        negTer = dev->GetTerminal(Negative);
        if (HasConnection(negTer, Positive)) {
            noReverseLen += RowDistance(negTer, Positive);
        } else if (HasConnection(negTer, Negative)) {
            noReverseLen += RowDistance(negTer, Negative);
        }
    }

    /* Second, we assume reverse */
    m_reverse = true;
    foreach (dev, m_predecessors) {
        posTer = dev->GetTerminal(Positive);
        if (HasConnection(posTer, Positive)) {
            reverseLen += RowDistance(posTer, Positive);
        } else if (HasConnection(posTer, Negative)) {
            reverseLen += RowDistance(posTer, Negative); 
        }

        negTer = dev->GetTerminal(Negative);
        if (HasConnection(negTer, Positive)) {
            reverseLen += RowDistance(negTer, Positive); 
        } else if (HasConnection(negTer, Negative)) {
            reverseLen += RowDistance(negTer, Negative); 
        }
    }

    if (reverseLen < noReverseLen)
        m_reverse = true;
    else
        m_reverse = false;
}

bool Device::HasConnection(Terminal *otherTer, TerminalType thisType) const
{
    bool has = false;
    Terminal *thisTer = m_terminals[thisType];
    if (thisTer->NodeId() != 0 && otherTer->NodeId() != 0) {
        if (thisTer->NodeId() == otherTer->NodeId())
            has = true;
    }

    return has;
}

void Device::ClearConnectors()
{
    foreach (Connector *cd, m_connectors)
        delete cd;
    m_connectors.clear();
}

/* R, L, V, V, I now */
bool Device::MaybeVertical() const
{
    TerminalTable cntToPredTers;
    Terminal *thisTer = nullptr;
    Device *cntDev = nullptr;
    int cntToGndCount = 0;

    foreach (Connector *cd, m_connectors) {
        cntDev = cd->connectDevice;
        if ((cntDev->LevelId() + 1) != m_levelId) continue; // predecessor
        thisTer = cd->thisTerminal;
        if (thisTer->NodeIsGnd()) {
            cntToGndCount++;
            continue;
        }
        cntToPredTers.insert(thisTer->GetTerminalType(), thisTer);
    }

#ifdef DEBUGx
    qDebug() << cntToPredTers;
#endif

    int count = cntToPredTers.size();
    if (count >= 2)
        return true;
    
    count += cntToGndCount;
    if (count < 2)
        return false;
    
    bool nextRowFilled = false;
    foreach (Device *dev, m_fellows)
        if (dev->LogicalRow() == m_logRow + 1) {
            nextRowFilled = true;
            break;
        }
    
    if (NOT nextRowFilled)
        return true;
    else
        return false;
}

void Device::DecideOrientationByPredecessors()
{
    if (MaybeVertical())
        m_orien = Vertical;
    else
        m_orien = Horizontal;
}

qreal Device::RowDistance(Terminal *otherTer, TerminalType thisType) const
{
    Terminal *thisTer = m_terminals[thisType];
    return qFabs(otherTer->LogicalRelRow() - thisTer->LogicalRelRow());
}

WireList Device::WiresToFellows() const
{
    WireList wires;
    Terminal *thisTer = nullptr, *cntTer = nullptr;
    Device *cntDev = nullptr;
    Node *node = nullptr;

    DeviceTable fellowTable;
    foreach (Device *dev, m_fellows)
        fellowTable.insert(dev->Name(), dev);
    bool isFellow = false;

    foreach (Connector *ct, m_connectors) {
        thisTer = ct->thisTerminal;
        if (thisTer->NodeIsGnd()) continue;
        cntDev = ct->connectDevice;
        if (cntDev->LevelId() != m_levelId) continue;

        node = thisTer->GetNode();
        isFellow = false;
        foreach (Device *dev, node->ConnectDeviceList()) {
            if (dev == this) continue;
            isFellow = fellowTable.contains(dev->Name());
            if (NOT isFellow)
                break;
        }

        if (NOT isFellow) continue;

        cntTer = ct->connectTerminal;
        // new wire
        Wire *newWire = new Wire(const_cast<Device*>(this), thisTer, cntDev, cntTer);
        wires.push_back(newWire);
    }

#ifdef DEBUGx
    printf("---------- Wires To Fellows ----------\n");

    qInfo() << "Device " << m_name;
    foreach (Wire *wire, wires) {
        qInfo() << wire->Name();
    }

    printf("--------------------------------------\n");
#endif

    return wires;
}

void Device::Print() const
{
    std::stringstream ss;
    ss << "******************************\n";
    ss << "Name(" << m_name.toStdString() << "), ";
    ss << "type(" << m_deviceType << "), ";
    ss << "id(" << m_id << "), ";
    ss << "maybeAtFirstLevel(" << m_maybeAtFirstLevel << ")";
    if (m_deviceType == CAPACITOR) {
        ss << ", coupled(" << CoupledCap() << ")";
    }
    std::cout << ss.str() << std::endl;
    foreach (Terminal *terminal, m_terminals.values())
        terminal->Print();
    std::cout << "******************************\n";
}
