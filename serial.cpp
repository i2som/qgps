#include "serial.h"

Serial::Serial(): QObject() {
    _serialPort = new QSerialPort(this);
    _serialPortInfo = new QSerialPortInfo();
    _serialBuffer = new QString;

    connect(_serialPort, SIGNAL(readyRead()), this, SLOT(readSerial()));
    connect(_serialPort, SIGNAL(error(QSerialPort::SerialPortError)), this, SLOT(handleSerialError(QSerialPort::SerialPortError)));
}

Serial::~Serial() {
    this->closeSerialPort();
    delete _serialPort;
    delete _serialPortInfo;
    delete _serialBuffer;
}

bool Serial::openSerialPort(QString portName,
                            QSerialPort::BaudRate baudRate,
                            QSerialPort::DataBits dataBits,
                            QSerialPort::Parity parity,
                            QSerialPort::StopBits stopBits,
                            QSerialPort::FlowControl flowControl) {
    this->closeSerialPort();
    _serialPort->setPortName(portName);
    _serialPort->setBaudRate(baudRate);
    _serialPort->setDataBits(dataBits);
    _serialPort->setParity(parity);
    _serialPort->setStopBits(stopBits);
    _serialPort->setFlowControl(flowControl);
    if (_serialPort->open(QIODevice::ReadWrite)) {
        qDebug() << "Serial port opened!";
        return true;
    } else {
        qDebug() << "Could not open Serial!";
        return false;
    }
}

void Serial::closeSerialPort() {
    if (_serialPort->isOpen()) {
        _serialPort->close();
        qDebug() << "Serial port closed!";
    }
}

void Serial::handleSerialError(QSerialPort::SerialPortError error) {
    if (error == QSerialPort::ResourceError) {
        this->closeSerialPort();
        qDebug() << "ERROR: Serial port Closed!";
    }
}

void Serial::readSerial() {
    _serialBuffer->append(_serialPort->readAll());
    if(_serialBuffer->size()) {
        parseSerialBuffer();
    }
}

void Serial::parseSerialBuffer() {
    qDebug() << "_serialBuffer: " << *_serialBuffer;
    if (!_serialBuffer->contains("\n"))
        return;

    QStringList msg_list = _serialBuffer->split("\n");
    foreach(QString str, msg_list) {
        if (str.contains("\r") && (str.size() > 6))
            emit serialReadyRead(str.toLocal8Bit());
    }
    QString last = msg_list.last();
    _serialBuffer->clear();
    if (!last.contains("\r"))
        _serialBuffer->append(last);
//    _serialBuffer->append(msg_list.last() + "\n");
    qDebug() << "_serialBuffer: " << *_serialBuffer;
}

QList<QSerialPortInfo> Serial::listSerialPortsAvailable() {
    return _serialPortInfo->availablePorts();
}

QString Serial::getSerialPortName() {
    return _serialPort->portName();
}

int Serial::getSerialPortBaudRate() {
    return _serialPort->baudRate();
}
