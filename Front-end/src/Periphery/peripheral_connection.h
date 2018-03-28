#ifndef PERIPHERAL_CONNECTION_H
#define PERIPHERAL_CONNECTION_H

#include <QObject>
#include <QIODevice>

struct PeripheralDeviceInfo
{
    QString name;
    QString systemLocation;
    QString address;
    QString id;
    bool isBusy = false;
};

class PeripheralConnection : public QObject
{
    Q_OBJECT
public:
    explicit PeripheralConnection(QObject *parent = nullptr);
    virtual ~PeripheralConnection();

    virtual void init() = 0;
    virtual void requestAvailableDevicesInfo() = 0;

    virtual void connectDevice(const QVariant& deviceParameters);
    virtual void disconnectDevice();

    void setTime(const uint32_t time);
    void setSamplesCount(const uint16_t count);
    void startConversion();
    void stopConversion();
    void getTime();
    void getSamples();
    void requestAdcData();
    QList<PeripheralDeviceInfo>* getAvailableDevicesInfo() const;


public:
    //High byte - command ID, low byte - command expected data length
    //Cmd format: CC:[DD:DD:...], CC - command id byte, DD - input data bytes
    //Response format: [DD:DD:DD:...]:CC:RR, DD - response data bytes, CC - command id byte, RR - result byte
    enum UARTCommand
    {
        NONE_CMD                = 0x0000,
        SET_TIME_CMD            = 'T' << 8 | 0x04,//Expected 4 bytes of input data;
        SET_SAMPLES_CMD         = 'S' << 8 | 0x02,//Expected 2 bytes of input data;
        SET_ADC_CONV_STATE_CMD  = 'C' << 8 | 0x01,//Expected 1 bytes of input data;
        GET_TIME_CMD            = 't' << 8 | 0x00,//No input data expected;
        GET_SAMPLES_CMD         = 's' << 8 | 0x00,//No input data expected;
        REQUEST_DATA_CMD        = 'R' << 8 | 0x00 //No input data expected;
    };

    enum UARTCommandResult
    {
        CMD_OK      = 'O',
        CMD_ERROR   = 'E'
    };


signals:
    void deviceConnected();
    void deviceConnectionError();
    void deviceDisconnected();
    void availableDevicesInfoUpdated();
    void dataReceived(const uint8_t cmdId, const uint8_t cmdResult, const QByteArray* data);

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
    virtual void sendCmd(const QByteArray& cmd);

    void writeCmd(const QByteArray& cmd);

protected:
    uint8_t activeCmdID;
    uint32_t conversionTime;
    uint16_t samplesCount;
    QIODevice* peripheryDevice;

    QByteArray* buffer;
    QByteArrayList* cmdQueue;
    QList<PeripheralDeviceInfo>* availableDevicesInfo;

};

#endif // PERIPHERAL_CONNECTION_H
