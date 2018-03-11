#-------------------------------------------------
#
# Project created by QtCreator 2018-03-02T16:24:12
#
#-------------------------------------------------

QT       = core network
#DEFINES  += GAIA_QT5

TARGET = Cyclops
TEMPLATE = lib

#DEFINES += CYCLOPS_MASTER
#DEFINES += CYCLOPS_NORMAL

CONFIG += c++03
#CONFIG += staticlib
#QMAKE_CXX = g++-7
#CONFIG += c++11


# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        cyclops.cpp \
        cyclopsmaster.cpp \
        cyclopsmasterserver.cpp \
        cyclopsproxy.cpp \
        cyclopsserver.cpp \
        yamlrpcserver.cpp \
        ../../yamlcpp.cpp

HEADERS += \
        cyclops.h \
        cyclopsmaster.h \
        cyclopsproxy.h \
        logging.h \
        yamlrpcserver.h \
        ../../yamlcpp.h

#cyclops.o: cyclops.moc

#cyclops.moc: cyclops.cpp
#        moc $(DEFINES) $(INCPATH) -i $< -o $@

# $$PWD means the current directory where the .pro file is.
# eg. ~/gaia/src/tools/cyclops_server
BUILD_DIR = $$PWD

LIBS += -lyaml -lQtNetwork

INCLUDEPATH += $$NS_BUILD_DIR

unix {

    SHARED_LIB_FILES = $$files($$BUILD_DIR/*.so)

    for(FILE, SHARED_LIB_FILES) {

        BASENAME = $$basename(FILE)

#	CLEARNAME = $$replace(BASENAME,libns,ns)

        CLEARNAME = $$replace(CLEARNAME,.so,)

        LIBS += -l$$CLEARNAME

    }
}
