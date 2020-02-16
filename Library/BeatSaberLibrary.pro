QT -= gui

TEMPLATE = lib
DEFINES += BEATSABERLIBRARY_LIBRARY

CONFIG += c++11

INCLUDEPATH += ./include ./src /usr/local/include

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
    src/showpage/FileUtilities.cpp \
    src/showpage/JSON_Serializable.cpp \
    src/showpage/OptionHandler.cpp \
    src/showpage/StringMethods.cpp \
    src/showpage/StringVector.cpp \
    src/showpage/URI.cpp \
    src/showpage/WaitCondition.cpp \
    src/showpage/WorkQueue.cpp \
    src/song_editor/CLI.cpp \
    src/song_editor/Common.cpp \
    src/song_editor/Generator.cpp \
    src/song_editor/Pattern.cpp \
    src/song_editor/Preferences.cpp \
    src/song_editor/SaberLocation.cpp \
    src/song_editor/Song.cpp

HEADERS += \
    include/chrono_io.h \
    include/date.h \
    include/json.hpp \
    src/showpage/FileUtilities.h \
    src/showpage/JSON_Serializable.h \
    src/showpage/OptionHandler.h \
    src/showpage/PointerMap.h \
    src/showpage/PointerVector.h \
    src/showpage/StringMethods.h \
    src/showpage/StringVector.h \
    src/showpage/URI.h \
    src/showpage/UnitTesting.h \
    src/showpage/WaitCondition.h \
    src/showpage/WorkQueue.h \
    src/song_editor/CLI.h \
    src/song_editor/Common.h \
    src/song_editor/Generator.h \
    src/song_editor/Pattern.h \
    src/song_editor/Preferences.h \
    src/song_editor/SaberLocation.h \
    src/song_editor/Song.h

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target
