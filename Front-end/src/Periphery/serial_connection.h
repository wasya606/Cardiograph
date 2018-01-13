#ifndef SERIAL_CONNECTION_H
#define SERIAL_CONNECTION_H

#include <QObject>
#include "peripheral_connection.h"

class SerialConnection : public PeripheralConnection
{
public:
    SerialConnection();
    //~SerialConnection();

    void init(const QVariant &deviceParameters);
    void requestAvailableDevicesInfo();
};

#endif // SERIAL_CONNECTION_H
