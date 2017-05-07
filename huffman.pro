TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    rawdata.cpp \
    prioritylist.cpp \
    packeddata.cpp \
    huffmanengine.cpp \
    runlengthengine.cpp

HEADERS += \
    rawdata.h \
    prioritylist.h \
    packeddata.h \
    huffmanengine.h \
    datatypedef.h \
    tchar.h \
    runlengthengine.h

