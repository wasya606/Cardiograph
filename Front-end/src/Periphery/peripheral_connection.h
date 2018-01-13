#ifndef PERIPHERAL_CONNECTION_H
#define PERIPHERAL_CONNECTION_H

#include <QObject>
#include <QIODevice>

class PeripheralConnection : public QObject
{
    Q_OBJECT
public:
    explicit PeripheralConnection(QObject *parent = nullptr);
    virtual ~PeripheralConnection();

    virtual void init(const QVariant& deviceParameters) = 0;
    virtual void requestAvailableDevicesInfo() = 0;

    virtual void connectDevice();
    virtual void disconnectDevice();

    void setTime(const uint16_t time);
    void getTime();
    void setSamplesCount(const uint16_t count);
    void getSamplesCount();
    void requestData(const uint8_t blocks, const uint16_t samples);

public:
    enum UartCommandRequest
    {
        SET_TIME            = 0xC0,
        GET_TIME            = 0xC1,
        SET_SAMPLES         = 0xC2,
        GET_SAMPLES         = 0xC3,
        REQUEST_DATA        = 0xC4
    };

    enum UartCommandResponse
    {
        CMD_OK      = 0xF0D0,
        CMD_BUSY    = 0xF0FE,
        CMD_ERROR   = 0xF0FF
    };

signals:
    void deviceConnected();
    void deviceConnectionError();
    void deviceDisconnected();
    void availableDevicesInfoReceived(const QVariant& info);
    void dataReceived(const uint8_t cmd, const QByteArray* data);

protected:
    struct PendingCmd
    {
        PendingCmd(const uint8_t cmd, const uint16_t data,
                   const uint16_t receivedDataLength, const uint16_t receivedDataBlocks = 1) :
                   receivedDataLength(receivedDataLength), receivedDataBlocks(receivedDataBlocks)
        {
            command.push_back(cmd);
            command.push_back((data >> 8) & 0xFF);
            command.push_back(data & 0xFF);
            command.push_back(cmd);
        }
        QByteArray command;
        uint16_t receivedDataLength;
        uint16_t receivedDataBlocks;
    };

protected slots:
    void onReadyRead();
    void onDeviceConnected();
    void onDeviceDisconnected();



protected:
    virtual void connectSignals();
    virtual void disconnectSignals();
    virtual void sendCmd(PendingCmd *cmd);
    virtual void sendQueuedCmd();

protected:
    bool isCmdConfirmed;
    uint16_t readedDataBlocks;
    QIODevice* peripheryDevice;

    QByteArray* buffer;
    QList<PendingCmd*>* cmdQueue;

    PendingCmd* pendingCmd;
};

#endif // PERIPHERAL_CONNECTION_H
