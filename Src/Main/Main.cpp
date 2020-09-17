/*
 * @project  : netlistviz
 * @filename : Main.h
 * @author   : Hao Limin
 * @date     : 2020-06-10
 * @email    : haolimin01@sjtu.edu.cn
 */

#include "MainWindow.h"

#include <QApplication>
#include <QDesktopWidget>


int main(int argv, char *args[])
{
    Q_INIT_RESOURCE(Schematic);

    QApplication app(argv, args);
    MainWindow mainWindow;

    /* Get screen size */
    QRect rect = QApplication::desktop()->availableGeometry(); 
    mainWindow.setGeometry(0, 0, rect.width(), rect.height());

    mainWindow.show();

    return app.exec();
}
