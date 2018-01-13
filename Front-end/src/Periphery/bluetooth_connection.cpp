#include "bluetooth_connection.h"
#include <QtBluetooth/QBluetoothSocket>
#include <QtBluetooth/QBluetoothServiceInfo>
#include <QtBluetooth/QBluetoothDeviceInfo>
#include <QtBluetooth/QBluetoothAddress>
#include <QtBluetooth/QBluetoothLocalDevice>
#include <QtBluetooth/QBluetoothHostInfo>
#include <QDebug>
#include <QVariantList>

BluetoothConnection::BluetoothConnection() : PeripheralConnection(),
    discoveryAgent(nullptr), availableBTDevices(new QList<QBluetoothDeviceInfo>())
{

}

void BluetoothConnection::init(const QVariant&)
{
    peripheryDevice = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol);
    connectSignals();
}

void BluetoothConnection::requestAvailableDevicesInfo()
{
    availableBTDevices->clear();

    discoveryAgent = new QBluetoothDeviceDiscoveryAgent();

    this->connect(discoveryAgent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered, this,
                  &BluetoothConnection::onServiceDiscovered);
    this->connect(discoveryAgent, &QBluetoothDeviceDiscoveryAgent::finished, this,
                  &BluetoothConnection::onDiscoveryFinished);
    this->connect(discoveryAgent, &QBluetoothDeviceDiscoveryAgent::canceled, this,
                  &BluetoothConnection::onDiscoveryFinished);

    discoveryAgent->start();
    qDebug() << "-------------- BT devices scanner started! -----------------";
}

void BluetoothConnection::connectDevice()
{
    //QBluetoothDeviceInfo devInfo = availableBTDevices->at(0);
    //qDebug() << "DEVICE: " << devInfo.name();
    //QBluetoothServiceInfo serviceInfo;
    //serviceInfo.setDevice(devInfo);

    qDebug() << "Try to connect to device";
    QBluetoothAddress addr("00:12:06:15:76:23");

    ((QBluetoothSocket*)peripheryDevice)->connectToService(addr, 1);//QBluetoothUuid(serviceUuid));
}

void BluetoothConnection::disconnectDevice()
{
    ((QBluetoothSocket*)peripheryDevice)->disconnectFromService();
}

void BluetoothConnection::connectSignals()
{
    PeripheralConnection::connectSignals();
    connect((QBluetoothSocket*)peripheryDevice, SIGNAL(connected()), this, SLOT(onDeviceConnected()));
    connect((QBluetoothSocket*)peripheryDevice, SIGNAL(disconnected()), this, SLOT(onDeviceDisconnected()));
}

void BluetoothConnection::disconnectSignals()
{
    PeripheralConnection::disconnectSignals();
    disconnect((QBluetoothSocket*)peripheryDevice, SIGNAL(connected()), this, SLOT(onDeviceConnected()));
    disconnect((QBluetoothSocket*)peripheryDevice, SIGNAL(disconnected()), this, SLOT(onDeviceDisconnected()));
}


void BluetoothConnection::onServiceDiscovered(const QBluetoothDeviceInfo &serviceInfo)
{
    availableBTDevices->append(serviceInfo);
}

void BluetoothConnection::onDiscoveryFinished()
{
    qDebug() << "-------------- List of avaliable BT devices -----------------";
    for (int i = 0; i < availableBTDevices->length(); i++)
    {
        QString deviceAddr = availableBTDevices->at(i).address().toString();
        QString deviceName = availableBTDevices->at(i).name();
        QString deviceUiid = availableBTDevices->at(i).deviceUuid().toString();
        qDebug() << "BT Device name: " << deviceName << ", address: " << deviceAddr << ", UUID" << deviceUiid;
    }
    qDebug() << "-------------- List of avaliable BT devices -----------------";
    emit availableDevicesInfoReceived(QVariant());
}
