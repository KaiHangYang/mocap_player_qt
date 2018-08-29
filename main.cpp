#include "mMainWindow.h"
#include <QApplication>
#include <QDebug>

#include "mRenderParameters.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    mMainWindow w(NULL, mRenderParams::mWindowWidth, mRenderParams::mWindowHeight, "Mocap Data Generator");
    w.show();
    return a.exec();
}
