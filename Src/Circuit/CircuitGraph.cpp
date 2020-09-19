#include "CircuitGraph.h"
#include <regex>
#include <QDebug>
#include "Circuit/Device.h"
#include "Node.h"
#include "Terminal.h"

CircuitGraph::CircuitGraph()
{
    m_nodeNumber = 1;
    m_deviceNumber = 0;
    m_terminalNumber = 0;
}

CircuitGraph::~CircuitGraph()
{
    /* delete all devices, terminals here */
    foreach (Device *device, m_deviceList)
        delete device;
    Clear();
}

/* Just clear containers here, devices and terminals are destroyed in SchematicXXX */
void CircuitGraph::Clear()
{
    m_deviceTable.clear();
    m_deviceList.clear();
    m_firstLevelDeviceList.clear();

    // node will be hold by SchematicTerminal
    // DestroyAllNodes();
    m_nodeTable.clear();
}

/* Destroy all nodes here */
void CircuitGraph::DestroyAllNodes()
{
    Node *node = nullptr;
    foreach (node, m_nodeTable.values())
        delete node;
}

int CircuitGraph::InsertR(QString name, QString posName, QString negName, double value)
{
#ifdef TRACEx
    qInfo() << LINE_INFO << endl;
#endif

    if (m_deviceTable.contains(name)) {
        qInfo() << "ERROR: Redefine " << name << endl;
        return ERROR;
    }

    Device *r = new Device(RESISTOR, name);
    r->SetValue(value);
    Terminal *posTer = InsertNode(posName, r);
    Terminal *negTer = InsertNode(negName, r);
    r->AddTerminal(posTer, Positive);
    r->AddTerminal(negTer, Negative);
    r->SetId(m_deviceNumber);
    m_deviceNumber++;
    m_deviceTable.insert(name, r);
    m_deviceList.push_back(r);

    return OKAY;
}

int CircuitGraph::InsertC(QString name, QString posName, QString negName, double value)
{
#ifdef TRACEx
    qInfo() << LINE_INFO << endl;
#endif
    if (m_deviceTable.contains(name)) {
        qInfo() << "ERROR: Redefine " << name << endl;
        return ERROR;
    }

    Device *c = new Device(CAPACITOR, name);
    c->SetValue(value);
    Terminal *posTer = InsertNode(posName, c);
    Terminal *negTer = InsertNode(negName, c);
    c->AddTerminal(posTer, Positive);
    c->AddTerminal(negTer, Negative);
    c->SetId(m_deviceNumber);
    m_deviceNumber++;

    /* tag cap category */
    if (posTer->NodeIsGnd() || negTer->NodeIsGnd())
        c->SetAsGroundedCap(true);
    else
        c->SetAsGroundedCap(false);

    m_deviceTable.insert(name, c);
    m_deviceList.push_back(c);

    return OKAY;
}

int CircuitGraph::InsertL(QString name, QString posName, QString negName, double value)
{
#ifdef TRACEx
    qInfo() << LINE_INFO << endl;
#endif

    Device *l = new Device(INDUCTOR, name);
    l->SetValue(value);
    Terminal *posTer = InsertNode(posName, l);
    Terminal *negTer = InsertNode(negName, l);
    l->AddTerminal(posTer, Positive);
    l->AddTerminal(negTer, Negative);
    l->SetId(m_deviceNumber);
    m_deviceNumber++;
    m_deviceTable.insert(name, l);
    m_deviceList.push_back(l);
    
    return OKAY;
}

int CircuitGraph::InsertI(QString name, QString posName, QString negName, double value)
{
#ifdef TRACEx
    qInfo() << LINE_INFO << endl;
#endif

    Device *i = new Device(ISRC, name);
    i->SetValue(value);
    Terminal *posTer = InsertNode(posName, i);
    Terminal *negTer = InsertNode(negName, i);
    i->AddTerminal(posTer, Positive);
    i->AddTerminal(negTer, Negative);
    i->SetId(m_deviceNumber);
    m_deviceNumber++;

    /* tag isrc */
    if (posTer->NodeIsGnd() || negTer->NodeIsGnd())
        i->SetMaybeAtFirstLevel(true);

    m_deviceTable.insert(name, i);
    m_deviceList.push_back(i);

    return OKAY;
}

int CircuitGraph::InsertV(QString name, QString posName, QString negName, double value)
{
#ifdef TRACEx
    qInfo() << LINE_INFO << endl;
#endif

    Device *v = new Device(VSRC, name);
    v->SetValue(value);
    Terminal *posTer = InsertNode(posName, v);
    Terminal *negTer = InsertNode(negName, v);
    v->AddTerminal(posTer, Positive);
    v->AddTerminal(negTer, Negative);
    v->SetId(m_deviceNumber);
    m_deviceNumber++;

    /* tag vsrc */
    if (posTer->NodeIsGnd() || negTer->NodeIsGnd())
        v->SetMaybeAtFirstLevel(true);

    m_deviceTable.insert(name, v);
    m_deviceList.push_back(v);

    return OKAY;
}

Terminal* CircuitGraph::InsertNode(const QString &name, Device *device)
{
    Node *node = nullptr;
    NodeTable::const_iterator finder;
    finder = m_nodeTable.find(name);
    if (finder != m_nodeTable.constEnd()) {
        node = finder.value(); // found
    } else {
        node = new Node(name);
        if (IsGnd(name)) {
            node->SetId(0);
            node->SetGnd(true);
        } else {
            node->SetId(m_nodeNumber);
            m_nodeNumber++;
            node->SetGnd(false);
        }
        m_nodeTable.insert(name, node);
    }

    node->AddDevice(device);

    Terminal *terminal = CreateTerminal(node);
    terminal->SetDevice(device);
    return terminal;
}

Terminal* CircuitGraph::CreateTerminal(Node *node)
{
    Terminal *terminal = new Terminal(node);
    terminal->SetId(m_terminalNumber);
    m_terminalNumber++;
    return terminal;
}

bool CircuitGraph::IsGnd(const QString &name) const
{
    std::regex gndRex("^(0|gnd)$", std::regex::icase);
    bool ret = std::regex_match(name.toStdString(), gndRex);
    return ret;
}

Device* CircuitGraph::GetDevice(const QString &name) const
{
    return m_deviceTable[name];
}

void CircuitGraph::SetFirstLevelDeviceList(const DeviceList &devList)
{
    m_firstLevelDeviceList.clear();
    m_firstLevelDeviceList = devList;
}

void CircuitGraph::PrintCircuit() const
{
    Node *node = nullptr;
    Device *dev = nullptr;
    qInfo() << "\n--------------- Node ---------------";
    foreach (node, m_nodeTable.values()) {
        node->Print();
    }
    qInfo() << "------------------------------------\n";

    qInfo() << "--------------- Device -------------";
    foreach (dev, m_deviceList) {
        dev->Print();
    }
    qInfo() << "------------------------------------\n";
}

