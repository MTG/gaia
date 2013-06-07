# -------------------------------------------------
# Project created by QtCreator 2010-02-19T16:58:27
# -------------------------------------------------
QT -= gui
TARGET = Gaia2Python
TEMPLATE = lib
DEFINES += GAIA2PYTHON_LIBRARY

win32:INCLUDEPATH += $$quote(C:/Documents and Settings/Build Bot/build-space/gaia2-thirdparty/include) \
                     $$quote(C:/Documents and Settings/Build Bot/build-space/gaia2-thirdparty/yaml-0.1.3/include) \
                     ../../../src \
                     ../../../src/3rdparty \
                     ../../../src/metrics \
                     ../../../src/bindings \
                     C:\Python27\include

macx:INCLUDEPATH=../../../src\
                 ../../../src/3rdparty \
                 ../../../src/metrics \
                 ../../../src/bindings \
                 -F/Library/Frameworks \
                 /Library/Frameworks/Python.framework/Versions/2.7/include/python2.7 \
                 /opt/local/include

win32:LIBS += -L"C:/Python27/libs" -lpython27 \
              -L../../Gaia2lib/release -lGaia2lib \
              -L$$quote(C:/Documents and Settings/Build Bot/build-space/gaia2-thirdparty/yaml-0.1.3/yaml/release) -lyaml

macx: LIBS += -F/Library/Frameworks -L/Library/Frameworks \
              -L../Gaia2lib/ -lGaia2lib \
              -L/essentia/third-party/lib -L/opt/local/lib -lyaml \
              -framework Python

SOURCES += gaia_wrap.cxx
HEADERS +=
