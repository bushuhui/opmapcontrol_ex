#include <QApplication>
#include <QWidget>

#include <QtCore>
#include <QtGui>

#include "MapWidget_TestWindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTestWin w;

    w.show();

    return a.exec();
}
