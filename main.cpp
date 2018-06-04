#include "mMainWindow.h"
#include <QApplication>
#include <QDebug>

#include "mRenderParameters.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    mMainWindow w(NULL, mWindowWidth, mWindowHeight);
    w.show();
    return a.exec();
}
