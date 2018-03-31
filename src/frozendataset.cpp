/* 
 * Copyright (C) 2006-2013  Music Technology Group - Universitat Pompeu Fabra
 *
 * This file is part of Gaia
 * 
 * Gaia is free software: you can redistribute it and/or modify it under 
 * the terms of the GNU Affero General Public License as published by the Free 
 * Software Foundation (FSF), either version 3 of the License, or (at your 
 * option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT 
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more 
 * details.
 * 
 * You should have received a copy of the Affero GNU General Public License
 * version 3 along with this program.  If not, see http://www.gnu.org/licenses/
 */

/* <copyright entity="UPF">
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
*/


#include <QFile>
#include "frozendataset.h"
#include "dataset.h"
#include "utils.h"
using namespace gaia2;

FrozenDataSet::FrozenDataSet() : MatrixRXf(), _referenceDataSet(0) {}

FrozenDataSet::FrozenDataSet(int npoints, int dimension)
  : MatrixRXf(npoints, dimension), _names(npoints), _referenceDataSet(0) {}


int FrozenDataSet::size() const {
  return rows();
}

int FrozenDataSet::dimension() const {
  return cols();
}


QStringList FrozenDataSet::pointNames() const {
  QStringList result;
  for (int i=0; i<_names.size(); i++) result << _names[i];
  return result;
}

const QString& FrozenDataSet::pointName(int i) const {
  return _names.at(i);
}

int FrozenDataSet::pointIndex(const QString& name) const {
  QVector<QString>::const_iterator pos = qBinaryFind(_names, name);
  if (pos == _names.constEnd()) {
    throw GaiaException("Unable to find point with id '", name, "'");
  }

  return pos - _names.constBegin();
}

FrozenPoint FrozenDataSet::point(const QString& name) const {
  return row(pointIndex(name));
}


#define MAGIC_NUMBER (quint32)0x6AEA7231

void FrozenDataSet::load(const QString& filename) {
  QFile file(filename);
  if (!file.open(QIODevice::ReadOnly)) {
    throw GaiaException("Could not open file: ", filename);
  }

  QDataStream in(&file);

  // read magic number and check stream version
  quint32 magic;
  in >> magic;

  if (magic != MAGIC_NUMBER) {
    throw GaiaException("File does not seem to be a Gaia frozen dataset file.");
  }

  checkValidDataStream(in);

  // read matrix dimensions
  quint32 npoints, dimension;
  in >> npoints >> dimension;

  resize(npoints, dimension);

  // read matrix data
  for (int i=0; i<(int)npoints; i++) {
    for (int j=0; j<(int)dimension; j++) {
      in >> (*this)(i, j);
    }
  }

  // read point names
  _names.resize(npoints);
  for (int i=0; i<(int)npoints; i++) {
    in >> _names[i];
  }

  // read layout
  _layout.clear();
  QString desc;
  quint32 ndescs, start, end;
  in >> ndescs;
  for (int i=0; i<(int)ndescs; i++) {
    in >> desc >> start >> end;
    // FIXME: remove me, transitional hack/fix
    if (!desc.startsWith(".")) desc.prepend(".");
    _layout.insert(desc, qMakePair<int, int>(start, end));
  }
}


void FrozenDataSet::save(const QString& filename) const {
  QFile file(filename);
  if (!file.open(QIODevice::WriteOnly)) {
    throw GaiaException("Could not open file: ", filename);
  }

  QDataStream out(&file);

  // write header with magic number and version
  out << MAGIC_NUMBER;
  setDataStreamVersion(out);

  const int npoints = rows();
  const int dimension = cols();

  // write matrix dimensions
  out << (quint32)npoints << (quint32)dimension;

  // write matrix data
  for (int i=0; i<npoints; i++) {
    for (int j=0; j<dimension; j++) {
      out << (*this)(i, j);
    }
  }

  // write point names
  for (int i=0; i<npoints; i++) {
    out << _names[i];
  }

  // write layout
  out << (quint32)_layout.size();
  QMap<QString, QPair<int, int> >::const_iterator it = _layout.constBegin();
  for (; it != _layout.constEnd(); ++it) {
    out << it.key() << it.value().first << it.value().second;
  }
}


void FrozenDataSet::fromDataSet(const DataSet& dataset) {
  checkValidFrozenDataSet(dataset);

  // now we're sure we have a fixed-length dataset, we can get the dimension out of it

  int npoints = dataset.size();
  int dimension = dataset.layout().correspondingRegion().dimension();

  resize(npoints, dimension);
  _names.resize(npoints);
  _layout.clear();

  foreach (const QString& desc, dataset.layout().descriptorNames()) {
    Segment descloc = dataset.layout().descriptorLocation(desc).segment();
    _layout.insert(desc, qMakePair(descloc.begin, descloc.end));
  }

  for (int i=0; i<npoints; i++) {
    _names[i] = dataset[i]->name();
    const RealDescriptor& d = dataset[i]->frealData();
    this->row(i) = Eigen::VectorXf::Map(&d[0], d.size());
  }
}

QPair<int, int> FrozenDataSet::descriptorLocation(const QString& descriptorName) const {
  if (!_layout.contains(descriptorName) && !_layout.contains(QString(".") + descriptorName)) {
    throw GaiaException("Could not find descriptor '", descriptorName, "' in this dataset... Available descriptors: ", _layout.keys());
  }

  if (_layout.contains(descriptorName))
    return _layout.value(descriptorName);
  else
    return _layout.value(QString(".") + descriptorName);
}

QStringList FrozenDataSet::descriptorNames() const {
  return _layout.keys();
}


void FrozenDataSet::checkValidFrozenDataSet(const DataSet& dataset) {
  // Check that the dataset we have can be converted
  PointLayout layout = dataset.layout();

  QStringList vlength;
  foreach (const QString& name, layout.descriptorNames()) {
    if (layout.descriptorLocation(name).lengthType() == VariableLength) vlength << name;
  }

  if (!vlength.empty()) {
    throw GaiaException("The following descriptors are variable-length: ", vlength,
                        "\nCan only freeze a dataset which is entirely fixed-length at the moment...");
  }

  if (!layout.descriptorNames(StringType).empty() || !layout.descriptorNames(EnumType).empty()) {
    throw GaiaException("Can only freeze datasets which contain only real descriptors (ie: no strings)");
  }

}


void FrozenDataSet::setReferenceDataSet(DataSet* dataset) {
  // NOTE: we do not check the linked datasets here, as a frozen dataset cannot be changed
  //       if the linked reference dataset is changed, though, the behavior is undefined
  //       (this meaning you'll either get a crash if you're lucky or data currution if
  //        you're not)

  // first check that this dataset is a valid reference dataset
  const int npoints = this->size();
  if (dataset->size() != npoints) {
    throw GaiaException("Cannot set the given dataset as a reference dataset as it doesn't "
                        "have the same number of points.");
  }

  // make sure all the points are the same
  for (int i=0; i<npoints; i++) {
    if (_names[i] != dataset->at(i)->name()) {
      throw GaiaException("Cannot set the given dataset as a reference dataset as they don't "
                          "have the same points.");
    }
  }

  _referenceDataSet = dataset;
}

const DataSet* FrozenDataSet::referenceDataSet() const {
  if (!_referenceDataSet) throw GaiaException("No reference dataset has been set.");
  return _referenceDataSet;
}

