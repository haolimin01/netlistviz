#include "Device.h"
#include <sstream>
#include <iostream>
#include <QDebug>
#include "Node.h"
#include "Terminal.h"
#include "ASG/Wire.h"

Device::Device(DeviceType type, QString name)
{
    m_name = name;
    m_deviceType = type;
    m_id = 0;
    m_maybeAtFirstLevel = false;
    m_levelId = 0;
    m_reverse = false;
    m_bubbleValue = 0;
    m_reverse = false;
    m_row = 0;
    m_sDevice = nullptr;
    m_groundCap = false;
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

    m_predecessors.clear();
    m_successors.clear();
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
    qDebug() << Name() << endl;
    m_capConnectDeviceList.push_back(dev);

    TerminalTable::const_iterator thisCit;
    TerminalTable::const_iterator otherCit;
    thisCit = m_terminals.constBegin();
    otherCit = dev->m_terminals.constBegin();

    for (; thisCit != m_terminals.constEnd(); ++ thisCit) {
        if (thisCit.value()->NodeId() == 0) continue;
        otherCit = dev->m_terminals.constBegin();
        for (; otherCit != dev->m_terminals.constEnd(); ++ otherCit) {
            if (otherCit.value()->NodeId() == 0) continue;
            if (thisCit.value()->NodeId() == otherCit.value()->NodeId()) {
                m_capConnectTerminalTable.insert(thisCit.key(), otherCit.value());
            }
        }
    }
    
    Q_ASSERT(m_capConnectTerminalTable.size() > 0);
}

bool Device::CoupledCap() const
{
    bool coupled = false;
    if ((m_deviceType == CAPACITOR) && (NOT m_groundCap))
        coupled = true;
    return coupled;
}

/* If they connect to the same node, return true (ignore orientation) */
bool Device::HasConnection(Device *otherDevice) const
{
    bool has = false;
    TerminalTable::const_iterator thisCit;
    thisCit = m_terminals.constBegin();
    TerminalTable::const_iterator otherCit;
    Terminal *thisTerminal = nullptr, *otherTerminal = nullptr;

    for (; thisCit != m_terminals.constEnd(); ++ thisCit) {
        thisTerminal = thisCit.value();
        otherCit = otherDevice->m_terminals.constBegin();
        for (; otherCit != otherDevice->m_terminals.constEnd(); ++ otherCit) {
            otherTerminal = otherCit.value();
            if (thisTerminal->NodeId() == otherTerminal->NodeId()) {
                has = true;
                return has;
            }
        }
    }

    return has;
}

/* If they connect to the same node, return true (ignore gnd and orientation) */
bool Device::HasConnectionIgnoreGnd(Device *otherDevice) const
{
    bool has = false;
    TerminalTable::const_iterator thisCit;
    thisCit = m_terminals.constBegin();
    TerminalTable::const_iterator otherCit;
    Terminal *thisTerminal = nullptr, *otherTerminal = nullptr;

    for (; thisCit != m_terminals.constEnd(); ++ thisCit) {
        thisTerminal = thisCit.value();
        if (thisTerminal->GetNode()->IsGnd()) continue;
        otherCit = otherDevice->m_terminals.constBegin();
        for (; otherCit != otherDevice->m_terminals.constEnd(); ++ otherCit) {
            otherTerminal = otherCit.value();
            if (thisTerminal->NodeId() == otherTerminal->NodeId()) {
                has = true;
                return has;
            }
        }
    }

    return has;
}

void Device::AddPredecessor(Device *dev)
{
    m_predecessors.push_back(dev);
}

void Device::AddSuccessor(Device *dev)
{
    m_successors.push_back(dev);
}

void Device::SetRow(int row)
{
    Q_ASSERT(row >= 0);
    m_row = row;
}

void Device::CalBubbleValueByPredecessors(IgnoreCap ignore)
{
    if (m_predecessors.size() < 1) {
        m_bubbleValue = 0;
        return;
    }
    int sum = 0;
    int count = 0;
    foreach (Device *dev, m_predecessors) {
        if ((ignore == IgnoreGCap) && (dev->GroundCap())) continue;
        if ((ignore == IgnoreGCCap) && (dev->GroundCap() || dev->CoupledCap())) continue; 
        sum += dev->Row();
        count++;
    }
    if (count == 0)
        m_bubbleValue = 0;
    else
        m_bubbleValue = sum / count;
}

void Device::CalBubbleValueBySuccessors(IgnoreCap ignore)
{
    if (m_successors.size() < 1) {
        m_bubbleValue = 0;
        return;
    }
    int sum = 0;
    int count = 0;
    foreach (Device *dev, m_successors) {
        if ((ignore == IgnoreGCap) && (dev->GroundCap())) continue;
        if ((ignore == IgnoreGCCap) && (dev->GroundCap() || dev->CoupledCap())) continue; 
        sum += dev->Row();
        count++;
    }
    if (count == 0)
        m_bubbleValue = 0;
    else
        m_bubbleValue = sum / count;
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

void Device::DecideReverseBySuccessors(IgnoreCap ignore)
{
    int connectionsAtPos = 0;
    int connectionsAtNeg = 0;
    Device *device = nullptr;
    foreach (device, m_successors) {
        if (ignore == IgnoreGCap && device->GroundCap()) continue;
        if ((ignore == IgnoreGCCap) && (device->GroundCap() || device->CoupledCap())) continue;

        if (device->m_reverse) { // to successor's negative terminal
            Terminal *negTer = device->GetTerminal(Negative);
            if (HasConnectionIgnoreGnd(negTer, Positive))
                connectionsAtPos++;
            else if (HasConnectionIgnoreGnd(negTer, Negative))
                connectionsAtNeg++;
        } else { // to successor's positive terminal
            Terminal *posTer = device->GetTerminal(Positive);
            if (HasConnectionIgnoreGnd(posTer, Positive))
                connectionsAtPos++;
            else if (HasConnectionIgnoreGnd(posTer, Negative))
                connectionsAtNeg++;
        }
    }

    if (connectionsAtPos > connectionsAtNeg)
        m_reverse = true;
    else
        m_reverse = false;
}

bool Device::HasConnectionIgnoreGnd(Terminal *otherTer, TerminalType thisType)
{
    bool has = false;
    Terminal *thisTer = m_terminals[thisType];
    if (thisTer->NodeId() != 0 && otherTer->NodeId() != 0) {
        if (thisTer->NodeId() == otherTer->NodeId())
            has = true;
    }

    return has;
}

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

void Device::PrintBubbleValue() const
{
    printf("------------------------------\n");
    QString tmp = "";
    tmp += (m_name + " ");
    tmp += ("bubbleValue(" + QString::number(m_bubbleValue) + ")");
    qInfo() << tmp;
    printf("------------------------------\n");
}
