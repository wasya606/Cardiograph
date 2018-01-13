#include "oscillview.h"
#include <QDebug>
#include <math.h>
#include "serial_connection.h"
#include "bluetooth_connection.h"
#include "signalprocessor.h"

OscillView::OscillView(QQuickItem *context) :
    context(context),
    loopTimer(new QTimer(this)),
    buffer(new QVariantList()),
    blocksCount(0),
    blockSize(32),
    currentBlock(0),
    amplitude(430),
    peripheralConnection(nullptr)
{

}

OscillView::~OscillView()
{

}

void OscillView::init()
{
    srand(time(0));
    connect(loopTimer, SIGNAL(timeout()), this, SLOT(loop()));
    connect(this, SIGNAL(update()), context, SLOT(update()));

    peripheralConnection = new BluetoothConnection();

    connect(peripheralConnection, SIGNAL(dataReceived(uint8_t,const QByteArray*)), this,
            SLOT(onDataReceived(uint8_t,const QByteArray*)));

    connect(peripheralConnection, SIGNAL(availableDevicesInfoReceived(const QVariant&)), this,
            SLOT(onAvailableDevicesInfoReceived(const QVariant&)));

    connect(peripheralConnection, SIGNAL(deviceConnected()), this,
            SLOT(onPeripheralDeviceConnected()));

    //peripheralConnection->requestAvailableDevicesInfo();

    context->setProperty("blockSize", blockSize);

    blocksCount = 1024 / blockSize;

    QVariantMap peripheryDeviceParameters;
    peripheryDeviceParameters["serialPortName"] = "/dev/ttyUSB0";
    peripheryDeviceParameters["serialPortBaudRate"] = 115200;

    peripheralConnection->init(QVariant());

    peripheralConnection->connectDevice();

}

void OscillView::loop()
{
    if (!buffer->isEmpty())
    {
        QVariantList values;
        for (uint16_t i = 0; i < blockSize; i++)
        {
            if (!buffer->isEmpty())
            {
                values.push_back(buffer->first());
                buffer->pop_front();
            }
            else
            {
                qDebug() << "FAKE DATA!!!";
                values.push_back(amplitude / 2);
            }
        }
        //qDebug() << "LOOP values first: " << values.first() << ", last: " << values.last();
        context->setProperty("values", values);



        emit update();
    }
    else
    {
        peripheralConnection->requestData(blocksCount, blockSize);
        loopTimer->stop();
    }
}

void OscillView::onAvailableDevicesInfoReceived(const QVariant &info)
{
    qDebug() << "OscillView::onAvailableDevicesInfoReceived: " << info;
}

void OscillView::onPeripheralDeviceConnected()
{
    //peripheralConnection->setTime(20);
    //peripheralConnection->setSamplesCount(32);

    peripheralConnection->setTime(60);
    peripheralConnection->setSamplesCount(blockSize);
    peripheralConnection->requestData(blocksCount, blockSize);
}

void OscillView::onPeripheralDeviceDisconnected()
{
    loopTimer->stop();
}

void OscillView::onDataReceived(const uint8_t cmd, const QByteArray *data)
{
    if (cmd == PeripheralConnection::REQUEST_DATA)
    {
        for (int i = 0; i < blockSize * 2; i += 2)
        {
            uint8_t highByte = data->at(i);
            uint8_t lowByte = data->at(i + 1);
            uint16_t val = (highByte << 8) | lowByte;
            val = val * amplitude / 4095;
            buffer->push_back(int(amplitude / 2 - val));
            //qDebug() << "buffer[" << i << "] = " << amplitude / 2 - val;
        }
        //qDebug() << "onDataReceived data len: " << data->length() << "buff len:" << buffer->length() ;
        if (!loopTimer->isActive())
        {
            loopTimer->start(50);
        }
    }

}
