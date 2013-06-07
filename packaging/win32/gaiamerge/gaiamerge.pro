# -------------------------------------------------
# Project created by QtCreator 2010-02-19T19:09:46
# -------------------------------------------------
QT -= gui
TARGET = gaiamerge
CONFIG += console
CONFIG -= app_bundle
TEMPLATE = app

DEFINES += YAML_DECLARE_STATIC GAIA_VERSION=\\\"2.3-dev\\\"
INCLUDEPATH = ../../../../gaia2-thirdparty/include \
    ../../../../gaia2-thirdparty/yaml-0.1.3/include \
    ../../../src \
    ../../../src/3rdparty \
    ../../../src/metrics

LIBS += -L"C:/Python26/libs" -lpython26 \
    -L../Gaia2lib/release -lGaia2lib \
    -L../../../../gaia2-thirdparty/yaml-0.1.3/yaml/release -lyaml

SOURCES += ../../../src/tools/gaiamerge.cpp
