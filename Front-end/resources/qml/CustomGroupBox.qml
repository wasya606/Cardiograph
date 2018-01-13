import QtQuick 2.7
import QtQuick.Controls 2.0

GroupBox
{
    property int customtTitleMargin: 5;
    property int customBorderLineWidth: 2;
    property string customBorderColor: "gold";
    property string customTitleColor: "white";
    property alias customTitleLabel: customTitle

    id: groupBox;
    title: "GroupBox";

    background: Canvas
    {
        id: bgCanvas
        anchors.fill: parent;
        anchors.topMargin: customTitle.height / 2;

        onPaint:
        {
            var drawContext = bgCanvas.getContext("2d");

            drawContext.strokeStyle = customBorderColor;
            drawContext.lineWidth = customBorderLineWidth;
            drawContext.beginPath();
            drawContext.moveTo(customTitle.x - customtTitleMargin, 0);
            drawContext.lineTo(0, 0);
            drawContext.lineTo(0, bgCanvas.height);
            drawContext.lineTo(bgCanvas.width, bgCanvas.height);
            drawContext.lineTo(bgCanvas.width, 0);
            drawContext.lineTo(customTitle.x + customTitle.width + customtTitleMargin, 0);
            drawContext.stroke();
        }
    }

    label: Label
    {
        id: customTitle
        anchors.horizontalCenter: groupBox.horizontalCenter;
        color: customTitleColor;
        text: groupBox.title;
    }


}
