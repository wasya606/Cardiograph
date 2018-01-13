import QtQuick 2.0

Item
{
    id: lineStage
    property int fromX: 0;
    property int fromY: 0;
    property int toX: 0;
    property int toY: 0;
    property string lineColor: "white";
    property int lineWidth: 3;

    property bool isRight: toX > fromX;
    property bool isDown: toY > fromY;

    x: isRight ? fromX : toX;
    y: isDown ? fromY : toY;
    width: Math.abs(toX - x) + lineWidth;
    height: Math.abs(toY - fromY) + lineWidth;

    Canvas
    {
        id: can
        anchors.fill: parent;

        renderStrategy: Canvas.Threaded;

        onPaint:
        {
            var drawContext = can.getContext("2d");
            drawContext.reset();
            drawContext.strokeStyle = lineColor;
            drawContext.lineWidth = lineWidth;
            drawContext.beginPath();
            drawContext.moveTo(isRight ? 0 : lineStage.width, isDown ? 0 : lineStage.height);
            drawContext.lineTo(isRight ? lineStage.width : 0, isDown ? lineStage.height : 0);
            drawContext.stroke();
        }
    }

    function update()
    {
        can.requestPaint();
    }
}
