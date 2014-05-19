TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
#CONFIG -= qt
QT += core
#LIBS += -lsqlite3x
DEFINES += UNIX
debug,release {
    SOURCES += main.cpp
}
SOURCES +=  connectiongraph.cpp \
    #matcher.cpp \
    #subscription.cpp \
    #lpc.cpp \
    #registrymanager.cpp \
    #tablesubscriptionmanager.cpp \
    #pal.cpp \
    #iniconfig.cpp \
    #iconfig.cpp \
    #registry.cpp \
    #dps.cpp \
    test/test_connectiongraph.cpp

HEADERS += \
    #matcher.h \
    #subscription.h \
    lpc.h \
    registrymanager.h \
    dps.h \
    isubscriptionmanager.h \
    tablesubscriptionmanager.h \
    pal.h \
    iniconfig.h \
    iconfig.h \
    registry.h \
    connectiongraph.h \
    connectiongraph.tpl.hpp

test{
    SOURCES += test/main.cpp
    DEFINES+=TEST_ENV=1
    LIBS+=-lcppunit
}
