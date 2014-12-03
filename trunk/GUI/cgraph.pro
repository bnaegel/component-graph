QMAKE_CXXFLAGS_RELEASE+= -O2

QMAKE_CXXFLAGS+=-std=c++0x
QMAKE_LFLAGS+=-std=c++0x
QMAKE_CXXFLAGS_X86_64 += -std=c++0x -stdlib=libc++
QMAKE_CXXFLAGS_X86_64+= -mmacosx-version-min=10.7
QMAKE_LFLAGS_X86_64 += -std=c++0x -stdlib=libc++
QMAKE_LFLAGS_X86_64-= -mmacosx-version-min=10.5
QMAKE_LFLAGS_X86_64+= -mmacosx-version-min=10.7


TEMPLATE = app
CONFIG += release
TARGET = cgraph




QT+=core gui widgets concurrent
SOURCES += \
    mainwindow.cpp \
    main.cpp \
    maindisplay.cpp \
    corefilter.cpp \
    noise.cpp \
    dialogNoise.cpp \
    dialogimpulsenoise.cpp \
    ../cgraph.cpp \
    ../ragraph.cpp

INCLUDEPATH+=..



HEADERS += \
    mainwindow.h \
    maindisplay.h \
    corefilter.h \
    noise.h \
    dialogNoise.h \
    dialogimpulsenoise.h \
    ../cgraph.h \
    ../colorordering.h \
    ../ragraph.h

FORMS += \
    mainwindow.ui \
    dialogNoise.ui \
    dialogimpulsenoise.ui \
    mainwindow.ui \
    dialogimpulsenoise.ui

OTHER_FILES +=


