TEMPLATE = app

QT += qml quick widgets serialport bluetooth

CONFIG += c++11

INCLUDEPATH += src/Controller \
               src/View \
               src/Model \
               src/Common \
               src/Periphery

SOURCES += src/main.cpp \
           src/Controller/appcontroller.cpp \
           src/View/mainview.cpp \
           src/Common/constants.cpp \
           src/View/oscillview.cpp \
           src/Periphery/peripheral_connection.cpp \
           src/Periphery/serial_connection.cpp \
    src/Periphery/bluetooth_connection.cpp \
    src/Model/signalprocessor.cpp

HEADERS += src/Controller/appcontroller.h \
           src/View/mainview.h \
           src/Common/constants.h \
           src/View/oscillview.h \
           src/Periphery/peripheral_connection.h \
           src/Periphery/serial_connection.h \
    src/Periphery/bluetooth_connection.h \
    src/Model/signalprocessor.h

RESOURCES += resources/qml.qrc \
    resources/images.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH = resourses/qml/

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

