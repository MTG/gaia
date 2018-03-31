# -------------------------------------------------
# Project created by QtCreator 2010-02-19T15:29:39
# -------------------------------------------------
#QT -= gui
#TARGET = Gaia2lib
#TEMPLATE = lib
#CONFIG += c++03
#CONFIG += staticlib
QMAKE_CXX = g++
CONFIG += c++11
QT -= gui
QT += concurrent core
TARGET = Gaia2lib
TEMPLATE = lib

DEFINES += YAML_DECLARE_STATIC \
           GAIA_VERSION=\\\"2.4.4\\\" \
           GAIA_GIT_SHA=\\\"v2.4.4-43-g1d593b8-dirty\\\"

win32:INCLUDEPATH = $$quote(C:/Documents and Settings/Build Bot/build-space/gaia2-thirdparty/include) \
                    $$quote(C:/Documents and Settings/Build Bot/build-space/gaia2-thirdparty/yaml-0.1.3/include) \
                    ../../../src \
                    ../../../src/3rdparty \
                    ../../../src/metrics

macx:INCLUDEPATH = ../../../src \
                   ../../../src/3rdparty \
                   ../../../src/metrics \
                   /opt/local/include \
                   /usr/local/include

SOURCES += ../../../src/yamlcpp.cpp \
           ../../../src/utils.cpp \
           ../../../src/transformation.cpp \
           ../../../src/timer.cpp \
           ../../../src/searchspacepool.cpp \
           ../../../src/searchspace.cpp \
           ../../../src/region.cpp \
           ../../../src/pointlayout.cpp \
           ../../../src/point.cpp \
           ../../../src/parameter.cpp \
           ../../../src/linalg.cpp \
           ../../../src/gaiamath.cpp \
           ../../../src/gaia.cpp \
           ../../../src/descriptortree.cpp \
           ../../../src/debugging.cpp \
           ../../../src/dataset.cpp \
           ../../../src/frozendataset.cpp \
           ../../../src/convert.cpp \
           ../../../src/parser/grammar.c \
           ../../../src/parser/filter.cpp \
           ../../../src/metrics/weightedpearsondistance.cpp \
           ../../../src/metrics/weightedeuclideandistance.cpp \
           ../../../src/metrics/summedsegmentdistance.cpp \
           ../../../src/metrics/semanticclassifierdistance.cpp \
           ../../../src/metrics/rhythmdistance.cpp \
           ../../../src/metrics/randomdistance.cpp \
           ../../../src/metrics/cosineangledistance.cpp \
           ../../../src/metrics/minkowskidistance.cpp \
           ../../../src/metrics/manhattandistance.cpp \
           ../../../src/metrics/linearcombinationdistance.cpp \
           ../../../src/metrics/kullbackleiblerdistance.cpp \
           ../../../src/metrics/keydistance.cpp \
           ../../../src/metrics/forceidentitydistance.cpp \
           ../../../src/metrics/exponentialcompressdistance.cpp \
           ../../../src/metrics/euclideandistance.cpp \
           ../../../src/metrics/distancefunction.cpp \
           ../../../src/metrics/frozendistance.cpp \
           ../../../src/metrics/crosssegmentdistance.cpp \
           ../../../src/metrics/cosinesimilarity.cpp \
           ../../../src/metrics/bpfdistance.cpp \
           ../../../src/algorithms/svmtrain.cpp \
           ../../../src/algorithms/svmpredict.cpp \
           ../../../src/algorithms/selectdesc.cpp \
           ../../../src/algorithms/select.cpp \
           ../../../src/algorithms/removevl.cpp \
           ../../../src/algorithms/removedesc.cpp \
           ../../../src/algorithms/remove.cpp \
           ../../../src/algorithms/rca.cpp \
           ../../../src/algorithms/pca.cpp \
           ../../../src/algorithms/normalize.cpp \
           ../../../src/algorithms/mergeregionapplier.cpp \
           ../../../src/algorithms/mergeregion.cpp \
           ../../../src/algorithms/merge.cpp \
           ../../../src/algorithms/matmult.cpp \
           ../../../src/algorithms/inverseprojection.cpp \
           ../../../src/algorithms/gaussianize.cpp \
           ../../../src/algorithms/fixlengthapplier.cpp \
           ../../../src/algorithms/fixlength.cpp \
           ../../../src/algorithms/extractapplier.cpp \
           ../../../src/algorithms/extract.cpp \
           ../../../src/algorithms/enumerateapplier.cpp \
           ../../../src/algorithms/enumerate.cpp \
           ../../../src/algorithms/dotproduct.cpp \
           ../../../src/algorithms/distribute.cpp \
           ../../../src/algorithms/cleaner.cpp \
           ../../../src/algorithms/center.cpp \
           ../../../src/algorithms/applier.cpp \
           ../../../src/algorithms/analyzer.cpp \
           ../../../src/algorithms/algoutils.cpp \
           ../../../src/algorithms/addfieldapplier.cpp \
           ../../../src/algorithms/addfield.cpp \
           ../../../src/algorithms/rename.cpp \
           ../../../src/algorithms/renameapplier.cpp \
           ../../../src/parser/lex.yy.cpp \
           ../../../src/3rdparty/libsvm/svm.cpp \
           ../../../src/3rdparty/libsvm/gaiasvmutils.cpp \
           ../../../src/3rdparty/alglib/normaldistr.cpp \
           ../../../src/3rdparty/alglib/igammaf.cpp \
           ../../../src/3rdparty/alglib/gammaf.cpp \
           ../../../src/3rdparty/alglib/chisquaredistr.cpp \
           ../../../src/3rdparty/alglib/ap.cpp \
           ../../../src/parser/filterwrapper.cpp \
           ../../../src/metrics/resistoraveragedistance.cpp

HEADERS += ../../../src/yamlcpp.h \
           ../../../src/view.h \
           ../../../src/utils.h \
           ../../../src/types.h \
           ../../../src/transformation.h \
           ../../../src/timer.h \
           ../../../src/textprogress.h \
           ../../../src/searchspacepool.h \
           ../../../src/searchspace.h \
           ../../../src/region.h \
           ../../../src/queryoptimizer.h \
           ../../../src/pointlayout.h \
           ../../../src/point.h \
           ../../../src/parameter.h \
           ../../../src/osdetect.h \
           ../../../src/linalg.h \
           ../../../src/gaiamath.h \
           ../../../src/gaiaexception.h \
           ../../../src/gaia.h \
           ../../../src/factory.h \
           ../../../src/descriptortree.h \
           ../../../src/descriptor.h \
           ../../../src/debugging.h \
           ../../../src/dataset.h \
           ../../../src/convert.h \
           ../../../src/baseexception.h \
           ../../../src/parser/parsertypes.h \
           ../../../src/parser/grammar.h \
           ../../../src/parser/filter.h \
           ../../../src/parser/token.h \
           ../../../src/metrics/weightedpearsondistance.h \
           ../../../src/metrics/weightedeuclideandistance.h \
           ../../../src/metrics/summedsegmentdistance.h \
           ../../../src/metrics/semanticclassifierdistance.h \
           ../../../src/metrics/rhythmdistance.h \
           ../../../src/metrics/randomdistance.h \
           ../../../src/metrics/nulldistance.h \
           ../../../src/metrics/cosineangledistance.h \
           ../../../src/metrics/minkowskidistance.h \
           ../../../src/metrics/manhattandistance.h \
           ../../../src/metrics/linearcombinationdistance.h \
           ../../../src/metrics/layoutawarefactory.h \
           ../../../src/metrics/kullbackleiblerdistance.h \
           ../../../src/metrics/keydistance.h \
           ../../../src/metrics/forceidentitydistance.h \
           ../../../src/metrics/exponentialcompressdistance.h \
           ../../../src/metrics/euclideandistance.h \
           ../../../src/metrics/distancefunction.h \
           ../../../src/metrics/crosssegmentdistance.h \
           ../../../src/metrics/cosinesimilarity.h \
           ../../../src/metrics/bpfdistance.h \
           ../../../src/algorithms/svmtrain.h \
           ../../../src/algorithms/svmpredict.h \
           ../../../src/algorithms/selectdesc.h \
           ../../../src/algorithms/select.h \
           ../../../src/algorithms/removevl.h \
           ../../../src/algorithms/removedesc.h \
           ../../../src/algorithms/remove.h \
           ../../../src/algorithms/rca.h \
           ../../../src/algorithms/pca.h \
           ../../../src/algorithms/normalize.h \
           ../../../src/algorithms/mergeregionapplier.h \
           ../../../src/algorithms/mergeregion.h \
           ../../../src/algorithms/merge.h \
           ../../../src/algorithms/matmult.h \
           ../../../src/algorithms/inverseprojection.h \
           ../../../src/algorithms/gaussianize.h \
           ../../../src/algorithms/fixlengthapplier.h \
           ../../../src/algorithms/fixlength.h \
           ../../../src/algorithms/extractapplier.h \
           ../../../src/algorithms/extract.h \
           ../../../src/algorithms/enumerateapplier.h \
           ../../../src/algorithms/enumerate.h \
           ../../../src/algorithms/dotproduct.h \
           ../../../src/algorithms/distribute.h \
           ../../../src/algorithms/cleaner.h \
           ../../../src/algorithms/center.h \
           ../../../src/algorithms/applier.h \
           ../../../src/algorithms/analyzer.h \
           ../../../src/algorithms/algoutils.h \
           ../../../src/algorithms/addfieldapplier.h \
           ../../../src/algorithms/addfield.h \
           ../../../src/algorithms/rename.h \
           ../../../src/algorithms/renameapplier.h \
           ../../../src/3rdparty/libsvm/gaiasvmutils.h \
           ../../../src/3rdparty/libsvm/svm.h \
           ../../../src/3rdparty/alglib/normaldistr.h \
           ../../../src/3rdparty/alglib/igammaf.h \
           ../../../src/3rdparty/alglib/gammaf.h \
           ../../../src/3rdparty/alglib/chisquaredistr.h \
           ../../../src/3rdparty/alglib/apvt.h \
           ../../../src/3rdparty/alglib/ap.h \
    ../../../src/3rdparty/Eigen/Core \
    ../../../src/3rdparty/Eigen/Dense \
    ../../../src/3rdparty/Eigen/Eigen \
    ../../../src/3rdparty/Eigen/Eigenvalues \
    ../../../src/3rdparty/MersenneTwister.h

