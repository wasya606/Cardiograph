#ifndef CONSTANTS_H
#define CONSTANTS_H
#include <QString>
#include <QUrl>
#include <cstring>

class Constants
{
public:
    static constexpr const char* const QML_PATH_PREFIX = "qrc:/qml/";
    static constexpr const char* const IMAGE_PATH_PREFIX = "qrc:/images/";

    static constexpr const char* const QML_MAIN_WINDOW_FILE_NAME = "main.qml";

    static constexpr const char* const OSCILL_VIEW_OBJECT_NAME = "OscillView";


};


#endif // CONSTANTS_H
