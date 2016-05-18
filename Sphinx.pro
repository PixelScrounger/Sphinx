#-------------------------------------------------
#
# Project created by QtCreator 2014-08-17T16:05:55
#
#-------------------------------------------------

QT       += core gui
QT       += sql
QT       += printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Sphinx
TEMPLATE = app

win32: CONFIG += c++11

SOURCES += main.cpp\
        mainwindow.cpp \
    login.cpp \
    register.cpp \
    helpsubjects.cpp \
    addquestion.cpp \
    questions.cpp \
    addanswer.cpp \
    questionary.cpp \
    questionarylist.cpp \
    questionaryinfo.cpp \
    printquestionary.cpp \
    selectdatabase.cpp \
    mymsgbox.cpp \
    users.cpp \
    about.cpp

HEADERS  += mainwindow.h \
    login.h \
    register.h \
    helpsubjects.h \
    addquestion.h \
    questions.h \
    addanswer.h \
    questionary.h \
    questionarylist.h \
    questionaryinfo.h \
    printquestionary.h \
    selectdatabase.h \
    mymsgbox.h \
    users.h \
    about.h

FORMS    += mainwindow.ui \
    login.ui \
    register.ui \
    helpsubjects.ui \
    addquestion.ui \
    questions.ui \
    addanswer.ui \
    questionary.ui \
    questionarylist.ui \
    questionaryinfo.ui \
    printquestionary.ui \
    selectdatabase.ui \
    users.ui \
    about.ui

!win32: LIBS += -lqrencode -lzbar -lopencv_core -lopencv_imgproc -lopencv_highgui

win32: {
    INCLUDEPATH += C:\Dev\opencv\RELEASE\install\include
    INCLUDEPATH += C:\Dev\ZBar\include
    INCLUDEPATH += C:\Dev\QREncode\qtqrencode-master\build\qqrencode\release\include

    LIBS += C:\Dev\opencv\RELEASE\bin\libopencv_core249.dll
    LIBS += C:\Dev\opencv\RELEASE\bin\libopencv_highgui249.dll
    LIBS += C:\Dev\opencv\RELEASE\bin\libopencv_imgproc249.dll

    LIBS += C:\Dev\ZBar\bin\libzbar-0.dll
    LIBS += C:\Dev\QREncode\qtqrencode-master\build\qqrencode\release\qtqrencode.dll
}


RC_FILE = Sphinx.rc

RESOURCES += \
    res.qrc
