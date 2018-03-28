#ifndef OSCILLVIEW_H
#define OSCILLVIEW_H

#include <QObject>
#include <QQuickItem>
#include <QTimer>
#include "peripheral_connection.h"
#include <QList>

class OscillView : public QObject
{
    Q_OBJECT
public:
    OscillView(QQuickItem *context);
    ~OscillView();

    void init();

signals:
    void update(QVariant);
    void peripheryConnectedSignal(QVariant);

private slots:
    void onAvailableDevicesInfoUpdated();
    void onPeripheralDeviceConnected();
    void onPeripheralDeviceDisconnected();
    void onDataReceived(const uint8_t cmdId, const uint8_t cmdResult, const QByteArray* data);

    void onConversionStatusChanged(QVariant status);
    void onRequestPeriperyConnection(QVariant status);
    void onTimeChangedQml(QVariant newTime);
    void onSamplesCountChangedQml(QVariant newSamplesCount);
    void onRequestAvaliableDevices();
    void onCanvasUpdated();


private:
    void connectSignals();
    void disconnectSignals();

private:
    bool isAdcStarted;

    QQuickItem* context;
    QVariantList* buffer;

    uint32_t adcTime;
    uint16_t samplesCount;

    PeripheralConnection* peripheralConnection;
};

#endif // OSCILLVIEW_H
