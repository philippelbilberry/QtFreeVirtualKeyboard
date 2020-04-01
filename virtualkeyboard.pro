#-------------------------------------------------
#
# Project created by QtCreator 2013-04-04T23:11:38
#
#-------------------------------------------------

QT       += qml quick quick-private gui-private

CONFIG += plugin c++11

TARGET = freevirtualkeyboardplugin
TEMPLATE = lib


SOURCES += VirtualKeyboardInputContextPlugin.cpp \
    VirtualKeyboardInputContext.cpp \
    DeclarativeInputEngine.cpp

HEADERS += VirtualKeyboardInputContextPlugin.h\
    VirtualKeyboardInputContext.h \
    DeclarativeInputEngine.h


deployment.files = *.qml FontAwesome.otf qmldir

linux-buildroot-g++ {
    deployment.path = /usr/qml/QtQuick/FreeVirtualKeyboard
    target.path = /usr/lib/qt/plugins/platforminputcontexts
} else {
    deployment.path = $$[QT_INSTALL_QML]/QtQuick/FreeVirtualKeyboard
    target.path = $$[QT_INSTALL_PLUGINS]/platforminputcontexts
}

OBJECTS_DIR = obj
MOC_DIR = moc
UI_DIR = ui
RCC_DIR = resource

INSTALLS += target \
    deployment

OTHER_FILES += \
    InputPanel.qml \
    KeyModel.qml \
    KeyButton.qml \
    KeyPopup.qml

RESOURCES += \
    virtualkeyboard.qrc
