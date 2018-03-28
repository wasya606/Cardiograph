#include "oscillview.h"
#include <QDebug>
#include <math.h>
#include "serial_connection.h"
#include "bluetooth_connection.h"
#include "signalprocessor.h"

OscillView::OscillView(QQuickItem *context) :
    isAdcStarted(false),
    context(context),
    buffer(new QVariantList()),
    adcTime(0),
    samplesCount(0),
    peripheralConnection(nullptr)
{

}

OscillView::~OscillView()
{
    disconnectSignals();

    if (buffer != nullptr)
    {
        delete buffer;
        buffer = nullptr;
    }

    if (peripheralConnection != nullptr)
    {
        delete peripheralConnection;
        peripheralConnection = nullptr;
    }
}

void OscillView::init()
{
    //peripheralConnection = new BluetoothConnection();
    peripheralConnection = new SerialConnection();
    connectSignals();
    peripheralConnection->requestAvailableDevicesInfo();

    //context->setProperty("samplesCount", samplesCount);

    peripheralConnection->init();
    //peripheralConnection->init(QVariant());

}

void OscillView::onAvailableDevicesInfoUpdated()
{
    qDebug() << "-------- Avaliable devices info-----------";
    QList<PeripheralDeviceInfo>* devices = peripheralConnection->getAvailableDevicesInfo();
    QVariantList avaliableDevices;
    for (int i = 0; i < devices->length(); i++)
    {
        const PeripheralDeviceInfo& device = devices->at(i);
        qDebug() << "#" << i << ":" << device.systemLocation << ", is busy:" << device.isBusy;
        if (device.isBusy == false)
            avaliableDevices.push_back(device.systemLocation);
    }
    qDebug() << "-------------------------------------------";
    context->setProperty("avaliableDevices", avaliableDevices);
}

void OscillView::onPeripheralDeviceConnected()
{
    samplesCount = context->property("samplesCount").toUInt();
    adcTime = context->property("time").toUInt();
    peripheralConnection->setTime(adcTime);
    peripheralConnection->setSamplesCount(samplesCount);
    emit peripheryConnectedSignal(QVariant(true));
}

void OscillView::onPeripheralDeviceDisconnected()
{
    emit peripheryConnectedSignal(QVariant(false));
}

void OscillView::onDataReceived(const uint8_t cmdId, const uint8_t cmdResult, const QByteArray *data)
{
    //qDebug() << "onDataReceived: " << data;

    if (cmdResult == PeripheralConnection::CMD_ERROR)
        return;

    switch(cmdId)
    {
        case PeripheralConnection::SET_TIME_CMD >> 8:
        {
            qDebug() << "onDataReceived: SET_TIME_CMD SUCCESS!!!";
            break;
        }
        case PeripheralConnection::SET_SAMPLES_CMD >> 8:
        {
            qDebug() << "onDataReceived: SET_SAMPLES_CMD SUCCESS!!!";
            break;
        }
        case PeripheralConnection::SET_ADC_CONV_STATE_CMD >> 8:
        {
            if (isAdcStarted == false && data->at(0) != 0)
                peripheralConnection->requestAdcData();
            qDebug() << "onDataReceived: ADC state chnged from: " << isAdcStarted << " to: " << (bool)data->at(0);
            isAdcStarted = data->at(0);
            break;
        }
        case PeripheralConnection::GET_TIME_CMD >> 8:
        {
            if (data->length() == sizeof(adcTime))
                adcTime = data->at(0) << 24 | data->at(1) << 16 | data->at(2) << 8 | data->at(3);

            qDebug() << "onDataReceived: Time updated from device: " << adcTime;
            break;
        }
        case PeripheralConnection::GET_SAMPLES_CMD >> 8:
        {
            if (data->length() == sizeof(samplesCount))
                samplesCount = data->at(0) << 8 | data->at(1);
            qDebug() << "onDataReceived: Samples count updated from device: " << samplesCount;
            break;
        }
        case PeripheralConnection::REQUEST_DATA_CMD >> 8:
        {
            //qDebug() << "onDataReceived: ADC data received, data size: " << data->length();
            buffer->clear();
            for (int i = 0; i < data->length(); i += 2)
            {
                uint8_t lowByte = data->at(i);
                uint8_t highByte = data->at(i + 1);
                uint16_t val = (highByte << 8) | lowByte;
                buffer->push_back(val);
            }
            emit update(QVariant(*buffer));
            break;
        }
        default:
        break;
    }
}

void OscillView::onConversionStatusChanged(QVariant status)
{
    bool isNeedToStart = status.toBool();

    if (isNeedToStart)
    {
        peripheralConnection->startConversion();
    }
    else
    {
        peripheralConnection->stopConversion();
    }
}

void OscillView::onRequestPeriperyConnection(QVariant status)
{
    const QString& statusString = status.toString();
    qDebug() << "onRequestPeriperyConnection: " << statusString;

    if (statusString.length() > 0)
    {
        QVariantMap peripheryDeviceParameters;
        peripheryDeviceParameters["serialPortName"] = statusString;
        peripheryDeviceParameters["serialPortBaudRate"] = 115200;
        peripheralConnection->connectDevice(peripheryDeviceParameters);
    }
    else
    {
        peripheralConnection->disconnectDevice();
    }
}

void OscillView::onTimeChangedQml(QVariant newTime)
{
    uint32_t newTimeInt = newTime.toUInt();
    if (newTimeInt != adcTime)
    {
        adcTime = newTime.toInt();
        peripheralConnection->setTime(adcTime);
        qDebug() << "OscillView::onTimeChangedQml: " << adcTime;
    }
}

void OscillView::onSamplesCountChangedQml(QVariant newSamplesCount)
{
    uint16_t newSamplesCountInt = newSamplesCount.toUInt();
    if (newSamplesCountInt != samplesCount)
    {
        samplesCount = newSamplesCountInt;
        peripheralConnection->setSamplesCount(samplesCount);
        qDebug() << "OscillView::onSamplesCountChangedQml: " << samplesCount;
    }
}

void OscillView::onRequestAvaliableDevices()
{
    peripheralConnection->requestAvailableDevicesInfo();
}

void OscillView::onCanvasUpdated()
{
    peripheralConnection->requestAdcData();
}

void OscillView::connectSignals()
{
    connect(this, SIGNAL(update(QVariant)), context, SLOT(update(QVariant)));
    connect(this, SIGNAL(peripheryConnectedSignal(QVariant)), context, SLOT(onPeripheryConnected(QVariant)));
    connect(context, SIGNAL(conversionStatusSignal(QVariant)), this, SLOT(onConversionStatusChanged(QVariant)));
    connect(context, SIGNAL(requestPeripheryConnectionSignal(QVariant)), this, SLOT(onRequestPeriperyConnection(QVariant)));
    connect(context, SIGNAL(requestAvaliableDevicesSignal()), this, SLOT(onRequestAvaliableDevices()));
    connect(context, SIGNAL(canvasUpdated()), this, SLOT(onCanvasUpdated()));
    connect(context, SIGNAL(timeChangedSignal(QVariant)), this, SLOT(onTimeChangedQml(QVariant)));
    connect(context, SIGNAL(samplesCountChangedSignal(QVariant)), this, SLOT(onSamplesCountChangedQml(QVariant)));

    connect(peripheralConnection, SIGNAL(dataReceived(const uint8_t, const uint8_t, const QByteArray*)), this,
            SLOT(onDataReceived(const uint8_t, const uint8_t, const QByteArray*)));
    connect(peripheralConnection, SIGNAL(availableDevicesInfoUpdated()), this,
            SLOT(onAvailableDevicesInfoUpdated()));
    connect(peripheralConnection, SIGNAL(deviceConnected()), this,
            SLOT(onPeripheralDeviceConnected()));
    connect(peripheralConnection, SIGNAL(deviceDisconnected()), this,
            SLOT(onPeripheralDeviceDisconnected()));
    connect(peripheralConnection, SIGNAL(deviceConnectionError()), this,
            SLOT(onPeripheralDeviceDisconnected()));
}

void OscillView::disconnectSignals()
{
    disconnect(this, SIGNAL(update(QVariant)), context, SLOT(update(QVariant)));
    disconnect(this, SIGNAL(peripheryConnectedSignal(QVariant)), context, SLOT(onPeripheryConnected(QVariant)));
    disconnect(context, SIGNAL(conversionStatusSignal(QVariant)), this, SLOT(onConversionStatusChanged(QVariant)));
    disconnect(context, SIGNAL(requestPeripheryConnectionSignal(QVariant)), this, SLOT(onRequestPeriperyConnection(QVariant)));
    disconnect(context, SIGNAL(requestAvaliableDevicesSignal()), this, SLOT(onRequestAvaliableDevices()));
    disconnect(context, SIGNAL(canvasUpdated()), this, SLOT(onCanvasUpdated()));
    disconnect(context, SIGNAL(samplesCountChangedSignal(QVariant)), this, SLOT(onSamplesCountChangedQml(QVariant)));

    disconnect(context, SIGNAL(timeChangedSignal(QVariant)), this, SLOT(onTimeChangedQml(QVariant)));
    disconnect(peripheralConnection, SIGNAL(dataReceived(const uint8_t, const uint8_t, const QByteArray*)), this,
            SLOT(onDataReceived(const uint8_t, const uint8_t, const QByteArray*)));
    disconnect(peripheralConnection, SIGNAL(availableDevicesInfoUpdated()), this,
            SLOT(onAvailableDevicesInfoUpdated()));
    disconnect(peripheralConnection, SIGNAL(deviceConnected()), this,
            SLOT(onPeripheralDeviceConnected()));
    disconnect(peripheralConnection, SIGNAL(deviceDisconnected()), this,
            SLOT(onPeripheralDeviceDisconnected()));
    disconnect(peripheralConnection, SIGNAL(deviceConnectionError()), this,
            SLOT(onPeripheralDeviceDisconnected()));
}
