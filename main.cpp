#include "gpswindow.h"
#include <QApplication>
#include <QLoggingCategory>
#include <QTcpSocket>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    QLoggingCategory::defaultCategory()->setEnabled(QtDebugMsg, true);

    GpsWindow *w = new GpsWindow;
//    w->setMinimumSize(1024, 600);
    w->show();

    return a.exec();
}
