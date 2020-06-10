/*
 * @project  : netlistviz
 * @filename : Main.h
 * @author   : Hao Limin
 * @date     : 2020-06-10
 * @email    : haolimin01@sjtu.edu.cn
 */

#include "Schematic/MainWindow.h"

#include <QApplication>


int main(int argv, char *args[])
{
    Q_INIT_RESOURCE(Schematic);

    QApplication app(argv, args);
    MainWindow mainWindow;
    mainWindow.setGeometry(100, 100, 800, 500);
    mainWindow.show();

    return app.exec();
}
