QT       += core gui
LIBS     += -lemcrk

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    crookconf.cpp \
    kernelselect.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    crookconf.h \
    kernelselect.h \
    mainwindow.h

FORMS += \
    kernelselect.ui \
    mainwindow.ui

TRANSLATIONS = translations/krokonf_en.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /usr/bin
!isEmpty(target.path): INSTALLS += target

