# -------------------------------------------------
# Project created by QtCreator 2010-02-19T16:58:27
# -------------------------------------------------
QT -= gui
TARGET = Gaia2Python
CONFIG += dll
TEMPLATE = lib
DEFINES += GAIA2PYTHON_LIBRARY

INCLUDEPATH += ../../../../gaia2-thirdparty/include \
    ../../../../gaia2-thirdparty/yaml-0.1.3/include \
    ../../../src \
    ../../../src/3rdparty \
    ../../../src/metrics \
    ../../../src/bindings \
    C:\Python27\include

LIBS += -L"C:/Python27/libs" -lpython27 \
    -L../Gaia2lib/release -lGaia2lib \
    -L../../../../gaia2-thirdparty/yaml-0.1.3/yaml/release -lyaml


SOURCES += gaia_wrap.cxx
HEADERS +=
