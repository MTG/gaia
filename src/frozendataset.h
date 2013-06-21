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
#ifndef GAIA_FROZENDATASET_H
#define GAIA_FROZENDATASET_H

#include <QString>
#include <QVector>
#include "types.h"

namespace gaia2 {

class DataSet;
class FrozenDataSet;
class FrozenSearchPoint;
class FrozenDistance;

template <typename DataSetType, typename PointType, typename SearchPointType, typename DistanceType>
class BaseView;

typedef BaseView<FrozenDataSet, FrozenPoint, FrozenSearchPoint, FrozenDistance> FrozenView;


/**
 * A FrozenDataSet is a dataset that has been flagged as immutable. No more fields
 * can be added, nor can any field be removed from it (hence the name frozen).
 * There are also conditions that need to be filled before creating a FrozenDataSet,
 * such as the fact that it only accepts real-type fixed-length descriptors at the moment.
 * It has however a few other advantages over a normal dataset, namely that the memory
 * used for it is contiguous (and thus much faster to iterate over, such as in the case
 * of nn-queries).
 */
class FrozenDataSet : public MatrixRXf, public QObject {
public:
  FrozenDataSet();
  FrozenDataSet(int npoints, int dimension);

  /**
   * Load a dataset from disk.
   */
  void load(const QString& filename);

  /**
   * Save this dataset to disk.
   */
  void save(const QString& filename) const;

  /**
   * Return the number of points contained in this dataset.
   */
  int size() const;

  /**
   * Return the number of dimensions of this dataset.
   */
  int dimension() const;


  QStringList pointNames() const;

  /**
   * Return the name of the point with the given index.
   */
  const QString& pointName(int i) const;

  /**
   * Return the index of the point with the given name.
   * @throw GaiaException if no such point exists
   */
  int pointIndex(const QString& name) const;

  FrozenPoint point(const QString& name) const;

  /**
   * Return the column indices of beginning and end fo the region spanning the descriptor.
   * NOTE: contrary to a normal dataset's layout, here you always have to give the
   *       fully-qualified name.
   */
  QPair<int, int> descriptorLocation(const QString& descriptorName) const;

  QStringList descriptorNames() const;

  /**
   * Note: this method is not made static as a named-constructor, because then we would
   *       have to return the FrozenDataSet by value which would be very inefficient.
   *       There could be 2 solutions to this, none of which seems really worth investing
   *       the time in it (curiosity would of course want me to do it, but oh well... :-) )
   *       - expression templates
   *       - C++0x move semantics
   */
  void fromDataSet(const DataSet& dataset);

  /**
   * Checks whether the given dataset would make a valid FrozenDataSet. If anything
   * prevents if from being so, this method throws a GaiaException explaining why,
   * otherwise it just returns silently.
   */
  static void checkValidFrozenDataSet(const DataSet& dataset);

  /**
   * Set the reference dataset, ie: the one used for fetching the values when
   * doing filtered queries.
   * Passing it a null pointer (or no argument) will mark this dataset has having no
   * reference dataset.
   */
  void setReferenceDataSet(DataSet* dataset = 0);

  /**
   * Return the reference dataset. If no reference dataset has been previously set,
   * then this method will throw an exception.
   */
  const DataSet* referenceDataSet() const;

  ///////////// NOT IMPLEMENTED ATM, BUT REQUIRED FOR THE TEMPLATES TO COMPILE

  void addView(FrozenView* view) { Q_UNUSED(view); }
  void removeView(FrozenView* view) { Q_UNUSED(view); }

protected:
  /**
   * This variable records the names of the points. It should always be the same size
   * as the number of rows contained in this FrozenDataSet.
   */
  QVector<QString> _names;

  // map of desc -> (start, end) indices
  QMap<QString, QPair<int, int> > _layout;

  DataSet* _referenceDataSet;

};


} // namespace gaia2

#endif // GAIA_FROZENDATASET_H
