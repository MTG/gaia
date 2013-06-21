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
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see http://www.gnu.org/licenses/
 */
#include "matmult.h"
#include "utils.h"
using namespace std;
using namespace gaia2;
using namespace Eigen;

MatMult::~MatMult() {}

Region regionFromNames(const PointLayout& layout, const QStringList& descriptorNames) {
  QStringList allNames = layout.descriptorNames(RealType, descriptorNames);
  Region region = layout.descriptorLocation(allNames);
  region.checkTypeOnlyFrom(RealType, &layout);
  return region;
}

MatMult::MatMult(const Transformation& transfo) : Applier(transfo) {
  // check we only have real descriptors here
  _descriptorNames = transfo.params.value("descriptorNames").toStringList();
  _matrix = transfo.params.value("matrix").toMatrix();

  // prepare layout
  QString newName = transfo.params.value("resultName", "").toString();

  // to ensure compatibility with histories from Gaia <= 2.2.2.1
  if (newName.isEmpty()) {
    int targetDimension = _matrix.rows();

    newName =
      QString("%1 %2 (%3)")
      .arg(transfo.analyzerName)
      .arg(targetDimension)
      .arg(_descriptorNames.join(", "));

    newName.replace(".", "_"); // it is a bad idea to have dots "." in a descriptor name...

    G_DEBUG(GAlgorithms, "new name:" << newName);
  }

  _layout.add(newName, RealType);
  _layout.fixLength(newName, _matrix.rows());

  // precompute region in original layout
  _regionCache = regionFromNames(_transfo.layout, _descriptorNames);
}


void matrixMultiply(const RealDescriptor& x, const MatrixXf& matrix,
                    RealDescriptor& result) {

  Q_ASSERT(x.size() == matrix.cols());
  int targetDimension = matrix.rows();
  result.resize(targetDimension);

  VectorXf::Map(result.data(), targetDimension) = matrix * VectorXf::Map(x.data(), x.size());
}

Point* MatMult::mapPoint(const Point* p, const Region& region) const {
  Point* result = new Point;
  result->setName(p->name());
  result->setLayout(_layout, p->numberSegments());

  FORSEG(*result) {
    RealDescriptor merged = mergeDescriptors(p, nseg, region);
    RealDescriptor& resultData = result->frealData(nseg); // only element available

    matrixMultiply(merged, _matrix, resultData);
  }

  return result;

}

Point* MatMult::mapPoint(const Point* p) const {
  checkLayout(p->layout());
  return mapPoint(p, _regionCache);
}
