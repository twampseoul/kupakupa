#include "kupagui.h"
#include <QApplication>
#include <stdio.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    kupagui w;
    w.show();

    return a.exec();
}
