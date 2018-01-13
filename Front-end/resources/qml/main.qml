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
    }

    ControlPanel
    {
        anchors.top: oscillView.bottom;
        width: parent.width;
        height: parent.height - oscillView.height;
    }
}
