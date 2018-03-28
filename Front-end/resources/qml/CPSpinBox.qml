import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Controls.Styles 1.2

SpinBox
{
    id: control;

    editable: true;
    background: Rectangle
    {
        color: "transparent";
        border.color: "gold";
        border.width: 2;
    }

    contentItem: TextInput
    {
        id: spinTextBox
        //anchors.verticalCenter: control.verticalCenter;
        //anchors.horizontalCenter: control.horizontalCenter;
        //anchors.left: downBtn.right;
        //anchors.right: upBtn.right;
        //z: 2;
        width: control.width / 3;
        text: control.textFromValue(control.value, control.locale);

        //font: control.font;
        font.pixelSize: 18
        color: "white";
        selectionColor: "blue";
    }

    up.indicator: Rectangle
    {
        id: upBtn;
        anchors.right: control.right;
        anchors.top: control.top;
        anchors.bottom: control.bottom;
        //height: 40
        width: control.width / 4;
        //implicitWidth: 40
        //implicitHeight: 40
        color: "transparent";
        border.color: "gold"

        Text
        {
            text: "+"
            font.pixelSize: control.font.pixelSize * 2
            color: "white"
            anchors.fill: parent
            fontSizeMode: Text.Fit
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
    }

    down.indicator: Rectangle
    {
        id: downBtn;
        anchors.left: control.left;
        anchors.top: control.top;
        anchors.bottom: control.bottom;
        width: control.width / 4;
        //height: parent.height
        //implicitWidth: 40
        //implicitHeight: 40
        color: "transparent"
        border.color: "gold";

        Text
        {
            text: "-"
            font.pixelSize: control.font.pixelSize * 2
            color: "white"
            anchors.fill: parent
            fontSizeMode: Text.Fit
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
    }
}
