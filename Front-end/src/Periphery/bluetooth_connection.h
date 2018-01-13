#ifndef BLUETOOTH_CONNECTION_H
#define BLUETOOTH_CONNECTION_H

#include <QObject>
#include "peripheral_connection.h"
#include <QtBluetooth/QBluetoothDeviceDiscoveryAgent>

static const QString serviceUuid(QStringLiteral("00001101-0000-1000-8000-00805F9B34FB"));

class BluetoothConnection : public PeripheralConnection
{
public:

    BluetoothConnection();

    void init(const QVariant&);
    void requestAvailableDevicesInfo();

    void connectDevice();
    void disconnectDevice();

protected:
    void connectSignals();
    void disconnectSignals();

private:
    void onServiceDiscovered(const QBluetoothDeviceInfo &serviceInfo);
    void onDiscoveryFinished();

private:
    QBluetoothDeviceDiscoveryAgent* discoveryAgent;
    QList<QBluetoothDeviceInfo>* availableBTDevices;
};

#endif // BLUETOOTH_CONNECTION_H
