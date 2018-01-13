import QtQuick 2.0

Item
{
    id: stage

    property int verticalPos: height / 2;
    property int blockSize: 16;
    property int blocksCount: width / blockSize;
    property int currentBlock: 0;
    property int currentBlockX: currentBlock * blockSize;
    property real lastValue: 0;

    property string lineColor: "white"

    //Background
    property int verticalLinesInterval: blockSize;
    property int horizontalLinesInterval: 60;
    property string backGroundColor: "black";
    property string verticalLinesColor: "blue";
    property string horizontalLinesColor: "green";

    property var values: [];

    property bool isAllowDraw: false;

    Component.onCompleted: init();

    onBlockSizeChanged:
    {
        init();
    }

    Rectangle
    {
        id: background
        anchors.fill: parent
        color: backGroundColor

        Canvas
        {
            id: backgroundCanvas;
            anchors.fill: parent;

            onPaint:
            {
                var drawContext = backgroundCanvas.getContext("2d");

                //Draw horizontal lines
                drawContext.strokeStyle = horizontalLinesColor;
                drawContext.lineWidth = 2;
                drawContext.globalAlpha = 0.1;
                drawContext.beginPath();
                for (var i = 0; verticalPos - i >= 0; i += horizontalLinesInterval)
                {
                    drawContext.moveTo(0, verticalPos + i);
                    drawContext.lineTo(stage.width, verticalPos + i);
                    drawContext.moveTo(0, verticalPos - i);
                    drawContext.lineTo(stage.width, verticalPos - i);
                }
                drawContext.stroke();

                //Draw vertical lines
                drawContext.strokeStyle = verticalLinesColor;
                drawContext.lineWidth = 2;
                drawContext.globalAlpha = 0.3;
                drawContext.beginPath();
                for (i = 0; i < stage.width; i += verticalLinesInterval)
                {
                    drawContext.moveTo(i, 0);
                    drawContext.lineTo(i, stage.height);
                }
                drawContext.stroke();

                //Draw center horizonal line
                drawContext.strokeStyle = horizontalLinesColor;
                drawContext.lineWidth = 4;
                drawContext.globalAlpha = 0.15;
                drawContext.beginPath();
                drawContext.moveTo(0, verticalPos);
                drawContext.lineTo(stage.width, verticalPos);
                drawContext.stroke();

            }
        }
    }

    Canvas
    {
        id: drawCanvas;
        anchors.fill: parent;

        onPaint:
        {
            if (!isAllowDraw)
                return;

            var drawContext = drawCanvas.getContext("2d");
            drawContext.clearRect(currentBlockX, 0, blockSize * 2, stage.height);
            drawContext.strokeStyle = lineColor;
            drawContext.lineWidth = 2;
            drawContext.beginPath();

            drawContext.moveTo(currentBlockX == 0 ? 0 : currentBlockX - 1, lastValue + verticalPos);
            for (var i = 0; i < blockSize; i++)
            {
                drawContext.lineTo(currentBlockX + i, values[i] + verticalPos);
            }
            drawContext.stroke();

            if (currentBlock < blocksCount - 1)
            {
                lastValue = values[blockSize - 1] ? values[blockSize - 1] : lastValue;
                currentBlock++;
            }
            else
            {
                currentBlock = 0;
                lastValue = 0;
            }
        }
    }

    function update()
    {
        isAllowDraw = true;
        drawCanvas.requestPaint();
    }

    function init()
    {
        console.log("Qscill.qml -> init()");
        for (var i = 0; i < blockSize; i++)
        {
            values[i] = 0;
        }
    }

}
