#include "gpswindow.h"
#include "ui_gpswindow.h"
#include "serialsetup.h"
#include <QInputDialog>
#include <QDebug>

GpsWindow::GpsWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GpsWindow)
{
    ui->setupUi(this);

    _gps = new GPS;
    _serial = NULL;
    _receivedPackages = 0;

    connect(ui->actionSTART_FROM_SERIAL, SIGNAL(triggered()), this, SLOT(startSerial()));
    connect(ui->actionSTOP, SIGNAL(triggered()), this, SLOT(stop()));
    connect(ui->actionQUIT, SIGNAL(triggered()), this, SLOT(close()));
    ui->_console->document()->setMaximumBlockCount(100);
}

GpsWindow::~GpsWindow()
{
    delete ui;
}


void GpsWindow::showGPS(QByteArray gpsPackage) {
    //qDebug() << "showGPS: " << QString(gpsPackage);
    _gps->parseGPS(gpsPackage);

    if(_gps->_gpgga != NULL) {
        ui->_latitude->display(_gps->_gpgga->_latitude);
        ui->_longitude->display(_gps->_gpgga->_longitude);
        ui->_numeroSatelites->display(_gps->_gpgga->_satelites);
        ui->_altitude->display(_gps->_gpgga->_altitude);
        int hours = (int) _gps->_gpgga->_utcTime / 10000 % 100;
        int minutes = (int) _gps->_gpgga->_utcTime / 100 % 100;
        int seconds = (int) _gps->_gpgga->_utcTime % 100;
        ui->_horaPacote->display(QTime(hours, minutes, seconds).toString(QString("hh:mm:ss")));
        ui->_precisaoPacote->display(_gps->_gpgga->_hdop);
    }
    if(_gps->_gprmc != NULL) {
        //qDebug() << _gps->_gprmc->_utDate;
        int day = _gps->_gprmc->_utDate / 10000 % 100;
        int month = _gps->_gprmc->_utDate / 100 % 100;
        int year = _gps->_gprmc->_utDate % 100 + 2000;
        ui->_dataPacote->display(QDate(year, month, day).toString(QString("yyyy-MM-dd")));
        ui->_velocidade->display(_gps->_gprmc->_speed);
    }
    if(_gps->_gpgsa != NULL) {
        ui->_sateliteList->clear();
        ui->_sateliteInfo->clear();
        for(int index = 0; index < _gps->_gpgsa->_satelites->size(); index++) {
            ui->_sateliteList->addItem("SATELLITE NUMBER " + QString::number(_gps->_gpgsa->_satelites->at(index)));
        }
        if(!ui->_sateliteList->size().isEmpty()) {
            for(int index = 0; index < _gps->_gpgsa->_satelites->size(); index++) {
                if(_selectedSateliteId == _gps->_gpgsa->_satelites->at(index)) {
                    ui->_sateliteList->setCurrentRow(index);
                    break;
                }
            }
        }
    }
    ui->_console->append(gpsPackage);
    this->updateStatusBar();
}

void GpsWindow::updateSateliteInfo() {
    if(_gps->_gpgsv != NULL && !ui->_sateliteList->size().isEmpty()) {
        for(int index = 0; index < _gps->_gpgsv->_satelites->size(); index++) {
            if(ui->_sateliteList->selectedItems().size()) {
                _selectedSateliteId = _gps->_gpgsa->_satelites->at(ui->_sateliteList->row(ui->_sateliteList->selectedItems().at(0)));
            }
            if(_gps->_gpgsv->_satelites->at(index)._id == _selectedSateliteId) {
                SATELITE satelite = _gps->_gpgsv->_satelites->at(index);
                ui->_sateliteInfo->setText(QString("ID = " + QString::number(satelite._id) + "\nAltitude = " +
                       QString::number(satelite._elevation) + "° (Max. 90°)\n" + "Azimuth = " + QString::number(satelite._azimuth) +
                       "° (Max. 359°)\n" + "SNR = " + QString::number(satelite._snr) + " dB\n"));
                break;
            }
        }
    }
}

Serial *GpsWindow::setupSerialPortDialog() {
    serialSetup dialog;
    if (dialog.exec() == QDialog::Accepted) {
        Serial *serial = new Serial;
        qDebug() << dialog.portName;
        qDebug() << dialog.baudRate;
        qDebug() << dialog.dataBits;
        qDebug() << dialog.parity;
        qDebug() << dialog.stopBits;
        qDebug() << dialog.flowControl;
        bool ret = serial->openSerialPort(dialog.portName,
                                       static_cast<QSerialPort::BaudRate>(dialog.baudRate),
                                       static_cast<QSerialPort::DataBits>(dialog.dataBits),
                                       static_cast<QSerialPort::Parity>(dialog.parity),
                                       static_cast<QSerialPort::StopBits>(dialog.stopBits),
                                       static_cast<QSerialPort::FlowControl>(dialog.flowControl));
        if (ret)
            return serial;
    }
    return NULL;
}

void GpsWindow::startSerial() {
    if(_serial == NULL) {
        _serial = this->setupSerialPortDialog();
        if(_serial != NULL) {
            QObject::connect(_serial, SIGNAL(serialReadyRead(QByteArray)), this, SLOT(showGPS(QByteArray)));
        }
    }
    this->updateStatusBar();
}

void GpsWindow::stop() {
    if(_serial != NULL) {
        delete _serial;
        _serial = NULL;
    }
    this->updateStatusBar();
}

void GpsWindow::updateStatusBar() {
    if(_serial == NULL) {
        this->statusBar()->showMessage(QString("Disconnected"));
    } else {
        QString str = QString("Connected: ");
        if(_serial != NULL) {
            str.append(QString("Serial ") + _serial->getSerialPortName() + QString(" at ") +
                        QString::number(_serial->getSerialPortBaudRate()) + QString(" bps; "));
        }
        str.append(QString("Package count: ") + QString::number(++_receivedPackages));
        this->statusBar()->showMessage(str);
    }
}

void GpsWindow::on__sateliteList_itemSelectionChanged()
{
    qDebug() << "on__sateliteList_itemSelectionChanged";
    updateSateliteInfo();
}
