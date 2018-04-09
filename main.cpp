#include "mMainWindow.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    mMainWindow w;
    w.show();

    return a.exec();
}
