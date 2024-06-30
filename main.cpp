#include "hhgen.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    hhgen w;
    w.show();
    return a.exec();
}
