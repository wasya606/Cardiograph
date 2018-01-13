import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Controls.Styles 1.2

Item
{
    id: cPanel

    property bool isConnected: false;
    property bool isStarted: false;

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

        btnLabel.text: isConnected ? "Disconnect" : "Connect";

        onClicked:
        {
            isConnected = !isConnected;
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

    CustomGroupBox
    {
        id: connectionSetupGroupBox;
        anchors.right: btnConnect.left;
        anchors.rightMargin: 1;
        anchors.top: parent.top;
        anchors.bottom: parent.bottom;
        width: parent.width / 2;

        title: "Connection setup";

        ComboBox
        {
            id: serialPortComboBox
            anchors.left: parent.left;
            anchors.verticalCenter: parent.verticalCenter;
            width: parent.width / 2;
            height: parent.height / 2;

            model: ["/dev/ttyUSB0", "/dev/ttyUSB1"];

            background: Rectangle
            {
                border.color: "gold";
                color: "transparent";
            }

            contentItem: Text
            {
                text: serialPortComboBox.displayText;
                font: serialPortComboBox.font;
                color: "white";
                verticalAlignment: Text.AlignVCenter;
                horizontalAlignment: Text.AlignLeft;
                elide: Text.ElideRight;
                leftPadding: 10;
            }

        }

    }

    onIsConnectedChanged:
    {
        if (!isConnected)
            isStarted = false;
    }

}
