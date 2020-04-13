#include "gps.h"


GPGGA::GPGGA(){}
GPGSA::GPGSA(){}
SATELITE::SATELITE(){}
GPGSV::GPGSV(){}
GPRMC::GPRMC(){}

GPS::GPS(){}
GPS::~GPS()
{

    if (_gpgga != NULL) {
        delete _gprmc;
    }
    if (_gpgsa != NULL) {
        delete _gprmc;
    }
    if (_gpgsv != NULL) {
        if (_gpgsv->_satelites != NULL)
            delete _gpgsv->_satelites;
        delete _gprmc;
    }
    if (_gprmc != NULL) {
        delete _gprmc;
    }
}

bool GPS::checkSum(QByteArray package, int checksum) {
    int sum = 0;
    if(package.size() && package.at(0) == '$') {
        for(int index = 1; package.at(index) != '*' && index < package.size(); index++) {
            sum ^= package.at(index);
        }
        if(sum == checksum) {
            return true;
        }
    }
    return false;
}

QList<QByteArray> *GPS::splitPackage(QByteArray package, QString packageName) {
    QList<QByteArray> splitChecksum = package.split('*');
    if(splitChecksum.size() < 2 || splitChecksum.at(1).size() < 2) {
        qDebug() << packageName << " parse failed!";
        return NULL;
    }
    int checksum = splitChecksum.at(1).left(2).toInt(0, 16);
    if(!checkSum(package, checksum)) {
        qDebug() << packageName << "checksum failed!";
        return NULL;
    }
    return new QList<QByteArray>(splitChecksum.at(0).split(','));
}

void GPS::parseGPS(QByteArray gpsPackage) {
    qDebug() << "parseGPS: " << gpsPackage;

    QList<QByteArray> splitPackages = gpsPackage.split('\n');



    QByteArray package = gpsPackage;
    qDebug() << "start parse";
    if(QString(package).startsWith("$GPGGA")) {
        if(_gpgga != NULL)
            delete _gpgga;
        qDebug() << "parseGPGGA";
        _gpgga = parseGPGGA(package);
        qDebug() << "parseGPGGA ok";
    } else if (QString(package).startsWith("$GPGSA")) {
        if(_gpgsa != NULL)
            delete _gpgsa;
        qDebug() << "parseGPGSA";
        _gpgsa = parseGPGSA(package);
        qDebug() << "parseGPGSA ok";
    } else if (QString(package).startsWith("$GPGSV")) {
        qDebug() << "parseGPGSV";
        GPGSV *gpgsv = parseGPGSV(package);
        if(gpgsv != NULL) {

            if(_gpgsv == NULL) {
                _gpgsv = new GPGSV;
                _gpgsv->_satelites = new QList<SATELITE>;
            }

            for(int index = 0; index < gpgsv->_satelites->size(); index++) {
                _gpgsv->_satelites->append(gpgsv->_satelites->at(index));
            }
            while (_gpgsv->_satelites->size() >12) {
                qDebug() << "_gpgsv->_satelites: " << _gpgsv->_satelites;
                _gpgsv->_satelites->removeAt(0);
            }
            delete gpgsv;
        }
        qDebug() << "parseGPGSV ok";
    } else if(QString(package).left(6).compare("$GPRMC") == 0) {
        if(_gprmc != NULL)
            delete _gprmc;
        qDebug() << "parseGPRMC";
        _gprmc = parseGPRMC(package);
        qDebug() << "parseGPRMC ok";
    }
}

GPGGA *GPS::parseGPGGA(QByteArray gpggaPackage) {
    GPGGA *gpgga = new GPGGA;
    QList<QByteArray> *splitedPackage = splitPackage(gpggaPackage, "GPGGA");
    if(splitedPackage == NULL)
        return NULL;
    if (splitedPackage->size() < 11) {
        qDebug() << "wrong size: " << splitedPackage->size();
        delete splitedPackage;
        return NULL;
    }

    if (splitedPackage->at(3).isEmpty()) {
        qDebug() << "wrong section: " << 3;
        delete splitedPackage;
        return NULL;
    }
    if (splitedPackage->at(5).isEmpty()) {
        qDebug() << "wrong section: " << 5;
        delete splitedPackage;
        return NULL;
    }

    gpgga->_utcTime = splitedPackage->at(1).toFloat();
    float latitude = splitedPackage->at(2).toFloat(), longitude = splitedPackage->at(4).toFloat();
    char ns = splitedPackage->at(3)[0], ew = splitedPackage->at(5)[0];
    int msbLatitude = latitude / 100, msbLongitude = longitude / 100;
    gpgga->_latitude = ((latitude - msbLatitude * 100) / 60 + msbLatitude);
    if(ns == 'S') gpgga->_latitude *= -1;
    gpgga->_longitude = ((longitude - msbLongitude * 100) / 60 + msbLongitude);
    if(ew == 'W') gpgga->_longitude *= -1;
    gpgga->_quality = splitedPackage->at(6).toInt();
    gpgga->_satelites = splitedPackage->at(7).toInt();
    gpgga->_hdop = splitedPackage->at(8).toFloat();
    gpgga->_altitude = splitedPackage->at(9).toFloat();
    gpgga->_geoildalSeparation = splitedPackage->at(11).toFloat();
    delete splitedPackage;
    return gpgga;
}

GPGSA *GPS::parseGPGSA(QByteArray gpgsaPackage) {
    GPGSA *gpgsa = new GPGSA;
    QList<QByteArray> *splitedPackage = splitPackage(gpgsaPackage, "GPGSA");
    if(splitedPackage == NULL) return NULL;
    if (splitedPackage->size() < 17) {
        qDebug() << "wrong size: " << splitedPackage->size();
        delete splitedPackage;
        return NULL;
    }

    if (splitedPackage->at(1).isEmpty()) {
        qDebug() << "wrong section: " << 1;
        delete splitedPackage;
        return NULL;
    }


    gpgsa->_modeType = splitedPackage->at(1)[0];
    gpgsa->_mode = splitedPackage->at(2).toInt();
    gpgsa->_satelites = new QList<int>;
    for(int index = 3; index < 15; index++) {
        if(splitedPackage->at(index).size()) {
            gpgsa->_satelites->append(splitedPackage->at(index).toInt());
        }
    }
    qSort(gpgsa->_satelites->begin(), gpgsa->_satelites->end());
    gpgsa->_pdop = splitedPackage->at(15).toFloat();
    gpgsa->_hdop = splitedPackage->at(16).toFloat();
    gpgsa->_vdop = splitedPackage->at(17).toFloat();
    delete splitedPackage;
    return gpgsa;
}

GPGSV *GPS::parseGPGSV(QByteArray gpgsvPackage) {
    GPGSV *gpgsv = new GPGSV;
    QList<QByteArray> *splitedPackage = splitPackage(gpgsvPackage, "GPGSV");
    if(splitedPackage == NULL) return NULL;
    if (splitedPackage->size() < 19) {
        qDebug() << "wrong size: " << splitedPackage->size();
        delete splitedPackage;
        return NULL;
    }

    gpgsv->_satelites = new QList<SATELITE>;
    for(int index = 0; index <= 12; index += 4) {
        SATELITE satelite;
        satelite._id = splitedPackage->at(4 + index).size() ? splitedPackage->at(4 + index).toInt() : -1;
        satelite._elevation = splitedPackage->at(5 + index).size() ? splitedPackage->at(5 + index).toInt() : -1;
        satelite._azimuth = splitedPackage->at(6 + index).size() ? splitedPackage->at(6 + index).toInt() : -1;
        satelite._snr = splitedPackage->at(7 + index).size() ? splitedPackage->at(7 + index).toInt() : -1;
        gpgsv->_satelites->append(satelite);
    }
    return gpgsv;
}

GPRMC *GPS::parseGPRMC(QByteArray gprmcPackage) {
    GPRMC *gprmc = new GPRMC;
    qDebug() << "----------------";
    QList<QByteArray> *splitedPackage = splitPackage(gprmcPackage, "GPRMC");
    qDebug() << "========";
    if(splitedPackage == NULL) return NULL;
    if (splitedPackage->size() < 9) {
        qDebug() << "wrong size: " << splitedPackage->size();
        delete splitedPackage;
        return NULL;
    }

    if (splitedPackage->at(2).isEmpty()) {
        qDebug() << "wrong section: " << 2;
        delete splitedPackage;
        return NULL;
    }
    if (splitedPackage->at(4).isEmpty()) {
        qDebug() << "wrong section: " << 4;
        delete splitedPackage;
        return NULL;
    }
    if (splitedPackage->at(6).isEmpty()) {
        qDebug() << "wrong section: " << 6;
        delete splitedPackage;
        return NULL;
    }

    gprmc->_utcTime = splitedPackage->at(1).toFloat();
    gprmc->_validity = splitedPackage->at(2)[0];
    float latitude = splitedPackage->at(3).toFloat(), longitude = splitedPackage->at(5).toFloat();
    char ns = splitedPackage->at(4)[0], ew = splitedPackage->at(6)[0];
    int msbLatitude = latitude / 100, msbLongitude = longitude / 100;
    gprmc->_latitude = ((latitude - msbLatitude * 100) / 60 + msbLatitude);
    if(ns == 'S') gprmc->_latitude *= -1;
    gprmc->_longitude = ((longitude - msbLongitude * 100) / 60 + msbLongitude);
    if(ew == 'W') gprmc->_longitude *= -1;
    gprmc->_speed = splitedPackage->at(7).toFloat() * 1.852000;
    gprmc->_course = splitedPackage->at(8).toFloat();
    gprmc->_utDate = splitedPackage->at(9).toInt();
    return gprmc;

}
