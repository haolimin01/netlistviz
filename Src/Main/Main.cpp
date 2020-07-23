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
    app.setAttribute(Qt::AA_DontUseNativeMenuBar);
    MainWindow mainWindow;
    mainWindow.setGeometry(100, 100, 1200, 900);
    mainWindow.show();

    return app.exec();
}
