QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    main_window.cpp \
    plan_position_indicator.cpp \
    radar_points.cpp \
    radar_tracks.cpp

HEADERS += \
    main_window.h \
    plan_position_indicator.h \
    radar_points.h \
    radar_tracks.h

FORMS += \
    main_window.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32:CONFIG(debug,debug|release){
    LIBS += -L$$PWD/lib_windows/ -lqwtd -lqwtpolard
}
win32:CONFIG(release,debug|release){
    LIBS += -L$$PWD/lib_windows/ -lqwt -lqwtpolar
}

INCLUDEPATH += $$PWD/include/qwt
DEPENDPATH += $$PWD/include/qwt

INCLUDEPATH += $$PWD/include/qwt_polar
DEPENDPATH += $$PWD/include/qwt_polar
