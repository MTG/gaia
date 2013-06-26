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

#ifndef GAIA_DATASET_H
#define GAIA_DATASET_H

#include <QVector>
#include <QReadWriteLock>
#include "point.h"
#include "pointlayout.h"
#include "transformation.h"

namespace gaia2 {

// forward-declaration of View, a bit complex as everything is now templated...
class SearchPoint;
class DistanceFunction;
class DataSet;

template <typename DataSetType, typename PointType, typename SearchPointType, typename DistanceType>
class BaseView;

typedef BaseView<DataSet, Point, SearchPoint, DistanceFunction> View;


/**
 * A PointArray is a simple subclass of a QVector<Point*> that owns the memory
 * to the points (hence will delete them when it is itself deleted).
 * NB: the PointArray can be told explicitly that it doesn't own the points, in
 *     which case it will behave the same as a simple QVector<Point*>.
 */
class PointArray : public QVector<Point*> {
 protected:
  bool _ownsMemory;

 public:

  PointArray(int n = 0, bool ownsMemory = true) : QVector<Point*>(n, 0), _ownsMemory(ownsMemory) {}
  PointArray(bool ownsMemory) : _ownsMemory(ownsMemory) {}
  ~PointArray();

  /**
   * Delete the points this array contains (if it owns them), then resize the
   * array to 0.
   */
  void clear();

  /**
   * Returns the total number of segments in this PointArray (the sum of the
   * number of segments for each point in the array).
   */
  int totalSegments() const;

  /**
   * Returns any single point from the PointArray.
   */
  const Point* samplePoint() const;
};



/**
 * This class represents a dataset and all related information.
 *
 * A DataSet is a set of points which all share the same structure. The points
 * are sorted internally to allow fast lookups in O(log(N)).
 *
 * The structure contains information such as dimension names, a pointer to the
 * original dataset, the history of the applied transformations, and a point
 * layout which maps: names of dimensions <-> indices inside the point data.
 *
 * The DataSet also provides methods for adding & removing points, merging with
 * another DataSet, and serialization functions.
 */
class DataSet : public PointArray, public QObject {

 public:
  DataSet();
  ~DataSet();

  /**
   * A lock available for users to take, if the dataset is to be used in a
   * multi-threaded context.
   */
  QReadWriteLock lock;

  /**
   * Return the name of this dataset.
   */
  const QString& name() const { return _name; }

  /**
   * Set the name for this dataset.
   */
  void setName(const QString& name) { _name = name; }

  /**
   * Find a point with a given ID.
   * @return the point with the given ID
   * @throw GaiaException when the point was not found in the dataset
   */
  const Point* point(const QString& id) const;

  /** @copydoc DataSet::point(const QString&) const */
  Point* point(const QString& id);

  /**
   * Return whether this dataset contains a point with the given ID.
   */
  bool contains(const QString& id) const;

  /**
   * Return a list of the names of the points contained in this dataset.
   */
  QStringList pointNames() const;

  /**
   * Return the layout of this dataset.
   */
  const PointLayout& layout() const { return _layout; }

  /**
   * Return the original layout of this dataset.
   */
  const PointLayout& originalLayout() const;

  /**
   * Check that all given points have the same layout object as this dataset.
   * If no points are given, it will take those from this dataset.
   * @throw GaiaException if there are some points with a different layout object
   */
  void checkAllPointsShareSameLayout(const QVector<Point*>* points = 0) const;

  /**
   * Return the history of this dataset (the list of all transformations that have been applied).
   */
  const TransfoChain& history() const { return _history; }

  /**
   * Set a predefined history for this dataset.
   * This only works on empty datasets, as it is forbidden to change the history
   * of preexisting points/datasets.
   *
   * This can be useful in the case where you copy points from one dataset into
   * another, and want to preserve their history.
   */
  void setHistory(const TransfoChain& history);

  /**
   * Reset the history to an empty one but does not touch any of the points.
   */
  void forgetHistory();

  /**
   * Simplify the history of the transformations to have it in a "normalized"
   * state, which consist at most of a Remove transformation followed by a
   * FixLength one.
   *
   * You can only call this method on a DataSet whose history contains the following
   * allowed transformations: [ Select, Remove, RemoveVL, Cleaner, FixLength ].
   */
  void simplifyHistory();



  /**
   * Set the reference dataset, ie: the one used for fetching the values when
   * doing filtered queries.
   * Passing it a null pointer (or no argument) will set the calling dataset
   * as a reference dataset.
   * It is highly recommended to always check for the original layout, as it is
   * impossible to add a point to 2 linked datasets that don't have the same
   * original layout.
   */
  void setReferenceDataSet(DataSet* dataset = 0, bool checkOriginalLayout = true);

  /**
   * Return the reference dataset.
   * A dataset always has a reference dataset, where it looks for the values
   * used when filtering queries. If no dataset has been set as a reference
   * dataset, the current dataset will be used.
   */
  const DataSet* referenceDataSet() const;

  /**
   * Add the specified point to this dataset. The dataset makes a copy of
   * the point for its own use, so no ownership is taken. If you don't want the
   * point anymore after having added it to the dataset, it is your
   * responsibility to free the memory for it.
   *
   * @throw GaiaException if there was already a point with the same
   * ID in this dataset.
   */
  void addPoint(const Point* point);

  /**
   * Add the specified points to this dataset. The dataset makes a copy of
   * the points for its own use, so no ownership is taken. This method is
   * equivalent to calling addPoint repeatedly, but is much faster.
   *
   * @exception GaiaException if adding all points would result in a dataset
   *                          with duplicates.
   */
  void addPoints(const QVector<Point*>& points);

  /**
   * Append the points from the second dataset to the first one. They must have
   * the same layout and transformation history for this to work.
   *
   * @exception GaiaException if there were duplicate IDs in the 2 dataset, if
   *                          the layouts were not the same or if the transformation
   *                          histories were not the same.
   */
  void appendDataSet(const DataSet* dataset);

  /**
   * Remove a single point from the dataset given its ID.
   *
   * @throw GaiaException if no point could be found with this ID.
   */
  void removePoint(const QString& id);

  /**
   * Remove a list of points from the dataset given their IDs. This is much
   * faster than calling DataSet::removePoint() repeatedly.
   *
   * @throw GaiaException if at least one ID could not be found. In that case,
   *                      no points will have been removed.
   */
  void removePoints(const QList<QString>& ids);




  /**
   * Register a view on this dataset. Registered Views are notified when the
   * underlying dataset change (ie: points are added, removed, ...).
   */
  void addView(View* view);

  /**
   * Remove a View from the list of registered views for this dataset.
   */
  void removeView(View* view);




  /**
   * Take a map of pairs (pointID, filename) and merges the
   * signature files in a single dataset and returns it.
   */
  static DataSet* mergeFiles(const QMap<QString, QString>& sigfiles,
                             const QStringList& descsSelect = QStringList() << "*",
                             const QStringList& descsExclude = QStringList(),
                             int start = 0, int end = 10000000,
                             PointLayout* reflayout = 0);

  /**
   * Performs a deep copy of this dataset (ie: the contained points are copied as well).
   */
  DataSet* copy() const;

  /**
   * Load a dataset from disk.
   * A value of end < 0 means that we should load all the points.
   *
   * @param filename the path to the dataset file
   * @param start index of the first point to be loaded
   * @param end the index of the last point to be loaded + 1 (ie: past iterator)
   */
  void load(const QString& filename, int start = 0, int end = -1);

  /**
   * Load the n-th part of a dataset from disk.
   *
   * @param filename the path to the dataset file
   * @param idx the index of the part to be loaded (0 <= idx < total)
   * @param total the number of parts in which the dataset should be split.
   */
  void loadNthPart(const QString& filename, int idx = 0, int total = 1);

  /**
   * Save this dataset to disk.
   */
  void save(const QString& filename) const;

  /**
   * Load a DataSet from its base64 representation.
   */
  void fromBase64(const std::string& data);

  /**
   * Load a DataSet from its base64 representation.
   */
  void fromBase64(const QByteArray& data);

  /**
   * Return a base64 representation for this DataSet.
   */
  std::string toBase64() const;



 protected:

  /**
   * Represents the name of the dataset, which should be a short way to
   * describe its function or where it comes from, its purpose, etc...
   * This information is only used for debugging purposes.
   */
  QString _name;

  /**
   * This represents the history of transformations that have been applied to
   * this dataset, and also contains all the parameters to allow mapping a
   * point from the original dataset space into the space this dataset is in.
   * For more information on this structure, refer to the
   * Transformation class.
   */
  TransfoChain _history;

  /**
   * This represents the common layout of all points contained in this dataset
   * and provides functions for retrieving the physical location of a descriptor
   * given its name, and reciprocally, retrieving the name of a descriptor given
   * its physical location.
   */
  PointLayout _layout;

  /**
   * This list contains all datasets linked to this one, like when a dataset
   * is referencing another one for the original values used when filtering.
   * This is necessary, because when we add a point in a dataset, we need to add
   * it as well in the referenced datasets and further in all the datasets
   * referencing it, etc... So we need to create a set of linked datasets, which
   * will be stored entirely in each one of these linked datasets.
   * The first element of this list holds a specific role as well: it is the
   * dataset which holds the reference values used when filtering.
   */
  QList<DataSet*> _linkedDataSets;

  /**
   * This list contains all Views linked to this DataSet, ie: the Views using
   * points from this dataset to do their queries. We need this, because when
   * add points, this invalidates the Views pointing on the dataset, so we need
   * a way to inform them.
   */
  QList<View*> _linkedViews;

  bool _isDataSorted;

  /**
   * Looks for point with name id, between indices start and end included.
   */
  int binarySearch(const QString& id, int start, int end) const;

  /**
   * Returns the index of the point with the given name.
   *
   * @exception GaiaException if the point name could not be found.
   */
  int pointIndex(const QString& id) const;

  void clear();

  /**
   * Returns the number of points in the dataset (not the number of points loaded).
   */
  int load(QDataStream& in, int start = 0, int end = -1, bool readAllPointsFromStream = false);

  void setLayoutIfEmpty(const Point* point);

  void invalidateViews();

  void modify();

  void setHistoryNoCheck(const TransfoChain& history) { _history = history; }

  void addTransformation(const Transformation& transfo);

  bool consistentLinks() const;

  void forceUnlinkReferringDataSets();

  /**
   * Make all the points in this DataSet share the same layout.
   */
  void unifyLayout();

  /**
   * Checks that all point names are unique (ie: there are no duplicates in this
   * dataset). A dataset with duplicate IDs is invalid and can lead to
   * crashes.
   *
   * @exception GaiaException if multiple points were found with the same ID.
   */
  void checkUniqueIDs();

  /**
   * Checks that all point names inside @c v are unique and also that none of them
   * is already in this dataset. As @c DataSet derives from @c QVector<Point*>
   * you can also pass a @c DataSet instance to this method.
   *
   * @exception GaiaException if either the given list of points contains duplicates
   *            or if one of them was found in this dataset.
   */
  void checkUniqueIDsFrom(const QVector<Point*>& v);


  /**
   * Adds the given points to his dataset.
   *
   * @param layoutCheck whether to check that the layouts of all points are
   *        compatible with that of the dataset. In case a point has a layout
   *        which is incompatible, it will throw an exception and no points will
   *        have been added.
   *
   * @param transformPoint whether to apply the history of transformations to
   *        the points we're adding, or to insert them directly as is in the dataset.
   *        In the former case, the layouts (if checked) need to be the same as
   *        the original layout of the dataset, in the former, they need to be
   *        the same as the current layout of the dataset.
   *
   * @param checkUnique whether to check for uniqueness condition in point names.
   *        A dataset is only valid if all points inside it have different names,
   *        so this makes sure that after adding the given points, we still have
   *        a valid dataset. It throws an exception otherwise.
   *
   * @param takeOwnership whether this method needs to make a copy of the given
   *        points or not. @b WARNING: if takeOwnership = true, there is no
   *        guarantee that the pointers in the vector are still valid after this
   *        call (ie: not only should you not delete them, but you should also
   *        stop using them directly afterwards).
   *
   * @param relaySignal whether this should be applied to all linked datasets
   *        or only to this one.
   */
  void addPoints(const QVector<Point*>& points,
                 bool layoutCheck,
                 bool transformPoint,
                 bool checkUnique = true,
                 bool takeOwnership = false,
                 bool relaySignal = true);

  void removePoints(const QList<QString>& ids, bool relaySignal);

  void sortPoints(int pivotIdx = -1);

  // make both the copy-constructor and assignment operator protected, so we
  // can ensure that no copy of a dataset can be made unintentionally (which could
  // lead to either memory problems or terrible performance)
  // people wanting to make a copy of a DataSet should use the DataSet::copy() method.
  DataSet(const DataSet& rhs);
  DataSet& operator=(const DataSet& rhs);

  // those functions from the parent class should also not be callable
  void resize(int n);

 public:

  friend class Applier; // to access the transformation history

  // mergeDataSets needs to have direct access to a DataSet to work properly
  // because it needs to call the setHistoryNoCheck() method.
  friend DataSet* mergeDataSets(const DataSet* ds1, const DataSet* ds2);


  // serialization methods
  friend QDataStream& operator<<(QDataStream& out, const DataSet& dataset);
  friend QDataStream& operator>>(QDataStream& in, DataSet& dataset);

};


} // namespace gaia2

#endif // GAIA_DATASET_H
