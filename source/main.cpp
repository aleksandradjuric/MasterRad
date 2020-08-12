#include "domen.h"
#include "scena.h"
#include "widget.h"
#include <QSize>
#include <QApplication>
#include <QDesktopWidget>

using namespace std;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.setMinimumSize(900,640);
    w.move(QApplication::desktop()->screen()->rect().center() - w.rect().center());
    w.setWindowTitle("Master rad");
    w.show();

    return a.exec();
}
