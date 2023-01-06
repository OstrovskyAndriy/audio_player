#include "users.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Users u;
    u.show();

    return a.exec();
}
