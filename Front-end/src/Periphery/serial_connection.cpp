#include "serial_connection.h"
#include <QString>
#include <QSerialPort>
#include <QVariantMap>
#include <QSerialPortInfo>
#include <QDebug>

SerialConnection::SerialConnection() : PeripheralConnection()
{

}

void SerialConnection::init(const QVariant& deviceParameters)
{
    QString serialPortName = deviceParameters.toMap()["serialPortName"].toString();
    qint32 serialPortBaudRate = deviceParameters.toMap()["serialPortBaudRate"].toInt();

    peripheryDevice = new QSerialPort();
    ((QSerialPort*)peripheryDevice)->setPortName(serialPortName);
    ((QSerialPort*)peripheryDevice)->setBaudRate(serialPortBaudRate);

    connectSignals();
}

void SerialConnection::requestAvailableDevicesInfo()
{
    QVariantList ports;
    qDebug() << "-------------- List of avaliable serial ports -----------------";
    for (QSerialPortInfo port : QSerialPortInfo::availablePorts())
    {
        qDebug() << "Serial port name: " << port.portName() << ", is busy: " << port.isBusy();
        ports.append(port.portName());
    }
    qDebug() << "--------------------------------------------------------------";
    emit availableDevicesInfoReceived(QVariant(ports));
}
