QT       += core gui
QT += network
 QT += core5compat
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


DEFINES += ZLIB_WINAPI

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    myproxyserver.cpp \
    serverthread.cpp

HEADERS += \
    KeyScanAPI.h \
    NLPIR.h \
    mainwindow.h \
    myproxyserver.h \
    serverthread.h \
    zconf.h \
    zlib.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

unix|win32: LIBS += -L$$PWD/lib/ -lKeyScanAPI

INCLUDEPATH += $$PWD/.
DEPENDPATH += $$PWD/.

unix|win32: LIBS += -L$$PWD/lib/ -lNLPIR

INCLUDEPATH += $$PWD/.
DEPENDPATH += $$PWD/.

DISTFILES += \
    keylist.txt

unix|win32: LIBS += -L$$PWD/lib/ -lzlib

INCLUDEPATH += $$PWD/.
DEPENDPATH += $$PWD/.
