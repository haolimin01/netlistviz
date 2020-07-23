#include "SchematicData.h"
#include <regex>
#include <QDebug>
#include "SchematicNode.h"
#include "SchematicDevice.h"
#include "SchematicTextItem.h"


SchematicData::SchematicData()
{
    m_nodeNumber = 1;
}


SchematicData::~SchematicData()
{
    for (int i = 0; i < m_nodeList.size(); ++ i) {
        delete m_nodeList.at(i);
    }

    for (int i = 0; i < m_deviceList.size(); ++ i) {
        delete m_deviceList.at(i);
    }

    m_nodeList.clear();
    m_deviceList.clear();

    m_nodeTable.clear();
    m_deviceTable.clear();
}


void SchematicData::ParseC(QString name, QString posName, QString negName, double value)
{
    if (m_deviceTable.contains(name)) {
        qInfo() << "ERROR: Redefine " << name << endl;
        EXIT;
    }

    SchematicNode *posNode = GetAddNode(posName);
    SchematicNode *negNode = GetAddNode(negName);
    SchematicDevice *c = new SchematicDevice(SchematicDevice::Capacitor, nullptr);
    c->SetName(name);
    c->SetValue(value);
    c->SetPosNode(posNode);
    c->SetNegNode(negNode);

    posNode->AddDevice(c);
    negNode->AddDevice(c);

    m_deviceTable.insert(name, c);
    m_deviceList.push_back(c);
}


void SchematicData::ParseI(QString name, QString posName, QString negName, double value)
{
    if (m_deviceTable.contains(name)) {
        qInfo() << "ERROR: Redefine " << name << endl;
        EXIT;
    }

    SchematicNode *posNode = GetAddNode(posName);
    SchematicNode *negNode = GetAddNode(negName);
    SchematicDevice *i = new SchematicDevice(SchematicDevice::Isrc, nullptr);
    i->SetName(name);
    i->SetValue(value);
    i->SetPosNode(posNode);
    i->SetNegNode(negNode);

    posNode->AddDevice(i);
    negNode->AddDevice(i);

    m_deviceTable.insert(name, i);
    m_deviceList.push_back(i);
}


void SchematicData::ParseL(QString name, QString posName, QString negName, double value)
{
    if (m_deviceTable.contains(name)) {
        qInfo() << "ERROR: Redefine " << name << endl;
        EXIT;
    }

    SchematicNode *posNode = GetAddNode(posName);
    SchematicNode *negNode = GetAddNode(negName);
    SchematicDevice *l = new SchematicDevice(SchematicDevice::Inductor, nullptr);
    l->SetName(name);
    l->SetValue(value);
    l->SetPosNode(posNode);
    l->SetNegNode(negNode);

    posNode->AddDevice(l);
    negNode->AddDevice(l);

    m_deviceTable.insert(name, l);
    m_deviceList.push_back(l);
}


void SchematicData::ParseR(QString name, QString posName, QString negName, double value)
{
    if (m_deviceTable.contains(name)) {
        qInfo() << "ERROR: Redefine " << name << endl;
        EXIT;
    }

    SchematicNode *posNode = GetAddNode(posName);
    SchematicNode *negNode = GetAddNode(negName);
    SchematicDevice *r = new SchematicDevice(SchematicDevice::Resistor, nullptr);
    r->SetName(name);
    r->SetValue(value);
    r->SetPosNode(posNode);
    r->SetNegNode(negNode);

    posNode->AddDevice(r);
    negNode->AddDevice(r);

    m_deviceTable.insert(name, r);
    m_deviceList.push_back(r);
}


void SchematicData::ParseV(QString name, QString posName, QString negName, double value)
{
    if (m_deviceTable.contains(name)) {
        qInfo() << "ERROR: Redefine " << name << endl;
        EXIT;
    }

    SchematicNode *posNode = GetAddNode(posName);
    SchematicNode *negNode = GetAddNode(negName);
    SchematicDevice *v = new SchematicDevice(SchematicDevice::Vsrc, nullptr);
    v->SetName(name);
    v->SetValue(value);
    v->SetPosNode(posNode);
    v->SetNegNode(negNode);

    posNode->AddDevice(v);
    negNode->AddDevice(v);

    m_deviceTable.insert(name, v);
    m_deviceList.push_back(v);
}


SchematicNode* SchematicData::GetAddNode(const QString &name)
{
    NodeTable::const_iterator finder;
    finder = m_nodeTable.find(name);
    if (finder != m_nodeTable.constEnd()) {
        return finder.value();
    }

    SchematicNode *node = new SchematicNode();
    node->SetName(name);
    
    if (IsGnd(name)) {
        node->SetId(0);
        m_nodeList.push_front(node);
        node->SetGnd(true);
    } else {
        node->SetId(m_nodeNumber);
        m_nodeNumber++;
        m_nodeList.push_back(node);
    }

    m_nodeTable.insert(name, node);

    return node;
}


bool SchematicData::IsGnd(const QString &name) const
{
    std::regex gndRex("^(0|gnd)$", std::regex::icase);
    bool ret = std::regex_match(name.toStdString(), gndRex);
    return ret;
}


void SchematicData::PrintNodeAndDevice() const
{
    SchematicNode *node = nullptr;
    SchematicDevice *dev = nullptr;
    qInfo() << "\n--------------- Node ---------------\n";
    for (int i = 0; i < m_nodeList.size(); ++ i) {
        node = m_nodeList.at(i);
        node->Print();
    }
    qInfo() << "------------------------------------\n";

    qInfo() << "-------------- Device --------------\n";
    for (int i = 0; i < m_deviceList.size(); ++ i) {
        dev = m_deviceList.at(i);
        dev->Print();
    }
    qInfo() << "------------------------------------\n";
}
