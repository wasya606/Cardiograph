import QtQuick 2.7
import QtQuick.Controls 2.0
//import QtQuick.Layouts 1.3

Button
{
    property alias btnLabel: label;
    property int minWidth: label.width + 5;

    id: btn;
    highlighted: true;
    //width: width > label.width + 5 ? width : label.width + 5;
    flat: true;

    background: Rectangle
    {
        id: backgroundRectangle;
        border.width: 2;
        border.color: "gold";
        color: parent.down ? "#2c5112" : parent.hovered ? "#1e4701" : "#1e360d";
    }

    Label
    {
        id: label;

        anchors.centerIn: parent;
        text: "Button";
        color: "white";
        font.italic: !parent.enabled;
    }

    Rectangle
    {
        id: disabledShadow;
        anchors.fill: parent;
        anchors.margins: backgroundRectangle.border.width;
        color: "#423f3f";
        opacity: 0.7;
        visible: !parent.enabled;
    }

}
