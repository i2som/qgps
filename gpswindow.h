#ifndef GPSWINDOW_H
#define GPSWINDOW_H

#include <QMainWindow>
#include "gps.h"
#include "serial.h"
#include <QTime>

namespace Ui {
class GpsWindow;
}

class GpsWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit GpsWindow(QWidget *parent = 0);
    ~GpsWindow();
    int _selectedSateliteId;
    int _receivedPackages;

    GPS *_gps;
    Serial *_serial;

    Serial *setupSerialPortDialog();
    void updateStatusBar();

private slots:
    void on__sateliteList_itemSelectionChanged();

public Q_SLOTS:
    void updateSateliteInfo();
    void showGPS(QByteArray gpsPackage);
    void startSerial();
    void stop();

private:
    Ui::GpsWindow *ui;
};

#endif // GPSWINDOW_H
