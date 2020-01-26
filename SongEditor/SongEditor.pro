QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

QMAKE_CXXFLAGS += -v
QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.14

INCLUDEPATH += /usr/local/include ../CT-StandardLibrary/src

LIBS += -v
LIBS += -L/usr/local/lib  -L../CT-StandardLibrary/lib
LIBS += -lvitac-mac -lsfml-audio -lsfml-graphics -lsfml-system -lsfml-window -lboost_filesystem

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Common.cpp \
    Generator.cpp \
    NotesTableForm.cpp \
    NotesViewPanel.cpp \
    Pattern.cpp \
    Preferences.cpp \
    SetupForm.cpp \
    Song.cpp \
    SongInfoForm.cpp \
    main.cpp \
    MainWindow.cpp

HEADERS += \
    Common.h \
    Generator.h \
    MainWindow.h \
    NotesTableForm.h \
    NotesViewPanel.h \
    Pattern.h \
    Preferences.h \
    SetupForm.h \
    Song.h \
    SongInfoForm.h

FORMS += \
    MainWindow.ui \
    NotesTableForm.ui \
    SetupForm.ui \
    SongInfoForm.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Images.qrc \
    Patterns/Basic.json

myPatterns.files = $$PWD/Patterns
myPatterns.path = Contents/Resources
QMAKE_BUNDLE_DATA += myPatterns

images.files = $$PWD/Images
images.path = Contents/Resources
QMAKE_BUNDLE_DATA += images
