#include "mainlojaqt.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainLojaQT w;
    w.show();
    return a.exec();
}
