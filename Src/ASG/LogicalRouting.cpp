#include "ASG.h"
#include <QDebug>
#include "Channel.h"
#include "Level.h"
#include "Circuit/Device.h"

int ASG::LogicalRouting()
{
#ifdef TRACE
    qInfo() << LINE_INFO << endl;
#endif

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

    Level *level = nullptr;

    for (int i = 1; i < m_levels.size(); ++ i) {
        level = m_levels.at(i);

        // new channel
        newChannel = new Channel(channelIndex);

        foreach (dev, level->AllDevices()) {
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
