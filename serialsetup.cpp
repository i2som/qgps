#include "serialsetup.h"
#include "ui_serialsetup.h"

serialSetup::serialSetup(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::serialSetup)
{
    ui->setupUi(this);


    Serial serial;
    QList<QSerialPortInfo> availablePorts = serial.listSerialPortsAvailable();
    for(int index = 0; index < availablePorts.size(); index++) {
        ui->portName->addItem(availablePorts.at(index).portName(), QString(availablePorts.at(index).portName()));
    }

    ui->baudRate->addItem(QString::number(QSerialPort::Baud1200), QSerialPort::Baud1200);
    ui->baudRate->addItem(QString::number(QSerialPort::Baud2400), QSerialPort::Baud2400);
    ui->baudRate->addItem(QString::number(QSerialPort::Baud4800), QSerialPort::Baud4800);
    ui->baudRate->addItem(QString::number(QSerialPort::Baud9600), QSerialPort::Baud9600);
    ui->baudRate->addItem(QString::number(QSerialPort::Baud19200), QSerialPort::Baud19200);
    ui->baudRate->addItem(QString::number(QSerialPort::Baud38400), QSerialPort::Baud38400);
    ui->baudRate->addItem(QString::number(QSerialPort::Baud57600), QSerialPort::Baud57600);
    ui->baudRate->addItem(QString::number(QSerialPort::Baud115200), QSerialPort::Baud115200);
    ui->baudRate->setCurrentIndex(7);

    ui->dataBits->addItem(QString::number(QSerialPort::Data5), QSerialPort::Data5);
    ui->dataBits->addItem(QString::number(QSerialPort::Data6), QSerialPort::Data6);
    ui->dataBits->addItem(QString::number(QSerialPort::Data7), QSerialPort::Data7);
    ui->dataBits->addItem(QString::number(QSerialPort::Data8), QSerialPort::Data8);
    ui->dataBits->setCurrentIndex(3);

    ui->parity->addItem(QString("No Parity"), QSerialPort::NoParity);
    ui->parity->addItem(QString("Even Parity"), QSerialPort::EvenParity);
    ui->parity->addItem(QString("Odd Parity"), QSerialPort::OddParity);
    ui->parity->addItem(QString("Space Parity"), QSerialPort::SpaceParity);
    ui->parity->addItem(QString("Mark Parity"), QSerialPort::MarkParity);

    ui->stopBits->addItem(QString("One"), QSerialPort::OneStop);
    ui->stopBits->addItem(QString("One And a Half"), QSerialPort::OneAndHalfStop);
    ui->stopBits->addItem(QString("Two"), QSerialPort::TwoStop);

    ui->flowControl->addItem(QString("No Flow Control"), QSerialPort::NoFlowControl);
    ui->flowControl->addItem(QString("Hardware Control"), QSerialPort::HardwareControl);
    ui->flowControl->addItem(QString("Software Control"), QSerialPort::SoftwareControl);

    connect(ui->portName, SIGNAL(currentIndexChanged(int)), this, SLOT(update_serial(int)));
    connect(ui->baudRate, SIGNAL(currentIndexChanged(int)), this, SLOT(update_serial(int)));
    connect(ui->dataBits, SIGNAL(currentIndexChanged(int)), this, SLOT(update_serial(int)));
    connect(ui->parity, SIGNAL(currentIndexChanged(int)), this, SLOT(update_serial(int)));
    connect(ui->stopBits, SIGNAL(currentIndexChanged(int)), this, SLOT(update_serial(int)));
    connect(ui->flowControl, SIGNAL(currentIndexChanged(int)), this, SLOT(update_serial(int)));


    QObject::connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    QObject::connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));


    portName = ui->portName->currentData().toString();
    baudRate = ui->baudRate->currentData().toInt();
    dataBits = ui->dataBits->currentData().toInt();
    parity = ui->parity->currentData().toInt();
    stopBits = ui->stopBits->currentData().toInt();
    flowControl =ui->flowControl->currentData().toInt();
}

serialSetup::~serialSetup()
{
    delete ui;
}
void serialSetup::update_serial(int index)
{
    qDebug() << index;
    portName = ui->portName->currentData().toString();
    baudRate = ui->baudRate->currentData().toInt();
    dataBits = ui->dataBits->currentData().toInt();
    parity = ui->parity->currentData().toInt();
    stopBits = ui->stopBits->currentData().toInt();
    flowControl =ui->flowControl->currentData().toInt();
}
