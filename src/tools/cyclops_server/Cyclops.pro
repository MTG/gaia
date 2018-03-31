#-------------------------------------------------
#
# Project created by QtCreator 2018-03-02T16:24:12
#
#-------------------------------------------------

QT       = core network
QT          -= gui
CONFIG      += console
DEFINES  += GAIA_QT5

TARGET = cyclopsmaster
TARGET = cyclops
TEMPLATE = lib

DEFINES += CYCLOPS_MASTER
#DEFINES += CYCLOPS_NORMAL

#CONFIG += c++03
#CONFIG += staticlib
#QMAKE_CXX = g++-7
CONFIG += c++11
TEMPLATE = app


# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
#DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0

#CONFIG += testcase
#SOURCES  += tst_qhttp.cpp
#INCLUDEPATH += "../../../include"

#QT = core network testlib http

#TARGET = tst_qhttp

#wince*: {
#    webFiles.files = webserver/*
#    webFiles.path = webserver
#    cgi.files = webserver/cgi-bin/*
#    cgi.path = webserver/cgi-bin
#    addFiles.files = rfc3252.txt testhtml
#    addFiles.path = .
#    DEPLOYMENT += addFiles webFiles cgi
#    DEFINES += SRCDIR=\\\"\\\"
#} else:vxworks*: {
#    DEFINES += SRCDIR=\\\"\\\"
#} else {
#    DEFINES += SRCDIR=\\\"$$PWD/\\\"
#}
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0


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
        qthttp/qhttp.cpp \
        qthttp/qhttpauthenticator.cpp \
        yamlrpcserver.cpp \
        #debugging.cpp \
        ../../yamlcpp.cpp
    #cyclopsnetworkreplyerror.cpp

HEADERS += \
        cyclops.h \
        cyclopsmaster.h \
        cyclopsproxy.h \
        logging.h \
        qthttp/qhttp.h \
        qthttp/qringbuffer_p.h \
        qthttp/qhttpauthenticator_p.h \
        yamlrpcserver.h \
        #debugging.h \
        ../../yamlcpp.h

#HEADERS += qhttp.h qringbuffer_p.h qhttpauthenticator_p.h
#SOURCES += qhttp.cpp qhttpauthenticator.cpp

cyclops.o: cyclops.moc

#cyclops.moc: cyclops.cpp
#        moc $(DEFINES) $(INCPATH) -i $< -o $@

# $$PWD means the current directory where the .pro file is.
# eg. ~/gaia/src/tools/cyclops_server
BUILD_DIR = $$PWD

LIBS += -lyaml -lQtNetwork

INCLUDEPATH += $$NS_BUILD_DIR

#unix {
#
#    SHARED_LIB_FILES = $$files($$BUILD_DIR/*.so)
#
#    for(FILE, SHARED_LIB_FILES) {
#
#        BASENAME = $$basename(FILE)
#
#	CLEARNAME = $$replace(BASENAME,libns,ns)
#
#        CLEARNAME = $$replace(CLEARNAME,.so,)
#
#        LIBS += -l$$CLEARNAME
#
#    }
#}
