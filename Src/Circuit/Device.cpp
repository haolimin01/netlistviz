#include "Device.h"
#include <sstream>
#include <iostream>
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
    WireList result;
    Wire *newWire = nullptr;

    TerminalTable::const_iterator thisCit;
    TerminalTable::const_iterator otherCit;

    foreach (Device *dev, m_predecessors) {
        otherCit = dev->m_terminals.constBegin();
        for (; otherCit != dev->m_terminals.constEnd(); ++ otherCit) {
            thisCit = m_terminals.constBegin();
            for (; thisCit != m_terminals.constEnd(); ++ thisCit) {
                if (thisCit.value()->NodeId() == 0) continue;
                if (otherCit.value()->NodeId() == thisCit.value()->NodeId()) {
                    // new Wire
                    newWire = new Wire(dev, otherCit.value(),
                                       const_cast<Device*>(this), thisCit.value());
                    result.push_back(newWire);
                }
            }
        }

    }

    return result;
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
            if (HasConnectionIgnoreGnd(posTer, Positive))
                connectionsAtPos++;
            else if (HasConnectionIgnoreGnd(posTer, Negative))
                connectionsAtNeg++;
        } else { // to predecessor's negative terminal
            Terminal *negTer = device->GetTerminal(Negative);
            if (HasConnectionIgnoreGnd(negTer, Positive))
                connectionsAtPos++;
            else if (HasConnectionIgnoreGnd(negTer, Negative))
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
    /* Horizontal */
    if (m_orien == Horizontal) {
        int cntAtPos = 0;
        int cntAtNeg = 0;
        Device *dev = nullptr;

        foreach (dev, m_predecessors) {
            if (dev->m_reverse) {
                Terminal *posTer = dev->GetTerminal(Positive);
                if (HasConnectionIgnoreGnd(posTer, Positive))
                    cntAtPos++;
                else if (HasConnectionIgnoreGnd(posTer, Negative))
                    cntAtNeg++;
            } else {
                Terminal *negTer = dev->GetTerminal(Negative);
                if (HasConnectionIgnoreGnd(negTer, Positive))
                    cntAtPos++;
                else if (HasConnectionIgnoreGnd(negTer, Negative))
                    cntAtNeg++;
            }
        }
        if (cntAtNeg > cntAtPos)
            m_reverse = true;
        else
            m_reverse = false;

        return;
    }

    /* Vertical */
    int rowSumAtPos = 0, rowSumAtNeg = 0;
    Device *dev = nullptr;

    foreach (dev, m_predecessors) {
        if (HasConnectionIgnoreGnd(dev, Positive))
            rowSumAtPos += dev->LogicalRow();
        else if (HasConnectionIgnoreGnd(dev, Negative))
            rowSumAtNeg += dev->LogicalRow();
    }

    if (rowSumAtPos > rowSumAtNeg)
        m_reverse = true;
    else
        m_reverse = false;
}

void Device::DecideReverseBySuccessors()
{
    /* Horizontal */
    if (m_orien == Horizontal) {
        int cntAtPos = 0;
        int cntAtNeg = 0;
        Device *dev = nullptr;

        foreach (dev, m_successors) {
            if (dev->m_reverse) {
                Terminal *negTer = dev->GetTerminal(Negative);
                if (HasConnectionIgnoreGnd(negTer, Positive))
                    cntAtPos++;
                else if (HasConnectionIgnoreGnd(negTer, Negative))
                    cntAtNeg++;
            } else {
                Terminal *posTer = dev->GetTerminal(Positive);
                if (HasConnectionIgnoreGnd(posTer, Positive))
                    cntAtPos++;
                else if (HasConnectionIgnoreGnd(posTer, Negative))
                    cntAtNeg++;
            }
        }
        if (cntAtPos > cntAtNeg)
            m_reverse = true;
        else
            m_reverse = false;

        return;
    }

    /* Vertical */
    int rowSumAtPos = 0, rowSumAtNeg = 0;
    Device *dev = nullptr;

    foreach (dev, m_successors) {
        if (HasConnectionIgnoreGnd(dev, Positive))
            rowSumAtPos += dev->LogicalRow();
        else if (HasConnectionIgnoreGnd(dev, Negative))
            rowSumAtNeg += dev->LogicalRow();
    }

    if (rowSumAtPos > rowSumAtNeg)
        m_reverse = true;
    else
        m_reverse = false;
}

bool Device::HasConnectionIgnoreGnd(Terminal *otherTer, TerminalType thisType) const
{
    bool has = false;
    Terminal *thisTer = m_terminals[thisType];
    if (thisTer->NodeId() != 0 && otherTer->NodeId() != 0) {
        if (thisTer->NodeId() == otherTer->NodeId())
            has = true;
    }

    return has;
}

bool Device::HasConnectionIgnoreGnd(Device *otherDev, TerminalType type) const
{
    Terminal *thisTer = m_terminals[type];
    foreach (Connector *cd, m_connectors) {
        if (cd->connectTerminal->Id() == 0) continue;
        if (cd->connectDevice == otherDev && cd->thisTerminal == thisTer) {
            return true;
        }
    }

    return false;
}

void Device::ClearConnectors()
{
    foreach (Connector *cd, m_connectors)
        delete cd;
    m_connectors.clear();
}

#if 0
STerminalTable Device::CapConnectSTerminalTable() const
{
    STerminalTable table;
    TerminalTable::const_iterator cit;
    cit = m_capConnectTerminalTable.constBegin();
    for (; cit != m_capConnectTerminalTable.constEnd(); ++ cit) {
        table.insert(cit.key(), cit.value()->GetSchematicTerminal());
    }
    return table;
}
#endif

/* R, L, V, V, I now */
bool Device::MaybeVertical() const
{
    int cntToPredCount = m_predecessors.size();
    int cntToGndCount = 0;
    
    foreach (Terminal *ter, m_terminals.values()) {
        if (ter->NodeId() == 0)
            cntToGndCount++;
    }

    // int sum = cntToPredCount + cntToGndCount;
    int sum = cntToPredCount;

    if (sum >= 2)
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
