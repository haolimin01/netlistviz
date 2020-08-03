#include "TablePlotter.h"
#include <QDebug>
#include <QHeaderView>

#define tr QObject::tr
static const int Table_W = 50;
static const int Table_H = 50;


TablePlotter::TablePlotter(QWidget *parent) : QTableWidget(parent)
{
    m_rowCount = 0;
    m_colCount = 0;

    setWindowTitle(tr("Incidence Matrix"));
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    resize(1200, 900);
}

TablePlotter::~TablePlotter()
{
    if (m_rowCount * m_colCount > 0)
        clear();
}

void TablePlotter::SetTableRowColCount(int rowCount, int colCount)
{
    Q_ASSERT(rowCount > 0 && colCount > 0);
    setRowCount(rowCount);
    setColumnCount(colCount);

    m_rowCount = rowCount;
    m_colCount = colCount;

    for (int i = 0; i < m_colCount; ++ i)
        setColumnWidth(i, Table_W);
    
    for (int i = 0; i < m_rowCount; ++ i)
        setRowHeight(i, Table_H);
}

void TablePlotter::SetRowHeaderText(const QStringList &stringList)
{
    Q_ASSERT(stringList.size() == m_rowCount);
    setVerticalHeaderLabels(stringList);
}

void TablePlotter::SetColHeaderText(const QStringList &stringList)
{
    Q_ASSERT(stringList.size() == m_colCount);
    setHorizontalHeaderLabels(stringList);
}

void TablePlotter::AddItem(int row, int col)
{
    QTableWidgetItem *item = new QTableWidgetItem;
    item->setBackgroundColor(Qt::black);
    setItem(row, col, item);
}

void TablePlotter::AddItem(int row, int col, const QString &text)
{
    QTableWidgetItem *item = new QTableWidgetItem;
    item->setText(text);
    item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    setItem(row, col, item);
}

void TablePlotter::Display()
{
    show();
}

void TablePlotter::Clear()
{
    if (m_rowCount * m_colCount > 0)
        clear();
}