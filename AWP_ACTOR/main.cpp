#include "authorization.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Authorization w;
    w.show();
    return a.exec();
}
