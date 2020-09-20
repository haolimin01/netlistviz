#include "Level.h"
#include <QDebug>
#include "Circuit/Device.h"

Level::Level(int id)
{
    m_id = id;
    m_deviceCountWithoutCap = 0;
    m_capCount = 0;
}

Level::Level()
{
    m_id = -1;
    m_deviceCountWithoutCap = 0;
    m_capCount = 0;
}

Level::~Level()
{
    m_deviceList.clear();
}

void Level::AddDevice(Device *device)
{
    if (device->GetDeviceType() == CAPACITOR)
        m_capCount++;
    else
        m_deviceCountWithoutCap++;
    
    device->SetLevelId(m_id);
    m_deviceList.push_back(device);
}

void Level::AddDevices(const DeviceList &devList)
{
    foreach (Device *dev, devList)
        AddDevice(dev);
}

void Level::SetId(int id)
{
    m_id = id;
    foreach (Device *dev, m_deviceList) {
        dev->SetLevelId(m_id);
    }
}

int Level::CapCount() const
{
    return m_capCount;
}

int Level::DeviceCountWithoutCap() const
{
    return m_deviceCountWithoutCap;
}

int Level::AllDeviceCount() const
{
    return m_capCount + m_deviceCountWithoutCap;
}

DeviceList Level::AllDevices() const
{
    return m_deviceList;
}

DeviceList Level::AllDevicesWithoutCap() const
{
    DeviceList result;
    foreach (Device *dev, m_deviceList) {
        if (dev->GetDeviceType() != CAPACITOR)
            result.push_back(dev);
    }
    return result;
}

DeviceList Level::Devices(DeviceType type) const
{
    DeviceList result;
    foreach (Device *dev, m_deviceList) {
        if (dev->GetDeviceType() == type)
            result.push_back(dev);
    }
    return result;
}

void Level::AssignRowNumberByBubbleValue()
{
    /* sort device by bubble value */
    qSort(m_deviceList.begin(), m_deviceList.end(),
        [](Device *a, Device *b){ return a->BubbleValue() < b->BubbleValue(); });

#ifdef DEBUGx
    qInfo() << "Level" << m_id;
    foreach (Device *dev, m_deviceList)
        dev->PrintBubbleValue();
#endif
    
    /* We consider bubble value as device row now */
    int maxRow = -1, row = -1;
    int bubbleValue = -1;
    foreach (Device *dev, m_deviceList) {
        bubbleValue = dev->BubbleValue();
        if (bubbleValue > maxRow) {
            row = bubbleValue;
            maxRow = row;
        } else {
            row = maxRow + 1;
            maxRow++;
        }
    
        dev->SetRow(row);
    }
}

void Level::PrintAllDevices() const
{
    printf("---------------- Level %d ---------------\n", m_id);

    foreach (Device *dev, m_deviceList)
        qInfo() << dev->Name();

    printf("----------------------------------------\n");
}

void Level::PrintAllConnections() const
{
    printf("---------------- Level %d ---------------\n", m_id);

    QString result("");

    foreach (Device *dev, m_deviceList) {
        result += (dev->Name() + " ");
        result += ("predecessors( ");
        foreach (Device *predecessor, dev->Predecessors())
            result += (predecessor->Name() + " ");
        result += (" ), ");
        result += ("successors( ");
        foreach (Device *successor, dev->Successors())
            result += (successor->Name() + " ");
        result += (" )");
        qInfo() << result;
        result = "";
    }

    printf("----------------------------------------\n");
}
