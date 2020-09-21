#ifndef NETLISTVIZ_ASG_TABLEPLOTTER_H
#define NETLISTVIZ_ASG_TABLEPLOTTER_H

/*
 * @filename : TablePlotter.h
 * @author   : Hao Limin 
 * @date     : 2020.07.30
 * @email    : haolimin01@aice.sjtu.edu.cn
 * @desp     : Plot Table for Matrix
 * @modified : Haolimin, 2020.09.12
 */

#include <QTableWidget>
#include <QGraphicsScene>
#include <QWheelEvent>
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

protected:
    void wheelEvent(QWheelEvent *event) override;

private:
    DISALLOW_COPY_AND_ASSIGN(TablePlotter);

    int m_rowCount;
    int m_colCount;

};

#endif // NETLISTVIZ_ASG_TABLEPLOTTER_H