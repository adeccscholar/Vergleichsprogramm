#include "DatabaseApp.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    DatabaseApp w;
    w.show();
    return a.exec();
}
