#include "ASG.h"
#include <QDebug>
#include "Channel.h"
#include "Level.h"
#include "Circuit/Device.h"
#include "HyperLevel.h"

int ASG::LogicalRouting()
{
#ifdef TRACE
    qInfo() << LINE_INFO << endl;
#endif

    /* Now we just consider channels, extra wires in HyperLevel are ignored */
    int error = CreateChannels();
    if (error)
        return ERROR;
    
    error = AssignTrackNumber();
    if (error)
        return ERROR;

#ifdef DEBUG
    foreach (Channel *ch, m_channels)
        ch->Print();
#endif

    return OKAY;
}

/*
 * Level0     Level1     Level2  ...  Level(n)
 *         |          |          |
 *     Channel0   channel1  Channel(n-1)
 */
int ASG::CreateChannels()
{
#ifdef TRACE
    qInfo() << LINE_INFO << endl;
#endif

    m_channels.clear();

    int channelIndex = 0;
    Channel *newChannel = nullptr;
    Device *dev = nullptr;

    foreach (HyperLevel *hl, m_hyperLevels) {
        newChannel = hl->CreateChannel();
        if (newChannel) {
            newChannel->SetId(channelIndex);
            channelIndex++;
            m_channels.push_back(newChannel);
        }
    }

    if (m_hyperLevels.size() < 2)
        return OKAY;

    HyperLevel *hl = nullptr;

    for (int i = 1; i < m_hyperLevels.size(); ++ i) {
        hl = m_hyperLevels.at(i);

        // new channel
        newChannel = new Channel(channelIndex);

        foreach (dev, hl->AllDevices()) {
            newChannel->AddWires(dev->WiresFromPredecessors());
        }

        m_channels.push_back(newChannel);
        channelIndex++;
    }

#ifdef DEBUG
    foreach (Channel *ch, m_channels)
        ch->Print();
#endif

    return OKAY;
}

int ASG::AssignTrackNumber()
{
#ifdef TRACE
    qInfo() << LINE_INFO << endl;
#endif

    foreach (Channel *ch, m_channels) {
        ch->AssignTrackNumber(m_ignoreCap);
    }

    return OKAY;
}
