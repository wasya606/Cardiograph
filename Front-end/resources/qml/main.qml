import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3

ApplicationWindow
{
    visible: true;
    width: 1024;
    height: 600;
    title: qsTr("Cardiograph");


    //objectName: "MainWindow";

    Oscill
    {
        id: oscillView;
        objectName: "OscillView";
        anchors.fill: parent;
        anchors.bottomMargin: 100;
        time: cPanel.time * 10 * 1000 / samplesCount;
        sensitivityPerPixel: cPanel.sensitivity / horizontalLinesInterval;

        onPeripheryConnectedSignal:
        {
            cPanel.connectionResponse(status);
        }
    }

    ControlPanel
    {
        id: cPanel;
        anchors.top: oscillView.bottom;
        width: parent.width;
        height: parent.height - oscillView.height;
        avaliableDevices: oscillView.avaliableDevices;

        onRequestPeripheryConnection:
        {
            oscillView.requestPeripheryConnection(status);
        }

        onRequestAvaliableDevices:
        {
            oscillView.requestAvaliableDevices();
        }

        onIsStartedChanged:
        {
            oscillView.setConversionStatus(isStarted);
        }

        onChangeCenter:
        {
            oscillView.changeCenter(dir);
        }
    }
}
