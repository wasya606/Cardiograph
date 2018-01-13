#include "peripheral_connection.h"
#include <QDebug>

PeripheralConnection::PeripheralConnection(QObject *parent) :
    QObject(parent), isCmdConfirmed(false), readedDataBlocks(0), peripheryDevice(nullptr),
    buffer(new QByteArray()), cmdQueue(new QList<PendingCmd*>()), pendingCmd(nullptr)
{

}

PeripheralConnection::~PeripheralConnection()
{
    disconnectDevice();
    disconnectSignals();
    if (peripheryDevice != nullptr)
        delete peripheryDevice;

    if (buffer != nullptr)
        delete buffer;

    if (cmdQueue != nullptr)
        delete cmdQueue;

    if (pendingCmd != nullptr)
        delete pendingCmd;
}

void PeripheralConnection::connectDevice()
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

void PeripheralConnection::setTime(const uint16_t time)
{
    sendCmd(new PendingCmd(SET_TIME, time, 0));
}

void PeripheralConnection::getTime()
{
    sendCmd(new PendingCmd(GET_TIME, 0xFFFF, 2));
}

void PeripheralConnection::setSamplesCount(const uint16_t count)
{
    sendCmd(new PendingCmd(SET_SAMPLES, count, 0));
}

void PeripheralConnection::getSamplesCount()
{
    sendCmd(new PendingCmd(GET_SAMPLES, 0xFFFF, 2));
}

void PeripheralConnection::requestData(const uint8_t blocks, const uint16_t samples)
{
    sendCmd(new PendingCmd(REQUEST_DATA, blocks, samples * 2, blocks));
}

void PeripheralConnection::onReadyRead()
{
    QByteArray buff = peripheryDevice->readAll();

    if (pendingCmd == nullptr)
        return;

    buffer->append(buff);

    if (!isCmdConfirmed && buffer->length() > 1)
    {
        uint16_t response = ((buffer->at(0) << 8) & 0xFF00) | (buffer->at(1) & 0x00FF);
        if (response == CMD_OK)
        {
            buffer->remove(0, 2);
            isCmdConfirmed = true;
        }
    }

    //qDebug() << "DATA RECEIVED!!! buffer len: " << buff.length();
    if (isCmdConfirmed && buffer->length() >= pendingCmd->receivedDataLength)
    {
        while(buffer->length() >= pendingCmd->receivedDataLength && readedDataBlocks < pendingCmd->receivedDataBlocks)
        {
            readedDataBlocks++;
            emit dataReceived(pendingCmd->command.at(0), buffer);
            buffer->remove(0, pendingCmd->receivedDataLength);
        }
        //qDebug() << "DATA RECEIVED!!! Readed blocks: " << readedDataBlocks << " of " << pendingCmd->receivedDataBlocks;
        if (readedDataBlocks >= pendingCmd->receivedDataBlocks)
        {
            isCmdConfirmed = false;
            readedDataBlocks = 0;
            delete pendingCmd;
            pendingCmd = nullptr;
            if (!cmdQueue->isEmpty())
            {
                pendingCmd = cmdQueue->first();
                cmdQueue->pop_front();
                peripheryDevice->write(pendingCmd->command);
                //qDebug() << "SENT CMD: " << QString::number(pendingCmd->command.at(0) & 0xFF, 16);
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

void PeripheralConnection::sendCmd(PendingCmd *cmd)
{
    if (pendingCmd == nullptr && cmdQueue->isEmpty())
    {
        pendingCmd = cmd;
        peripheryDevice->write(pendingCmd->command);
        //qDebug() << "SENT CMD: " << QString::number(pendingCmd->command.at(0) & 0xFF, 16);
    }
    else
    {
        cmdQueue->push_back(cmd);
    }
}

void PeripheralConnection::sendQueuedCmd()
{

}
