#include "_AR_UI_.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    _AR_UI_ w;
    w.show();
    return a.exec();
}
