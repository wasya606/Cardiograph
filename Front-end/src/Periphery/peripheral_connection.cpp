#include "peripheral_connection.h"
#include <QDebug>

PeripheralConnection::PeripheralConnection(QObject *parent) :
    QObject(parent), activeCmdID(NONE_CMD & 0xFF), conversionTime(0), samplesCount(0), peripheryDevice(nullptr),
    buffer(new QByteArray()), cmdQueue(new QByteArrayList()), availableDevicesInfo(new QList<PeripheralDeviceInfo>())
{

}

PeripheralConnection::~PeripheralConnection()
{
    disconnectDevice();
    disconnectSignals();
    if (peripheryDevice != nullptr)
    {
        delete peripheryDevice;
        peripheryDevice = nullptr;
    }

    if (buffer != nullptr)
    {
        delete buffer;
        buffer = nullptr;
    }

    if (cmdQueue != nullptr)
    {
        delete cmdQueue;
        cmdQueue = nullptr;
    }

    if (availableDevicesInfo != nullptr)
    {
        delete availableDevicesInfo;
        availableDevicesInfo = nullptr;
    }
}

void PeripheralConnection::connectDevice(const QVariant&)
{
    if (peripheryDevice != nullptr)
    {
        if (peripheryDevice->isOpen())
        {
            qDebug() << "Warning! Device is already opened!!!";
            return;
        }

        if (peripheryDevice->open(QIODevice::ReadWrite))
        {
            qDebug() << "Periphery device CONNECTED!";
            emit deviceConnected();
        }
        else
        {
            qDebug() << "ERROR opennig periphery device!!!";
            emit deviceConnectionError();
        }
    }
    else
    {
        qDebug() << "ERROR!!! PeripheralConnection::peripheryDevice is NULL!!!";
        emit deviceConnectionError();
    }
}

void PeripheralConnection::disconnectDevice()
{
    activeCmdID = NONE_CMD >> 8;
    cmdQueue->clear();
    if (peripheryDevice != nullptr)
    {
        if (peripheryDevice->isOpen())
        {
            peripheryDevice->close();
            qDebug() << "Periphry device DISCONNECTED!";
            emit deviceDisconnected();
        }
    }
    else
    {
        qDebug() << "ERROR!!! PeripheralConnection::peripheryDevice is NULL!!!";
        emit deviceConnectionError();
    }
}

void PeripheralConnection::setTime(const uint32_t time)
{
    conversionTime = time;
    QByteArray cmd;
    cmd.push_back((SET_TIME_CMD >> 8) & 0xFF);
    cmd.push_back((time >> 24) & 0xFF);
    cmd.push_back((time >> 16) & 0xFF);
    cmd.push_back((time >> 8) & 0xFF);
    cmd.push_back(time & 0xFF);
    sendCmd(cmd);
}

void PeripheralConnection::setSamplesCount(const uint16_t count)
{
    samplesCount = count;
    QByteArray cmd;
    cmd.push_back((SET_SAMPLES_CMD >> 8) & 0xFF);
    cmd.push_back((count >> 8) & 0xFF);
    cmd.push_back(count & 0xFF);
    sendCmd(cmd);
}

void PeripheralConnection::startConversion()
{
    QByteArray cmd;
    cmd.push_back((SET_ADC_CONV_STATE_CMD >> 8) & 0xFF);
    cmd.push_back(0x01);
    sendCmd(cmd);
}

void PeripheralConnection::stopConversion()
{
    QByteArray cmd;
    cmd.push_back((SET_ADC_CONV_STATE_CMD >> 8) & 0xFF);
    cmd.push_back('\0');
    sendCmd(cmd);
}

void PeripheralConnection::getTime()
{
    QByteArray cmd;
    cmd.push_back((GET_TIME_CMD >> 8) & 0xFF);
    sendCmd(cmd);
}

void PeripheralConnection::getSamples()
{
    QByteArray cmd;
    cmd.push_back((GET_SAMPLES_CMD >> 8) & 0xFF);
    sendCmd(cmd);
}

void PeripheralConnection::requestAdcData()
{
    QByteArray cmd;
    cmd.push_back((REQUEST_DATA_CMD >> 8) & 0xFF);
    sendCmd(cmd);
}

QList<PeripheralDeviceInfo> *PeripheralConnection::getAvailableDevicesInfo() const
{
    return availableDevicesInfo;
}

void PeripheralConnection::onReadyRead()
{
    QByteArray buff = peripheryDevice->readAll();

    if (activeCmdID == ((NONE_CMD >> 8) & 0xFF))
        return;

    buffer->append(buff);
    const size_t bufferLen = buffer->length();
    if (bufferLen >= 3)
    {
        QByteArray cmdResult;
        cmdResult.push_back(activeCmdID);
        cmdResult.push_back(CMD_OK);
        cmdResult.push_back(activeCmdID);

        int resultIndex = buffer->indexOf(cmdResult);
        if (resultIndex == -1)
        {
            cmdResult[1] = CMD_ERROR;
            resultIndex = buffer->indexOf(cmdResult);
        }
        if (resultIndex != -1)
        {
            const uint8_t cmdResult = buffer->at(resultIndex + 1);
            buffer->remove(resultIndex, bufferLen - resultIndex + 1);
            emit dataReceived(activeCmdID, cmdResult, buffer);
            \
            if (!cmdQueue->isEmpty())
            {
                const QByteArray cmd = cmdQueue->front();
                cmdQueue->pop_front();
                writeCmd(cmd);
            }
            else
            {
                activeCmdID = (NONE_CMD >> 8) & 0xFF;
            }
        }
    }
}

void PeripheralConnection::onDeviceConnected()
{
    qDebug() << "Periphery device CONNECTED!";
    emit deviceConnected();
}

void PeripheralConnection::onDeviceDisconnected()
{
    qDebug() << "Periphry device DISCONNECTED!";
    emit deviceDisconnected();
}

void PeripheralConnection::connectSignals()
{
    if (peripheryDevice != nullptr)
    {
        connect(peripheryDevice, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    }
}

void PeripheralConnection::disconnectSignals()
{
    if (peripheryDevice != nullptr)
    {
        disconnect(peripheryDevice, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    }
}

void PeripheralConnection::sendCmd(const QByteArray& cmd)
{
    if (activeCmdID == NONE_CMD >> 8 && cmdQueue->isEmpty())
    {
        writeCmd(cmd);
    }
    else
    {
        cmdQueue->push_back(cmd);
    }
}

void PeripheralConnection::writeCmd(const QByteArray &cmd)
{
    if (!cmd.isEmpty())
    {
        buffer->clear();
        activeCmdID = cmd.at(0);
        peripheryDevice->write(cmd);
    }
}

