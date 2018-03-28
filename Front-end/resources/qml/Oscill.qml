import QtQuick 2.0

Item
{
    id: stage

    property int samplesCount: 256;

    property int step: width / samplesCount;

    property int verticalPosDelta: 0

    property int verticalPos: verticalPosDelta * horizontalLinesInterval + height / 2;
    property string lineColor: "white"

    //Background
    property int verticalLinesInterval: width / 10;
    property int horizontalLinesInterval: height / 10;

    property string backGroundColor: "black";
    property string verticalLinesColor: "blue";
    property string horizontalLinesColor: "green";

    property int time: 0;
    property int sensitivityPerPixel: 0;

    property var values: [];
    property bool isStarted: false;
    property var avaliableDevices: []

    signal peripheryConnectedSignal(var status);
    signal conversionStatusSignal(var status);
    signal requestPeripheryConnectionSignal(var status);
    signal requestAvaliableDevicesSignal();
    signal canvasUpdated();
    signal timeChangedSignal(var newTime);
    signal samplesCountChangedSignal(var newSamplesCount);

    Component.onCompleted: init();

    onTimeChanged:
    {
        timeChangedSignal(time);
    }

    onSamplesCountChanged:
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
                drawContext.clearRect(0, 0, stage.width, stage.height);

                //Draw horizontal lines
                drawContext.strokeStyle = horizontalLinesColor;
                drawContext.lineWidth = 2;
                drawContext.globalAlpha = 0.1;
                drawContext.beginPath();
                for (var i = 0; height / 2 - i >= 0; i += horizontalLinesInterval)
                {
                    drawContext.moveTo(0, height / 2 + i);
                    drawContext.lineTo(stage.width, height / 2 + i);
                    drawContext.moveTo(0, height / 2 - i);
                    drawContext.lineTo(stage.width, height / 2 - i);
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
                drawContext.globalAlpha = 0.25;
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
            if (!isStarted)
                return;

            var drawContext = drawCanvas.getContext("2d");
            drawContext.clearRect(0, 0, stage.width, stage.height);
            drawContext.strokeStyle = lineColor;
            drawContext.lineWidth = 2;
            drawContext.beginPath();

            drawContext.moveTo(0, verticalPos);
            for (var i = 0; i < samplesCount; i++)
            {
                var validValue = -values[i] * 3300 / 4095 / sensitivityPerPixel;
                drawContext.lineTo(i * step, validValue + verticalPos);
            }
            drawContext.stroke();
            canvasUpdated();
        }
    }

    function update(newValues)
    {
        values = newValues;
        if (isStarted)
        {
            drawCanvas.requestPaint();
        }
    }

    function init()
    {
        timeChangedSignal(time);
        samplesCountChangedSignal(samplesCount);
        console.log("Qscill.qml -> init()");
        for (var i = 0; i < samplesCount; i++)
        {
            values[i] = 0;
        }
    }

    function setConversionStatus(status)
    {
        isStarted = status;
        conversionStatusSignal(status);
    }

    function requestPeripheryConnection(status)
    {
        requestPeripheryConnectionSignal(status);
    }

    function onPeripheryConnected(status)
    {
        peripheryConnectedSignal(status);
    }

    function requestAvaliableDevices()
    {
        avaliableDevices = [];
        requestAvaliableDevicesSignal();
    }

    function changeCenter(dir)
    {
        if (verticalPosDelta + dir < 5 && verticalPosDelta + dir > -5)
        {
            verticalPosDelta += dir;
            backgroundCanvas.requestPaint();
        }
    }
}
