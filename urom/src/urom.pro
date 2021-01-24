QT -= gui

CONFIG += c++14 console
CONFIG -= app_bundle

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    main.cpp \
    urom.cpp \
    ucode.cpp \
    step.cpp \
    instruction.cpp \
    uromdata.cpp

HEADERS += \
    urom.h \
    ucode.h \
    step.h \
    instruction.h \
    uromdata.h
