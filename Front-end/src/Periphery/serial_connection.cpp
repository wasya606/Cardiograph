#include "serial_connection.h"
#include <QString>
#include <QSerialPort>
#include <QVariantMap>
#include <QSerialPortInfo>
#include <QDebug>

SerialConnection::SerialConnection() : PeripheralConnection()
{

}

void SerialConnection::init()
{
    peripheryDevice = new QSerialPort();
    connectSignals();
}

void SerialConnection::requestAvailableDevicesInfo()
{
    availableDevicesInfo->clear();
    for (QSerialPortInfo port : QSerialPortInfo::availablePorts())
    {
        PeripheralDeviceInfo serialPort;
        serialPort.name = port.portName();
        serialPort.systemLocation = port.systemLocation();
        serialPort.isBusy = port.isBusy();
        availableDevicesInfo->push_back(serialPort);
    }
    emit availableDevicesInfoUpdated();
}

void SerialConnection::connectDevice(const QVariant &deviceParameters)
{
    if (peripheryDevice != nullptr)
    {
        QString serialPortName = deviceParameters.toMap()["serialPortName"].toString();
        qint32 serialPortBaudRate = deviceParameters.toMap()["serialPortBaudRate"].toInt();

        ((QSerialPort*)peripheryDevice)->setPortName(serialPortName);
        ((QSerialPort*)peripheryDevice)->setBaudRate(serialPortBaudRate);
    }
    PeripheralConnection::connectDevice(deviceParameters);
}
