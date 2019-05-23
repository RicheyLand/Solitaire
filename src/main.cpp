#include "klondike.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Klondike * w = new Klondike;                    //  allocate memory for created instance
    w->show();                                      //  show appropriate content

    return a.exec();
}
