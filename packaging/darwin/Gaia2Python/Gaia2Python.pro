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
                 /usr/local/Cellar/python/2.7.5/Frameworks/Python.framework/Versions/2.7/include/python2.7 # Your python distribution include path

win32:LIBS += -L"C:/Python27/libs" -lpython27 \
              -L../../Gaia2lib/release -lGaia2lib \
              -L$$quote(C:/Documents and Settings/Build Bot/build-space/gaia2-thirdparty/yaml-0.1.3/yaml/release) -lyaml

macx: LIBS += -L../Gaia2lib/ -lGaia2lib \ # compiled Gaia2lib lib path
              -L/essentia/third-party/lib \ # essentia lib path (not  required)
              -L/opt/local/lib -lyaml \ # libyaml lib path
              -L/usr/local/Cellar/python/2.7.5/Frameworks/Python.framework/Versions/2.7/lib -lpython2.7 # Your python distribution lib path

SOURCES += gaia_wrap.cxx
HEADERS +=
