#include "mainwindow.h"
#include "users.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //MainWindow w(1);

    //w.show();

    Users u;
    u.show();

    return a.exec();
}
