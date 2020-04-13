#ifndef SERIALSETUP_H
#define SERIALSETUP_H

#include <QDialog>
#include "serial.h"

namespace Ui {
class serialSetup;
}

class serialSetup : public QDialog
{
    Q_OBJECT

public:
    explicit serialSetup(QWidget *parent = 0);
    ~serialSetup();

    QString portName;
    int baudRate;
    int dataBits;
    int parity;
    int stopBits;
    int flowControl;

private:
    Ui::serialSetup *ui;
private slots:
    void update_serial(int);
};

#endif // SERIALSETUP_H
