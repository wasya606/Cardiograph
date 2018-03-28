import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Controls.Styles 1.2

Item
{
    id: cPanel

    property alias avaliableDevices: devicesComboBox.model;
    property alias time: timeSpinBox.value;
    property alias sensitivity: sensitivitySpinBox.value;

    property bool isConnected: false;
    property bool isStarted: false;
    property bool isRequestConnectionInProgress: false;

    signal requestPeripheryConnection(var status);
    signal requestAvaliableDevices();
    signal changeCenter(var dir)

    Rectangle
    {
        id: background;
        anchors.fill: parent;
        color: "#101010";
        //border.width: 2;
        //border.color: "gold";
    }

    CPannelButton
    {
        id: btnConnect
        anchors.right: parent.right;
        anchors.top: parent.top;
        width: parent.width / 8;
        height: parent.height / 2;
        enabled: !isRequestConnectionInProgress && avaliableDevices != [];

        btnLabel.text: isConnected ? "Disconnect" : "Connect";
        onClicked:
        {
            isRequestConnectionInProgress = true;
            requestPeripheryConnection(isConnected ? "" : devicesComboBox.currentText);
        }
    }

    CPannelButton
    {
        id: btnStart

        anchors.right: parent.right;
        anchors.top: btnConnect.bottom;
        anchors.bottom: parent.bottom;
        anchors.topMargin: 1;
        width: parent.width / 8;
        btnLabel.text: isStarted ? "Stop" : "Start";
        enabled: isConnected;

        onClicked:
        {
            isStarted = !isStarted;
        }
    }

    Rectangle
    {
        property int defaultMargins: 5;

        id: connectionSetup
        anchors.top: parent.top;
        anchors.bottom: parent.bottom;
        anchors.right: btnConnect.left;
        width: parent.width / 3;

        border.color: "gold";
        border.width: 2;
        color: "transparent";
        enabled: !isRequestConnectionInProgress && !isConnected;

        Label
        {
            id: selDeviceLabel;
            anchors.top: parent.top;
            anchors.horizontalCenter: devicesComboBox.horizontalCenter;
            anchors.margins: parent.defaultMargins;
            text: "Select periphery device:";
            color: "white";
        }

        ComboBox
        {
            id: devicesComboBox;
            anchors.left: parent.left;
            anchors.right: parent.right;
            anchors.top: selDeviceLabel.bottom;
            anchors.margins: parent.defaultMargins;
            height: (parent.height - selDeviceLabel.height - anchors.margins * 4) / 2;

            background: Rectangle
            {
                border.color: "gold";
                color: "transparent";
            }

            contentItem: Text
            {
                text: devicesComboBox.displayText;
                font: devicesComboBox.font;
                color: "white";
                verticalAlignment: Text.AlignVCenter;
                horizontalAlignment: Text.AlignLeft;
                elide: Text.ElideRight;
                leftPadding: 10;
            }           


        }

        CPannelButton
        {
            id: updateDevices;
            anchors.top: devicesComboBox.bottom;
            anchors.left: parent.left;
            anchors.margins: parent.defaultMargins;
            height: devicesComboBox.height;
            width: devicesComboBox.width;

            btnLabel.text: "Update avaliable devices";

            onClicked:
            {
                requestAvaliableDevices();
            }
        }

        Rectangle
        {
            anchors.fill: parent;
            color: background.color;
            opacity: 0.5;
            visible: !parent.enabled
        }

    }
    Rectangle
    {
        property int defaultMargins: 5;

        id: adcSetup
        anchors.top: parent.top;
        anchors.bottom: parent.bottom;
        anchors.left: parent.left;
        anchors.margins: defaultMargins;
        width: parent.width / 3;

        border.color: "gold";
        border.width: 2;
        color: "transparent";
        enabled: isConnected;

        Label
        {
            id: timeLabel;
            anchors.top: parent.top;
            anchors.horizontalCenter: timeSpinBox.horizontalCenter;
            anchors.margins: parent.defaultMargins;
            text: "Time(mS/Div):";
            color: "white";
        }

        Label
        {
            id: sensitivityLabel;
            anchors.top: parent.top;
            anchors.horizontalCenter: sensitivitySpinBox.horizontalCenter;
            anchors.margins: parent.defaultMargins;
            text: "Sensitivity(mV/Div):";
            color: "white";
        }

        CPSpinBox
        {
            id: timeSpinBox;
            anchors.left: parent.left;
            anchors.top: timeLabel.bottom;
            anchors.margins: parent.defaultMargins;
            width: parent.width / 2 - parent.width / 10;
            from: 1;
            to: 2000;
            value: 1;
            stepSize: value < 10 ? 1 : (value < 100 ? 10 : 100);
        }

        CPSpinBox
        {
            id: sensitivitySpinBox;
            anchors.right: parent.right;
            anchors.top: sensitivityLabel.bottom;
            anchors.margins: parent.defaultMargins;
            width: parent.width / 2 - parent.width / 10;
            from: 200;
            to: 4000;
            value: 1500;
            stepSize: 50;
        }

        CPannelButton
        {
            id: upCenterBtn;
            anchors.top: parent.top;
            anchors.left: timeSpinBox.right;
            anchors.right: sensitivitySpinBox.left;
            anchors.margins: parent.defaultMargins;
            //width: sensitivitySpinBox.left - timeSpinBox.right;
            height: (parent.height - parent.defaultMargins * 3) / 2;

            btnLabel.text: "^";

            onClicked:
            {
                changeCenter(-1);
            }
        }

        CPannelButton
        {
            id: downCenterBtn;
            anchors.top: upCenterBtn.bottom;
            anchors.bottom: parent.bottom
            anchors.left: timeSpinBox.right;
            anchors.right: sensitivitySpinBox.left;
            anchors.margins: parent.defaultMargins;

            btnLabel.text: "v";

            onClicked:
            {
                changeCenter(1);
            }
        }


        Rectangle
        {
            anchors.fill: parent;
            color: background.color;
            opacity: 0.5;
            visible: !parent.enabled
        }
    }

    Rectangle
    {
        property int defaultMargins: 5;
        id: statusArea;
        anchors.left: adcSetup.right;
        anchors.right: connectionSetup.left;
        anchors.top: parent.top;
        anchors.bottom: parent.bottom;
        anchors.margins: defaultMargins;
        border.color: "gold";
        border.width: 2;
        color: "transparent";
        enabled: isConnected;

        Label
        {
            id: statusTitleLabel;
            anchors.top: parent.top;
            anchors.horizontalCenter: parent.horizontalCenter;
            anchors.margins: parent.defaultMargins;
            text: "Status:";
            color: "white";
        }

        Rectangle
        {
            anchors.fill: parent;
            color: background.color;
            opacity: 0.5;
            visible: !parent.enabled
        }
    }

    onIsConnectedChanged:
    {
        if (!isConnected)
            isStarted = false;
    }

    function connectionResponse(status)
    {
        isRequestConnectionInProgress = false;
        isConnected = status;
    }
}
