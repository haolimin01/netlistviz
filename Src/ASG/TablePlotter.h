#ifndef NETLISTVIZ_ASG_TABLEPLOTTER_H
#define NETLISTVIZ_ASG_TABLEPLOTTER_H

#include <QTableWidget>
#include "Define/Define.h"

class Matrix;
class QTableWidget;
class QWidget;

class TablePlotter : public QTableWidget
{
public:
    explicit TablePlotter(QWidget *parent = nullptr);
    ~TablePlotter();

    void SetTableRowColCount(int rowCount, int colCount);
    void SetRowHeaderText(const QStringList &stringList);
    void SetColHeaderText(const QStringList &stringList);
    void AddItem(int row, int col);
    void AddItem(int row, int col, const QString &text);

    void Display();

    void Clear();

private:
    DISALLOW_COPY_AND_ASSIGN(TablePlotter);

    int m_rowCount;
    int m_colCount;
};

#endif // NETLISTVIZ_ASG_TABLEPLOTTER_H