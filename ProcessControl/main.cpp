#include "mainwindow.h"
#include <QtWidgets/QApplication>
#include <QUrl>

int main(int argc, char *argv[])
{
    ProcController myProcController;
    QApplication a(argc, argv);
    MainWindow w;
    w.setMinimumSize(250, 300);
    w.show();

    return a.exec();
}