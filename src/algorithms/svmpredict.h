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

#ifndef GAIA_SVMPREDICT_H
#define GAIA_SVMPREDICT_H

#include <QByteArray>
#include "applier.h"
#include "3rdparty/libsvm/svm.h"

namespace gaia2 {

/**
 * @ingroup appliers
 * SVMPredict applier class. Predicts the class of a point using a given SVM model.
 */
class SVMPredict : public Applier {

 public:
  SVMPredict(const Transformation& transfo);
  virtual ~SVMPredict();

  /**
   * Ownership of resulting point is handed to caller of this function.
   */
  virtual Point* mapPoint(const Point* p) const;

  virtual DataSet* mapDataSet(const DataSet* dataset) const;

 protected:
  Point* svmPredict(const Point* p, Point* result,
                    const struct svm_model* model, const Region& region,
                    DescriptorLengthType classLType, int classIdx,
                    DescriptorLengthType classProbLType, int classProbIdx) const;

  struct svm_model* loadModel() const;

  QStringList _classMapping;
  QString _className;
  QStringList _descriptorNames;
  QByteArray _modelData;
  bool _probability;
  struct svm_model* _model;
  int* _cmap; // reverse class map, used for getting correct class number from libsvm probabilities
};

} // namespace gaia2

#endif // GAIA_SVMPREDICT_H
