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
#ifndef GAIA_APPLIER_H
#define GAIA_APPLIER_H

#include "dataset.h"
#include "factory.h"
#include "transformation.h"
#include "pointlayout.h"
#include "textprogress.h"

namespace gaia2 {

/**
 * @ingroup appliers
 * The Applier abstract base class. Derived classes will have to implement a
 * constructor taking all its arguments as a string, and a mapPoint method.
 * They will also have to set _originalDimension and _targetDimension in
 * the constructor, as well as the other specific params they need for
 * themselves. A helper function checkSize should be called at the very
 * beginning of the mapPoint method to ensure consistency in error
 * reporting.
 * Do not forget as well to call the transferHistory() helper method at the end
 * of your mapDataSet() function, or you might end up with invalid datasets
 * (NB: it needs to be at the end, not at the beginning).
 */
class Applier {
 public:

  Applier(const Transformation& transfo) : _transfo(transfo) {}

  virtual ~Applier() {};

  /**
   * Check that the given layout is the same as the one that was used during
   * the analysis part of this transformation.
   *
   * @throw GaiaException if the layout was different than the current one.
   */
  void checkLayout(const PointLayout& layout) const;

  /**
   * Check that the given dataset has the same layout as the one that was used during
   * the analysis part of this transformation.
   * Also check that the dataset is consistent.
   *
   * @throw GaiaException if the layout was different than the current one.
   */
  void checkLayout(const DataSet* dataset) const;

  /**
   * Ownership of resulting point is handed to caller of this function.
   */
  virtual Point* mapPoint(const Point* p) const = 0;

  /**
   * Provides a default function that maps a whole dataset but which can be
   * overriden in derived classes to provide a more efficient implementation.
   */
  virtual DataSet* mapDataSet(const DataSet* dataset) const;

 protected:
  /**
   * This method tranfers the transformation history from the original dataset
   * to the one being created, and adds itself at the end of the history.
   */
  void transferHistory(const DataSet* original, DataSet* destination) const;

  /**
   * Adds the given points to a DataSet without checking for the layout to be
   * correct. The guarantee that it is correct is provided by the Applier
   * algorithm.
   */
  void addPointsNoLayoutCheck(DataSet* dataset, const QVector<Point*>& points) const;

  Transformation _transfo;

};

typedef Factory<QString, Applier, Transformation> ApplierFactory;

void registerAppliers();

} // namespace gaia2


/** macro to easily write a custom mapDataSet function */
#define MAPDATASET_PREPARE                                        \
  checkLayout(dataset);                                           \
  DataSet* result = new DataSet;                                  \
  result->setName(dataset->name());                               \
                                                                  \
  TextProgress info(dataset->size(),                              \
                    QString("Applying '") + _transfo.analyzerName \
                    + "' transformation [%1/%2] (%3% done)",      \
                    UpdateOnPercent);                             \
  info.setSilent(!gaia2::verbose);                                \
  int npoint = 0;

/** macro to easily write a custom mapDataSet function */
#define MAPDATASET_LOOP_POINT                                     \
  QVector<Point*> v(dataset->size());                             \
  for (int i=0; i<dataset->size(); i++) {                         \
    info.update(++npoint);                                        \
    const Point* p = dataset->at(i);

/** macro to easily write a custom mapDataSet function */
#define MAPDATASET_END_LOOP_NO_RETURN                             \
    v[i] = newp;                                                  \
  }                                                               \
  addPointsNoLayoutCheck(result, v);                              \
  transferHistory(dataset, result);


/** macro to easily write a custom mapDataSet function */
#define MAPDATASET_END_LOOP                                       \
  MAPDATASET_END_LOOP_NO_RETURN;                                  \
  return result;


#endif // GAIA_APPLIER_H
