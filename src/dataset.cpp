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

#include <QFile>
#include <QBuffer>
#include <QMutex>
#include <QFuture>
#include <QtConcurrentMap>
#include "dataset.h"
#include "utils.h"
#include "view.h"
#include "textprogress.h"
using namespace gaia2;


// comparison function for strings
inline bool nameLessThan(const QString& s1, const QString& s2) {
  return s1 < s2;
}

// comparison function for point IDs
inline bool pidLessThan(const Point* p1, const Point* p2) {
  return nameLessThan(p1->name(), p2->name());
}


PointArray::~PointArray() { clear(); }

void PointArray::clear() {
  if (_ownsMemory) {
    for (int i=0; i<size(); i++) delete at(i);
  }
  QVector<Point*>::clear();
}


int PointArray::totalSegments() const {
  int total = 0;
  for (int i=0; i<size(); i++) {
    total += at(i)->numberSegments();
  }
  return total;
}

const Point* PointArray::samplePoint() const {
  if (isEmpty()) {
    throw GaiaException("Could not get sample point because dataset is empty...");
  }

  return at(0);
}




DataSet::DataSet() : _isDataSorted(true) {
  setReferenceDataSet(this);
}

DataSet::~DataSet() {
  G_DEBUG(GMemory, "deleting dataset with name" << _name);
  try {
    forceUnlinkReferringDataSets();
    clear();
  }
  catch (GaiaException& e) {
    qDebug() << "FATAL ERROR: An exception occurred in a DataSet destructor: " << e.what();
    qDebug() << "Aborting...";
    exit(1);
  }
  G_DEBUG(GMemory, "done deleting dataset with name" << _name);
}

void DataSet::forceUnlinkReferringDataSets() {
  bool modified;
  do {
    modified = false;
    foreach (DataSet* ds, _linkedDataSets) {
      if (ds != this && ds->referenceDataSet() == this) {
        qDebug() << QString("WARNING: forcing removing reference from dataset '%1' to dataset '%2'")
          .arg(ds->name()).arg(this->name()).toUtf8().constData();
        ds->setReferenceDataSet(ds);
        modified = true;
        break;
      }
    }
  } while (modified);
}

void DataSet::clear() {
  // first, unlink this dataset from all the others (if any)
  setReferenceDataSet(this);

  // invalidate all views
  invalidateViews();

  // deleting the points data is now done by PointArray
  PointArray::clear();
}


void DataSet::addTransformation(const Transformation& transfo) {
  _history.append(transfo);
}


int DataSet::pointIndex(const QString& id) const {
  if (!_isDataSorted) const_cast<DataSet*>(this)->sortPoints();

  int idx = binarySearch(id, 0, size()-1);
  if ((idx < size()) && (at(idx)->name() == id)) return idx;

  throw GaiaException("Unable to find point with id '", id, "'");
}

const Point* DataSet::point(const QString& id) const {
  return at(pointIndex(id));
}

Point* DataSet::point(const QString& id) {
  return at(pointIndex(id));
}

bool DataSet::contains(const QString& id) const {
  try {
    pointIndex(id);
    return true;
  }
  catch (GaiaException&) {
    return false;
  }
}


QStringList DataSet::pointNames() const {
  QStringList result;
  for (int i=0; i<size(); i++) {
    result << at(i)->name();
  }
  return result;
}


const PointLayout& DataSet::originalLayout() const {
  if (_history.isEmpty()) return _layout;
  return _history.at(0).layout;
}

void DataSet::setLayoutIfEmpty(const Point* p) {
  // for the first point we add, we need to set the layout for the dataset and
  // all the collections in there
  if (empty()) {
    G_DEBUG(GDataSet, "first point!");

    // need to check whether we have a custom history, or if we insert the point
    // in an untouched dataset
    if (_history.size() == 0) {
      _layout = p->layout();
    }
    else {
      // we already have an history: in that case, the layout of the dataset is
      // the layout of the _mapped_ point, and we do _not_ touch the history's
      // original layout
      G_DEBUG(GDataSet, "History already set, not touching it");

      // NB: this conflicts with DataSet::appendDataSet() on an empty dataset...
      /*
      Point* mapped = _history.mapPoint(p);
      _layout = mapped->layout();
      delete mapped;
      */
    }
  }
}


void DataSet::setHistory(const TransfoChain& history) {
  if (!empty()) {
    throw GaiaException("Can not set the history of a non-empty dataset");
  }

  _history = history;
}

void DataSet::forgetHistory() {
  _history = TransfoChain();
}

void DataSet::simplifyHistory() {
  // first make sure we can actually perform this operation on our history
  QStringList allowed;
  allowed << "remove" << "removevl" << "select" << "cleaner" << "fixlength";

  for (int i=0; i<_history.size(); i++) {
    if (!allowed.contains(_history[i].analyzerName)) {
      throw GaiaException("Your history contains the ", _history[i].analyzerName,
                          ", which is not an allowed transformation when calling DataSet::simplifyHistory.");
    }
  }

  // the simplified history can be reached via 2 steps:
  // 1- we remove all the descriptors that are in the original layout but not in the final one
  // 2- we fix the length of the descriptors which are VL in the original layout and FL in the final one

  PointLayout origLayout = originalLayout();
  PointLayout targetLayout = layout();

  // 1- create an "artifical" Transformation that removes the unneeded descriptors
  QSet<QString> origDescs = QSet<QString>::fromList(origLayout.descriptorNames());
  QSet<QString> targetDescs = QSet<QString>::fromList(targetLayout.descriptorNames());
  QStringList toremove = (origDescs - targetDescs).toList();
  toremove.sort();

  Transformation t1;
  t1.analyzerName = "remove";
  t1.analyzerParams.insert("descriptorNames", toremove);
  t1.applierName = "removedesc";
  t1.params.insert("descriptorNames", toremove);
  t1.layout = origLayout;

  PointLayout intermediateLayout = origLayout.copy();
  foreach (const QString& desc, toremove) {
    intermediateLayout.remove(desc);
  }

  // 2- fix the length of those descriptors that need it
  // this works because we already have the same descriptor names
  QStringList tofix = intermediateLayout.symmetricDifferenceWith(targetLayout);
  tofix.sort();

  Transformation t2;
  t2.analyzerName = "fixlength";
  t2.analyzerParams.insert("descriptorNames", tofix);
  t2.applierName = "fixlengthapplier";
  t2.params.insert("descriptorNames", tofix);
  t2.layout = intermediateLayout;

  // just to make sure we didn't screw up
  PointLayout finalLayout = intermediateLayout.copy();
  foreach (const QString& desc, tofix) {
    finalLayout.fixLength(desc, targetLayout.descriptorLocation(desc).dimension());
  }
  if (finalLayout != targetLayout) {
    throw GaiaException("Ouch! DataSet::simplifyHistory() didn't work as expected...");
  }

  // and finally replace the old history with the new one
  forgetHistory();
  if (!t1.params.value("descriptorNames").toStringList().isEmpty()) _history << t1;
  if (!t2.params.value("descriptorNames").toStringList().isEmpty()) _history << t2;
}


// helper function for asserting coherency
bool DataSet::consistentLinks() const {
  QSet<DataSet*> closure;
  int csize = _linkedDataSets.size();
  DataSet* ref = _linkedDataSets[0];
  closure.insert(const_cast<DataSet*>(this));
  foreach (DataSet* ds, _linkedDataSets) {
    closure.insert(ds);
    if (ds->_linkedDataSets.size() != csize) return false;
    if (ds->_linkedDataSets[0] != ref) return false;
    foreach (DataSet* dsl, ds->_linkedDataSets) {
      closure.insert(dsl);
    }
  }

  if (closure.size() != csize) return false;
  if (!closure.contains(const_cast<DataSet*>(this))) return false;
  foreach (DataSet* ds, _linkedDataSets) {
    if (!closure.contains(ds)) return false;
  }

  return true;
}


const DataSet* DataSet::referenceDataSet() const {
  Q_ASSERT(!_linkedDataSets.isEmpty());
  return _linkedDataSets[0];
}

// We only want to allow 1-N relations here (any number of datasets referencing
// a single one, itself self-referencing), so we need to make quite some checks
// to ensure this condition.
// 1- can only reference a dataset which is self-referencing
// 2- cannot unlink a reference dataset which still has other ones pointing to it
// 3- datasets must be isomorph
void DataSet::setReferenceDataSet(DataSet* dataset, bool checkOriginalLayout) {
  // first things first, the very special cases:

  // a- setting the reference dataset to 0 means we want this dataset to be
  //    self-referencing
  if (!dataset) dataset = this;

  // b- a dataset which is not linked to any other one should be promoted
  //    immediately to a self-referencing dataset
  if (_linkedDataSets.isEmpty()) _linkedDataSets << this;

  // c- we're empty, so we don't have any layout yet -> error (except if want to
  //    self-reference, in which case we can just go on)
  if (isEmpty() && dataset != this) {
    throw GaiaException("DataSet is empty, hence cannot reference another one other than itself.");
  }

  // 1- can only reference a dataset which is self-referencing, except if we're
  //    trying to reference ourself and we had another ref previously
  if (dataset != this && dataset->_linkedDataSets[0] != dataset) {
    throw GaiaException("Can only reference a self-referencing dataset");
  }

  // 2- cannot unlink a reference dataset which still has other ones pointing to it
  if (_linkedDataSets[0] == this && // this is a self-ref dataset
      _linkedDataSets.size() > 1) {
    throw GaiaException("Cannot change reference for this dataset because there are "
                        "still other datasets referencing it");
  }

  // 3- datasets must be isomorph (if dataset = this, it's pretty obvious!)
  if (dataset != this) checkIsomorphDataSets(this, dataset, checkOriginalLayout);

  // first unlink this dataset from all the others
  foreach (DataSet* ds, _linkedDataSets) {
    ds->_linkedDataSets.removeAll(this);
  }
  _linkedDataSets.clear();

  // now link to the reference dataset and all its friends
  _linkedDataSets = dataset->_linkedDataSets;

  foreach (DataSet* ds, _linkedDataSets) {
    ds->_linkedDataSets << this;
  }

  // needs to be put at the end, otherwise it gets included multiple times...
  _linkedDataSets << this;

  Q_ASSERT(consistentLinks());

  invalidateViews();
}



void DataSet::addView(View* view) {
  _linkedViews << view;
}

void DataSet::removeView(View* view) {
  _linkedViews.removeAll(view);
}




void DataSet::invalidateViews() {
  foreach (View* v, _linkedViews) {
    v->invalidate();
  }
}

void DataSet::modify() {
  _isDataSorted = false;
  invalidateViews();
}

void DataSet::unifyLayout() {
  for (int i=0; i<size(); i++) {
    at(i)->setLayoutUnsafe(_layout);
  }
}


DataSet* DataSet::copy() const {
  DataSet* result = new DataSet;

  // need: name, history, layout, collections/points
  result->_name = this->_name;
  result->_history = this->_history;
  result->_layout = this->_layout.copy();

  ((PointArray*)result)->resize(size());
  for (int i=0; i<size(); i++) {
    (*result)[i] = new Point(*at(i));
    (*result)[i]->setLayoutUnsafe(result->_layout);
  }

  return result;
}


void DataSet::load(const QString& filename, int start, int end) {
  QFile file(filename);
  if (!file.open(QIODevice::ReadOnly)) {
    throw GaiaException("Could not open file: ", filename);
  }

  QDataStream in(&file);
  load(in, start, end);
}

void DataSet::loadNthPart(const QString& filename, int idx, int total) {
  QFile file(filename);
  if (!file.open(QIODevice::ReadOnly)) {
    throw GaiaException("Could not open file: ", filename);
  }

  QDataStream in(&file);
  int npoints = load(in, 0, 0);

  if (!file.seek(0)) throw GaiaException("Error rewinding to beginning of file");

  int size = (npoints - 1) / total + 1;
  load(in, idx*size, (idx+1)*size);
}


void DataSet::save(const QString& filename) const {
  QFile file(filename);
  if (!file.open(QIODevice::WriteOnly)) {
    throw GaiaException("Could not open file: ", filename);
  }

  QDataStream out(&file);

  out << *this;
}


void DataSet::fromBase64(const std::string& data) {
  fromBase64(QByteArray(data.c_str(), data.size()));
}

void DataSet::fromBase64(const QByteArray& data) {
  QByteArray array = QByteArray::fromBase64(data);
  QBuffer buffer(&array);
  buffer.open(QIODevice::ReadOnly);

  QDataStream in(&buffer);
  checkValidDataStream(in);

  in >> *this;
}

std::string DataSet::toBase64() const {
  QByteArray array;
  QBuffer buffer(&array);
  buffer.open(QIODevice::WriteOnly);

  QDataStream out(&buffer);
  setDataStreamVersion(out);

  out << *this;

  return array.toBase64().data();
}


int DataSet::binarySearch(const QString& id, int start, int end) const {
  while (start < end) {
    int middle = (start+end) / 2;
    if (nameLessThan(at(middle)->name(), id)) {
      start = middle + 1;
    }
    else {
      end = middle;
    }
  }
  return start;
}

void DataSet::sortPoints(int pivotIdx) {
  if (_isDataSorted) return;

  if (pivotIdx >= 0) {
    // presort the points using a merge sort, which will avoid the worst case
    // scenario of O(n^2) for the quicksort that's done afterwards
    int dummy = 0;
    QAlgorithmsPrivate::qMerge(this->begin(), this->begin()+pivotIdx, this->end(), dummy, pidLessThan);
  }

  sort(this->begin(), this->end(), pidLessThan);
  _isDataSorted = true;
}


void DataSet::appendDataSet(const DataSet* dataset) {
  // appending an empty dataset... nothing to do
  if (dataset->isEmpty()) return;

  // if dataset has linked datasets, make this operation impossible because
  // we're just appending the points, not adding and transforming them
  if (_linkedDataSets[0] != this || _linkedDataSets.size() > 1) {
    throw GaiaException("DataSet: you can only append a dataset to another one if it is not linked");
  }

  // if appending to an empty dataset, set it up so it passes all the following tests
  if (isEmpty()) {
    _layout = dataset->layout();
    _history = dataset->history();
  }

  // check layout once and for all
  if (_layout != dataset->layout()) {
    throw GaiaException("DataSets do not have the same layout. Impossible to append one to the other.");
  }

  // also check transfo history
  if (!(_history == dataset->history())) {
    throw GaiaException("DataSets do not have the same transformation history. "
                        "Impossible to append one to the other.");
  }

  addPoints(*dataset, false, false, true, false, false);
}


void DataSet::addPoint(const Point* p) {
  return addPoints(QVector<Point*>() << const_cast<Point*>(p),
                   true, true);
}


void DataSet::addPoints(const QVector<Point*>& points) {
  addPoints(points, true, true);
}


void DataSet::addPoints(const QVector<Point*>& points,
                        bool layoutCheck,
                        bool transformPoint,
                        bool checkUnique,
                        bool takeOwnership,
                        bool relaySignal) {

  if (points.isEmpty()) return;

  if (checkUnique) {
    checkUniqueIDsFrom(points);
    checkUnique = false;
  }

  // if no layout has been set yet, take the layout of the first point
  setLayoutIfEmpty(points[0]);

  QVector<Point*> preparedPoints;

  // check whether all layouts are the same, or if not, if we can adapt them.
  if (layoutCheck) {
    // The layout of the points need to be either the original layout of the
    // dataset if we intend to transform the points, or the current layout.
    //
    // In the case where transformPoint = true, we are already assured that the
    // original layouts of all linked datasets is the same, but in the case
    // where transformPoint = false, we need to make sure that all datasets in
    // which we'll be inserting those points have the same current layout.
    PointLayout targetLayout;

    if (transformPoint) targetLayout = originalLayout();
    else {
      targetLayout = _layout;
      for (int i=1; i<_linkedDataSets.size(); i++) {
        if (_linkedDataSets[0]->layout() != _linkedDataSets[i]->layout()) {
          throw GaiaException("Trying to insert points without transforming them "
                              "in linked datasets which don't have the same layout");
        }
      }
    }

    G_DEBUG(GDataSet, "Checking layouts of points to be added");
    for (int i=0; i<points.size(); i++) {
      Point* p = points[i];

      if (p->layout() == targetLayout) {
        Point* toAdd = p;
        if (!takeOwnership) toAdd = new Point(*p);
        preparedPoints << toAdd;
      }
      else {
        // second chance: can we morph into the required layout?
        // try to see if there are some descriptors in the layout that could
        // have been misdetected (ie: one string descriptor which happens to be
        // parsable as a float, etc...)
        G_DEBUG(GDataSet, "Trying to morph point to make it fit inside a dataset layout");

        // basic check (which should cover most, if not all the cases) in which the parser
        // could fail to auto-detect the correct type: look whether all different descriptors
        // are actually string descriptors which have been autodetected as real values
        // (closes ticket #21)
        if (!p->layout().canMorphInto(targetLayout)) {
          QStringList diff = targetLayout.symmetricDifferenceWith(p->layout());
          throw GaiaException("Trying to insert the point '", p->name(),
                              "' in a DataSet with incompatible layout.\n"
                              "Different descriptors: " + diff.join(" - "));
        }

        preparedPoints << targetLayout.morphPoint(p);
        if (takeOwnership) delete p;
      }
    }
  }
  else { // no layout check
    if (takeOwnership) {
      preparedPoints = points;
    }
    else {
      for (int i=0; i<points.size(); i++)
        preparedPoints << new Point(*points[i]);
    }
  }

  // NB: at this point, we have a QVector<Point*> of points which we own, and
  //     which don't need to be checked for their layout anymore.


  // relay the signal to all linked datasets
  // this needs to be done now, because depending on the dataset, the
  // transformation we want to apply to the points might not be the same
  if (relaySignal) {
    if (_linkedDataSets.size() == 1 && takeOwnership) {
      addPoints(preparedPoints,
                false,  // layoutCheck
                transformPoint,
                checkUnique,
                true,   // takeOwnership
                false); // relaySignal
    }
    else {
      foreach (DataSet* dataset, _linkedDataSets) {
        dataset->addPoints(preparedPoints,
                           false,  // layoutCheck
                           transformPoint,
                           checkUnique,
                           false,  // takeOwnership
                           false); // relaySignal
      }

      // preparedPoints was ours, we need to delete them now
      for (int i=0; i<preparedPoints.size(); i++)
        delete preparedPoints[i];
    }

    return;
  }
  // TODO: optimization: in the previous call, if we have no linked datasets, we
  //       can set takeOwnership = true which will spare us a copy of the points

  // transform all points, making a copy if takeOwnership is false
  if (transformPoint) {
    for (int i=0; i<preparedPoints.size(); i++) {
      Point* mappedPoint = _history.mapPoint(preparedPoints[i]);

      // share the DataSet layout for memory savings!
      Q_ASSERT(mappedPoint->layout() == _layout);
      mappedPoint->setLayout(_layout);

      delete preparedPoints[i];
      preparedPoints[i] = mappedPoint;
    }
  }

  // need to update the enums values here, as src & dest might not have
  // the same enum maps (which are not checked as part of layout equality)
  foreach (Point* p, preparedPoints) {
    p->switchLayout(_layout);
  }

  checkAllPointsShareSameLayout(&preparedPoints);

  // actually add them
  int origSize = size();
  (*this) << preparedPoints;

  modify();

  // most likely unnecessary, but might be good to do this now (ie: more
  // predictable response times afterwards)
  sortPoints(origSize);
}

void DataSet::checkAllPointsShareSameLayout(const QVector<Point*>* points) const {
  if (!points) points = this;
  foreach (const Point* p, *points) {
    if (!_layout.sameObjectAs(p->layout())) {
      throw GaiaException("Trying to append a point to dataset which doesn't have the exact same layout memory object");
    }
  }
}

void DataSet::removePoint(const QString& id) {
  removePoints(QStringList() << id);
}

void DataSet::removePoints(const QList<QString>& ids) {
  removePoints(ids, true);
}

void DataSet::removePoints(const QList<QString>& ids, bool relaySignal) {
  if (relaySignal) {
    foreach (DataSet* dataset, _linkedDataSets) {
      dataset->removePoints(ids, false);
    }
    return;
  }

  QList<int> pointIndices;

  foreach (QString id, ids) {
    pointIndices << pointIndex(id);
  }

  // delete points first
  foreach (int idx, pointIndices) {
    delete at(idx);
  }

  // repack the points (thus keeping the order) by shifting them by as many
  // places to the left as points that were before that and that had been deleted
  pointIndices.append(this->size()-1);
  sort(pointIndices.begin(), pointIndices.end());
  for (int i=0; i<pointIndices.size()-1; i++) {
    for (int idx = pointIndices[i]; idx < pointIndices[i+1]; idx++) {
      (*this)[idx-i] = (*this)[idx+1];
    }
  }

  PointArray::resize(size() - ids.size());

  modify();

  // most likely unnecessary, but might be good to do this now (ie: more
  // predictable response times afterwards)
  sortPoints();
}



void DataSet::checkUniqueIDs() {
  sortPoints();
  QMap<QString, int> duplicates;
  for (int i=0; i<size()-1; i++) {
    if (at(i)->name() == at(i+1)->name()) {
      duplicates[at(i)->name()] += 1;
    }
  }

  if (duplicates.empty()) return;

  QStringList msg;
  msg << "WARNING: DataSet is invalid because:";
  QMap<QString, int>::const_iterator it = duplicates.constBegin();
  for (; it != duplicates.constEnd(); ++it) {
    msg << "\n - point with ID '" << it.key()
        << "' appears " << QString::number(it.value() + 1) << " times";
  }

  throw GaiaException(msg);
}

void DataSet::checkUniqueIDsFrom(const QVector<Point*>& v) {
  // check none of the points we add are already in there, and that none of them
  // appear twice in the collection (ie: after adding them all, there are still
  // no duplicates in the collection)
  for (int i=0; i<v.size(); i++) {
    if (this->contains(v.at(i)->name())) {
      QStringList msg;
      msg << "DataSet: you're trying to add a point with ID '" << v.at(i)->name()
          << "', which is already in the dataset";
      throw GaiaException(msg);
    }
  }

  QVector<Point*> points(v);
  sort(points.begin(), points.end(), pidLessThan);
  for (int i=0; i<points.size()-1; i++) {
    if (points.at(i)->name() == points.at(i+1)->name()) {
      QStringList msg;
      msg << "DataSet: you're trying to add a collection in which there are "
          << "more than 1 point with the ID '" << points[i]->name() << "'";
      throw GaiaException(msg);
    }
  }
}


/**
 * This class represent a job that loads a single point, and returns it when done.
 * On error, it will print a message and return a null pointer.
 */
class LoadPointJob {
 public:
  LoadPointJob(const QString& pname, const QString& filename,
               const QStringList* descsSelect, const QStringList* descsExclude,
               PointLayout** layout, int* ncompleted, TextProgress* info, QMutex* lock) :
    _pname(pname), _filename(filename), _descsSelect(descsSelect),
    _descsExclude(descsExclude), _layout(layout), _numberOfJobsCompleted(ncompleted),
    _info(info), _globalLock(lock) {}

  ~LoadPointJob() {}

  Point* run() {
    Point* p = new Point();
    p->setName(_pname);

    try {
      p->load(_filename, *_descsSelect, *_descsExclude);
    }
    catch (GaiaException& e) {
      qWarning() << "Could not read from file:" << _filename;
      qWarning() << e.what();
      delete p;

      QMutexLocker lock(_globalLock);
      update();
      return 0;
    }

    // first do the comparison here, outside the critical section, because
    // comparing layouts involves recomputing some hash value which is quite
    // CPU-intensive and can be done outside.
    // The comparison in the critical section will use a cached value for the
    // hash, and is thus blazing fast, avoiding contention as much as possible.
    if (*_layout) p->layout() == **_layout;

    QMutexLocker lock(_globalLock);
    if (!*_layout) *_layout = &p->layout();

    if (p->layout() == **_layout) {
      ;
    }
    else if (p->layout().canMorphInto(**_layout)) {
      Point* morphed = (*_layout)->morphPoint(p);
      delete p;
      p = morphed;
    }
    else {
      qWarning() << "WARNING: Could not merge point" << _pname
                 << "because it has an incompatible layout";
      qWarning() << "Different descriptors are:"
                 << (*_layout)->symmetricDifferenceWith(p->layout()).join(", ");
      delete p;
      p = 0;
    }

    update();
    return p;
  }

  static Point* loadPoint(LoadPointJob* dljob) {
    return dljob->run();
  }

 protected:
  inline void update() {
    if (gaia2::verbose) _info->update(++(*_numberOfJobsCompleted));
  }

  QString _pname, _filename;
  const QStringList *_descsSelect, *_descsExclude;

  PointLayout** _layout;
  int* _numberOfJobsCompleted;
  TextProgress* _info;
  QMutex* _globalLock;
};


DataSet* DataSet::mergeFiles(const QMap<QString, QString>& sigfiles,
                             const QStringList& descsSelect,
                             const QStringList& descsExclude,
                             int start, int end, PointLayout* reflayout) {

  PointLayout* layout = reflayout;
  TextProgress info(0, "Merging file [%1/%2] (%3% done)", UpdateOnUnit);
  QMutex lock;

  QList<LoadPointJob*> listJobs;
  int currentJob = 0;

  QMap<QString, QString>::const_iterator it = sigfiles.constBegin();
  for (; it != sigfiles.constEnd(); ++it) {
    currentJob++;

    if (currentJob <= start) continue;
    if (currentJob > end) break;

    listJobs << new LoadPointJob(it.key(), it.value(),
                                 &descsSelect, &descsExclude,
                                 &layout, &currentJob, &info, &lock);
  }

  G_INFO("Processing jobs number from " << start << " to " << currentJob-1 <<
         " included (out of " << sigfiles.size() << " without duplicate ids)");

  G_INFO("Will run using " << QThreadPool::globalInstance()->maxThreadCount() << " threads");

  info.setTotal(listJobs.size());
  currentJob = 0;

  QFuture<Point*> future = QtConcurrent::mapped(listJobs, LoadPointJob::loadPoint);
  future.waitForFinished();

  G_INFO("All jobs finished, merging into dataset");

  QVector<Point*> validPoints;
  validPoints.reserve(listJobs.size());

  QFuture<Point*>::const_iterator p = future.constBegin();
  for (; p != future.constEnd(); ++p) {
    if (*p) validPoints << *p;
  }

  DataSet* result = new DataSet();
  // we use addPoints with takeOwnership = true, so there is no need to free
  // those points afterwards
  result->addPoints(validPoints, false, false, true, true, false);

  // cleanup
  foreach (LoadPointJob* j, listJobs) delete j;

  return result;
}






#define MAGIC_NUMBER 0x6AEA7230

/*
QDataStream::Qt_1_0 1   Version 1 (Qt 1.x)
QDataStream::Qt_2_0 2   Version 2 (Qt 2.0)
QDataStream::Qt_2_1 3   Version 3 (Qt 2.1, 2.2, 2.3)
QDataStream::Qt_3_0 4   Version 4 (Qt 3.0)
QDataStream::Qt_3_1 5   Version 5 (Qt 3.1, 3.2)
QDataStream::Qt_3_3 6   Version 6 (Qt 3.3)
QDataStream::Qt_4_0 7   Version 7 (Qt 4.0, Qt 4.1)
QDataStream::Qt_4_1 Qt_4_0  Version 7 (Qt 4.0, Qt 4.1)
QDataStream::Qt_4_2 8   Version 8 (Qt 4.2)
QDataStream::Qt_4_3 9   Version 9 (Qt 4.3)
QDataStream::Qt_4_4 10  Version 10 (Qt 4.4)
QDataStream::Qt_4_5 11  Version 11 (Qt 4.5)
QDataStream::Qt_4_6 12  Version 12 (Qt 4.6, Qt 4.7, Qt 4.8)
QDataStream::Qt_4_7 Qt_4_6  Same as Qt_4_6.
QDataStream::Qt_4_8 Qt_4_7  Same as Qt_4_6.
QDataStream::Qt_4_9 Qt_4_8  Same as Qt_4_6.
QDataStream::Qt_5_0 13  Version 13 (Qt 5.0)
QDataStream::Qt_5_1 14  Version 14 (Qt 5.1)
QDataStream::Qt_5_2 15  Version 15 (Qt 5.2)
QDataStream::Qt_5_3 Qt_5_2  Same as Qt_5_2
QDataStream::Qt_5_4 16  Version 16 (Qt 5.4)
QDataStream::Qt_5_5 Qt_5_4  Same as Qt_5_4
QDataStream::Qt_5_6 17  Version 17 (Qt 5.6)
QDataStream::Qt_5_7 Qt_5_6  Same as Qt_5_6
QDataStream::Qt_5_8 Qt_5_7  Same as Qt_5_6
QDataStream::Qt_5_9 Qt_5_8  Same as Qt_5_6
QDataStream::Qt_5_10 Qt_5_9 Same as Qt_5_6
*/
// Data Stream format versions:
// 100: Initial Gaia 2.0 data format
// 101: same as before + the following:
//      - Points now also write the Scope structure
//      - Transformations now also write the info field
// 102: For Qt_5_6 to Qt_5_10
QDataStream& gaia2::operator<<(QDataStream& out, const DataSet& dataset) {
  dataset.checkAllPointsShareSameLayout();

  // write header with magic number and version
  out << (quint32)MAGIC_NUMBER;

  setDataStreamVersion(out);

  // write name
  out << dataset._name;

  // write history
  out << dataset._history;

  // write layout
  out << dataset._layout;

  // write points
  out << (qint32)dataset.size();
  for (int i=0; i<dataset.size(); i++) {
    out << *dataset.at(i);
  }

  return out;
}


QDataStream& gaia2::operator>>(QDataStream& in, DataSet& dataset) {
  dataset.load(in);
  return in;
}

int DataSet::load(QDataStream& in, int start, int end, bool readAllPointsFromStream) {
  // read magic number
  quint32 magic;
  in >> magic;

  if (magic != MAGIC_NUMBER) {
    throw GaiaException("File does not seem to be a Gaia dataset file.");
  }

  checkValidDataStream(in);

  clear();

  // read name
  G_DEBUG(GIO, "Loading name...");
  in >> _name;

  // read history
  G_DEBUG(GIO, "Loading history...");
  in >> _history;

  // read layout
  G_DEBUG(GIO, "Loading layout...");
  in >> _layout;

  // read collections
  G_DEBUG(GIO, "Loading points...");
  qint32 size, csize;
  QString name;
  PointLayout layout;

  switch(gaiaVersion(in)) {

  case Gaia_2_0:
  case Gaia_2_1:
    if (start != 0 || end != -1) {
      throw GaiaException("Can only load parts of datasets for versions >= 2.3");
    }

    // pretend to load collections, then load points from them
    in >> size;
    for (int i=0; i<size; i++) {
      in >> name;

      // load collection
      G_DEBUG(GIO, "[" << i+1 << "/" << size << "] Loading collection" << name);
      in >> name;
      in >> csize;

      for (int j=0; j<csize; j++) {
        Point* p = new Point();
        in >> *p;
        *this << p;
      }
    }
    break;

  case Gaia_2_3: {
    // load all points that we asked for
    in >> size;

    // make sure we don't segfault
    start = qMin(qMax(start, 0), size);
    if (end < 0) end = size;
    end = qMin(qMax(end, 0), size);

    Point dummy;
    for (int i=0; i<start; i++) in >> dummy;

    for (int i=start; i<end; i++) {
      Point* p = new Point();
      in >> *p;
      *this << p;
    }

    // in some cases, we might want to read those leftover points anyway, to
    // leave the stream in a correct state
    if (readAllPointsFromStream) {
      for (int i=end; i<size; i++) in >> dummy;
    }

    break;
  }
#ifdef GAIA_QT5
  case Gaia_2_4: {
    // load all points that we asked for
    in >> size;

    // make sure we don't segfault
    start = qMin(qMax(start, 0), size);
    if (end < 0) end = size;
    end = qMin(qMax(end, 0), size);

    Point dummy;
    for (int i=0; i<start; i++) in >> dummy;

    for (int i=start; i<end; i++) {
      Point* p = new Point();
      in >> *p;
      *this << p;
    }

    // in some cases, we might want to read those leftover points anyway, to
    // leave the stream in a correct state
    if (readAllPointsFromStream) {
      for (int i=end; i<size; i++) in >> dummy;
    }

    break;
  }
#endif
  }

  // make sure dataset is valid & compact memory
  checkUniqueIDs();
  unifyLayout();

  checkAllPointsShareSameLayout();

  return size;
}
