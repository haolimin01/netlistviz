#include "ASG.h"
#include <QDebug>
#include <QString>
#include "Matrix.h"
#include "MatrixElement.h"
#include "TablePlotter.h"
#include "Circuit/Device.h"
#include "Circuit/CircuitGraph.h"
#include "Level.h"
#include "Channel.h"


ASG::ASG(CircuitGraph *ckt)
{
    Q_ASSERT(ckt);
    m_ckt = ckt;
    m_matrix = nullptr;
    m_levelsPlotter = nullptr;
    m_logDataDestroyed = false;
    m_ignoreCap = IgnoreGCap;

    m_visited = new int[m_ckt->DeviceCount()];  
    memset(m_visited, 0, sizeof(int) * m_ckt->DeviceCount());
}

ASG::ASG()
{
    m_ckt = nullptr;
    m_matrix = nullptr;
    m_visited = nullptr;
    m_levelsPlotter = nullptr;
    m_logDataDestroyed = false;
    m_ignoreCap = IgnoreGCap;
}

ASG::~ASG()
{
#ifdef TRACE
    qInfo() << LINE_INFO << endl;
#endif
    if (m_matrix) delete m_matrix;
    delete []m_visited;
    
    foreach (Level *level, m_levels)
        delete level;
    m_levels.clear();

    if (m_levelsPlotter)
        delete m_levelsPlotter;

    foreach (Channel *channel, m_channels)
        delete channel;
    m_channels.clear();

    m_sdeviceList.clear();
    m_swireList.clear();
}

void ASG::SetCircuitgraph(CircuitGraph *ckt)
{
    Q_ASSERT(ckt);
    m_ckt = ckt;

    if (m_matrix) {
        delete m_matrix;
        m_matrix = nullptr;
    }

    if (m_visited) delete []m_visited;
    m_visited = new int[m_ckt->DeviceCount()];
    memset(m_visited, 0, sizeof(int) * m_ckt->DeviceCount());

    m_logDataDestroyed = false;
}


void ASG::DestroyLogicalData()
{
    /* devices and terminals */
    if (m_ckt) {
        delete m_ckt;
        m_ckt = nullptr;
    }

    /* channels and wires */
    foreach (Channel *ch, m_channels)
        delete ch;
    m_channels.clear();

    /* levels */
    foreach (Level *level, m_levels)
        delete level;
    m_levels.clear();

    /* Matrix and it's elements */
    if (m_matrix) {
        delete m_matrix;
        m_matrix = nullptr;
    }

    m_logDataDestroyed = true;
}


/* Print and Plot */
void ASG::PlotLevels(const QString &title)
{
    if (m_levels.size() < 1)
        return;
    
    if (m_levelsPlotter) {
        m_levelsPlotter->close();
        m_levelsPlotter->clear();
    } else {
        m_levelsPlotter = new TablePlotter();
    }

    int maxDeviceCountInLevel = -1;
    foreach (Level *level, m_levels) {
        if (level->AllDeviceCount() > maxDeviceCountInLevel)
            maxDeviceCountInLevel = level->AllDeviceCount();
    }

    m_levelsPlotter->SetTableRowColCount(maxDeviceCountInLevel, m_levels.size());

    /* header */
    QStringList headerText;
    for (int i = 0; i < m_levels.size(); ++ i) {
        QString tmp = "L" + QString::number(m_levels.at(i)->Id());
        headerText << tmp;
    }
    m_levelsPlotter->SetColHeaderText(headerText);

    /* content */
    int row = 0;
    for (int i = 0; i < m_levels.size(); ++ i) {
        Level *level = m_levels.at(i);
        row = 0;
        foreach (Device *dev, level->AllDevices()) {
            m_levelsPlotter->AddItem(row, i, dev->Name());
            row++;
        }
    }

    m_levelsPlotter->setWindowTitle(title);
    m_levelsPlotter->Display();
}
