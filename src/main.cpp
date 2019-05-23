#include "klondike.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Klondike * w = new Klondike;
    w->show();

    return a.exec();
}
