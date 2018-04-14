%module gaia2
%{
#include <cstddef>
%}

#pragma SWIG nowarn=503, 389, 362, 401, 302, 509

%ignore lock;

%pythonbegin %{
# Copyright (C) 2006-2013  Music Technology Group - Universitat Pompeu Fabra
#
# This file is part of Gaia
#
# Gaia is free software: you can redistribute it and/or modify it under
# the terms of the GNU Affero General Public License as published by the Free
# Software Foundation (FSF), either version 3 of the License, or (at your
# option) any later version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the Affero GNU General Public License     
# version 3 along with this program. If not, see http://www.gnu.org/licenses/

# <copyright entity="UPF">
# UPF. All Right Reserved, http://www.upf.edu/
#
# This source is subject to the Contributor License Agreement of the Essentia project.
# Please see the CLA.txt file available at http://essentia.upf.edu/contribute/
# for more
# information.
# 
# THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY 
# KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
# PARTICULAR PURPOSE.
#
# </copyright>

from __future__ import division
from builtins import str
from builtins import zip
from builtins import map
from builtins import range
from past.builtins import basestring as old_basestring
from past.builtins import unicode as old_unicode
from builtins import object

%} //# %pythonbegin



%include "std_string.i"
%include "std_pair.i"
%include "std_iostream.i"
%include "std_vector.i"
%include "std_pair.i"
%include "std_map.i"
%include "std_string.i"

%include "docstring.i"
%include "docstring_factory.i"

// tell SWIG about ownership of objects & stuff
// why the hell this needs to go here and not at the top of transformation.i
// I can't understand, but maybe %include'ing all the headers is too naive a
// way of wrapping gaia's classes...
%newobject *::applyToDataSet;

%newobject *::nnSearch;

%newobject applyTransfoChain;
%newobject *::applyTransfoChain;


namespace std {
  %template(VectorD) std::vector<double>;
  %template(MatrixD) std::vector<std::vector<double> >;
  %template(SearchResultsPair) std::pair<std::string, float>;
  %template(FloatVector) std::vector<float>;
  %template(IntVector) std::vector<int>;
  %template(StringVector) std::vector<std::string>;
  %template(SearchResultsVector) std::vector<std::pair<std::string, float> >;
  %template(CoordVector) std::vector<std::pair<float, float> >;
  %template(StringMap) std::map<std::string, std::string>;
}

%exception {
  try {
    $action
  }
  catch (std::exception& e) {
    PyErr_SetString(PyExc_Exception, e.what());
    SWIG_fail;
  }
  catch (ap::ap_error& e) {
    PyErr_SetString(PyExc_Exception, e.msg.c_str());
    SWIG_fail;
  }
}


%{
#include "gaia.h"
#include "utils.h"
#include "3rdparty/alglib/ap.h" // for ap_error
#include "convert.h"
#include "region.h"
#include "pointlayout.h"
#include "view.h"
#include "frozendistance.h"
#include "gaiaexception.h"
#include "factory.h"
#include "metrics/layoutawarefactory.h"
#include "metrics/distancefunction.h"
#include "transformation.h"
#include "algorithms/analyzer.h"
#include "linalg.h"
#include "parser/filterwrapper.h"
#include <string>
#include <map>
#include "swigshadow.h"
using namespace gaia2;

// TODO: add as a parameter the list of descriptors that go into the region
inline std::vector<std::vector<double> > covarianceMatrix(const gaia2::DataSet* dataset) {
  const PointLayout& layout = dataset->layout();
  Region region = layout.descriptorLocation(selectDescriptors(layout, RealType));

  Eigen::MatrixXd covmat = covarianceMatrix(*dataset, region);
  const int size = covmat.rows();

  std::vector<std::vector<double> > result(size,
                                           std::vector<double>(size, 0));

  for (int i=0; i<size; i++) {
    for (int j=0; j<size; j++) {
      result[i][j] = covmat(i, j);
    }
  }

  return result;
}


// declare this here, definition is in parameter.i
PyObject* parameterToPython(const QVariant* p);
PyObject* pmapToPython(const ParameterMap* pm);

%}


%include "../gaia.h"
%include "../debugging.h"
%include "../types.h"
%include "../gaiaexception.h"
%include "../descriptor.h"
%include "../parameter.h"
%include "../searchspace.h"

%include "../region.h"
%include "pointlayout.i"
%include "../pointlayout.h"
%include "../transformation.h"
%include "../algorithms/analyzer.h"
%include "../metrics/distancefunction.h"

%init %{
  init();
%}


%include "point.i"
%include "region.i"
%include "dataset.i"
%include "frozendataset.i"
%include "frozenview.i"
%include "searchspace.i"
%include "../view.h"
%include "view.i"

namespace std {
  %template(VectorPoint) std::vector<gaia2::Point*>;
}

%template(SegmentVector) std::vector<gaia2::Segment>;
%template(RealDescriptorArray) gaia2::Array<gaia2::RealDescriptor>;
%template(RealDescriptor) gaia2::SingleValueMorphable<gaia2::Real>;
%template(StringDescriptor) gaia2::SingleValueMorphable<QString>;
%template(VectorFilter) std::vector<gaia2::Filter*>;

%template(ResultSet) gaia2::BaseResultSet<gaia2::SearchPoint, gaia2::DataSet>;
%template(FrozenResultSet) gaia2::BaseResultSet<gaia2::FrozenSearchPoint, gaia2::FrozenDataSet>;
%template(View) gaia2::BaseView<gaia2::DataSet, gaia2::Point, gaia2::SearchPoint, gaia2::DistanceFunction>;
%template(FrozenView) gaia2::BaseView<gaia2::FrozenDataSet, gaia2::FrozenPoint, gaia2::FrozenSearchPoint, gaia2::FrozenDistance>;

%include "parameter.i"
%include "descriptor.i"
%include "factory.i"
%include "analyzer.i"
%include "layoutawarefactory.i"
%include "transformation.i"

%include "../types.h"
%include "../descriptor.h"
%include "../region.h"
%include "../pointlayout.h"
%include "../point.h"
%include "../dataset.h"
%include "../searchspace.h"
%include "../view.h"
%include "../frozendataset.h"
%include "../metrics/frozendistance.h"
%include "../transformation.h"
%include "../algorithms/analyzer.h"
%include "../metrics/distancefunction.h"
%include "../parser/filter.h"
%include "../parser/filterwrapper.h"

// only functions from utils.h that should be usable from python
void checkIsomorphDataSets(const gaia2::DataSet* dataset1, const gaia2::DataSet* dataset2);
gaia2::PointLayout mergeLayouts(const gaia2::PointLayout& layout1, const gaia2::PointLayout& layout2);
gaia2::DataSet* mergeDataSets(const gaia2::DataSet* ds1, const gaia2::DataSet* ds2);

// from linalg.h
inline std::vector<std::vector<double> > covarianceMatrix(const gaia2::DataSet* dataset);

// from gaiamath.h
double erfinv(double P);
double norminv(double P, double mu, double sigma);
double chi2inv(double P, int v);


%delobject delete_QString;

%inline %{
void delete_QString(QString* str) {
  delete str;
}
%}



// functions & types wrappers to make the python API more pythonic than the C++ one
%include pythonic.i
