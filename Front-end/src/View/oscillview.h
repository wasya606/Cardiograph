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
    void update();

private slots:
    void loop();
    void onAvailableDevicesInfoReceived(const QVariant& info);
    void onPeripheralDeviceConnected();
    void onPeripheralDeviceDisconnected();
    void onDataReceived(const uint8_t cmd, const QByteArray* data);

private:
    QQuickItem* context;
    QTimer* loopTimer;

    QVariantList* buffer;

    int blocksCount;
    int blockSize;
    int currentBlock;
    int amplitude;

    PeripheralConnection* peripheralConnection;
};

#endif // OSCILLVIEW_H
