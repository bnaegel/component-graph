TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

# remove possible other optimization flags
QMAKE_CXXFLAGS_RELEASE -= -O
QMAKE_CXXFLAGS_RELEASE -= -O1
QMAKE_CXXFLAGS_RELEASE -= -O2

QMAKE_CXXFLAGS_RELEASE+= -O2

QMAKE_CXXFLAGS+=-std=c++0x
QMAKE_LFLAGS+=-std=c++0x
QMAKE_CXXFLAGS_X86_64 += -std=c++0x -stdlib=libc++
QMAKE_CXXFLAGS_X86_64+= -mmacosx-version-min=10.7
QMAKE_LFLAGS_X86_64 += -std=c++0x -stdlib=libc++
QMAKE_LFLAGS_X86_64-= -mmacosx-version-min=10.5
QMAKE_LFLAGS_X86_64+= -mmacosx-version-min=10.7


# add the desired -O3 if not present
QMAKE_CXXFLAGS_RELEASE *= -O3

SOURCES += \
    cgraph.cpp \
    cgraphdemo.cpp \
    ragraph.cpp \
    cgraphwatcher.cpp \
    colorordering.cpp \
    GUI/core.cpp \
    GUI/corefilter.cpp \
    GUI/dialogimpulsenoise.cpp \
    GUI/dialogNoise.cpp \
    GUI/main.cpp \
    GUI/maindisplay.cpp \
    GUI/mainwindow.cpp \
    GUI/moc_corefilter.cpp \
    GUI/moc_dialogimpulsenoise.cpp \
    GUI/moc_dialogNoise.cpp \
    GUI/moc_mainwindow.cpp \
    GUI/noise.cpp

HEADERS += \
    OrderedQueue.h \
    Image.h \
    FlatSE.h \
    Types.h \
    Point.h \
    ImageIterators.h \
    ImageIO.hxx \
    Image.hxx \
    cgraph.h \
    ragraph.h \
    cgraphwatcher.h \
    colorordering.h \
    GUI/core.h \
    GUI/corefilter.h \
    GUI/dialogimpulsenoise.h \
    GUI/dialogNoise.h \
    GUI/maindisplay.h \
    GUI/mainwindow.h \
    GUI/noise.h \
    GUI/ui_dialogimpulsenoise.h \
    GUI/ui_dialogNoise.h \
    GUI/ui_mainwindow.h \
    FlatSE.hxx

SUBDIRS += \
    GUI/cgraph.pro \
    GUI/cgraph.pro

