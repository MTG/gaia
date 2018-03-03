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
#include "cyclops.h"
#include "../../view.h"
#include "logging.h"
#include "yamlrpcserver.h" // for InvalidParams exception
using namespace gaia2;


void CyclopsResults::append(const Result& r, const QList<QVariant>& val) {
  results << r;
  values << val;
}


void CyclopsResults::mid(int pos, int length) {
  results = results.mid(pos, length);
  values = values.mid(pos, length);
}


bool cmpSearchResults(const QPair<Result, QList<QVariant> >& r1,
                      const QPair<Result, QList<QVariant> >& r2) {
  return r1.first.second < r2.first.second;
}

// NB: this is quite inefficient at the moment and could be optimized quite easily
void CyclopsResults::sort() {
  QList<QPair<Result, QList<QVariant> > > sortvec;

  for (int i=0; i<results.size(); i++) {
    sortvec << qMakePair(results[i], values[i]);
  }

  gaia2::sort(sortvec.begin(), sortvec.end(), cmpSearchResults);

  for (int i=0; i<results.size(); i++) {
    results[i] = sortvec[i].first;
    values[i] = sortvec[i].second;
  }
}


inline void datasetError(const QString& name) {
  throw GaiaException("Dataset '", name, "' does not exist");
}

inline void distanceError(const QString& name) {
  throw GaiaException("Distance '", name, "' does not exist");
}


Cyclops::Cyclops() {
  clog() << "A cyclops is born!";
}

Cyclops::~Cyclops() {
  foreach (DistanceFunction* dist, _dfuncs) delete dist;
  foreach (View* view, _views) delete view;
  foreach (DataSet* dataset, _datasets) delete dataset;
  clog() << "A cyclops just died...";
}


void Cyclops::load(const QString& datasetName, const QString& filename, int start, int end) {
  clog() << "Loading dataset:" << filename;

  DataSet* ds = new DataSet();
  ds->load(filename, start, end);

  // first delete previous dataset/view with the same name, if any
  delete _datasets.value(datasetName, 0);
  delete _views.value(datasetName, 0);

  _datasets.insert(datasetName, ds);
  _views.insert(datasetName, new View(ds));
}

void Cyclops::loadNthPart(const QString& datasetName, const QString& filename, int idx, int total) {
  clog() << "Loading dataset (part" << idx+1 << "of" << total << "in total) :" << filename;

  DataSet* ds = new DataSet();
  ds->loadNthPart(filename, idx, total);

  // first delete previous dataset/view with the same name, if any
  delete _datasets.value(datasetName, 0);
  delete _views.value(datasetName, 0);

  _datasets.insert(datasetName, ds);
  _views.insert(datasetName, new View(ds));
}

void Cyclops::unload(const QString& datasetName) {
  if (!_datasets.contains(datasetName)) datasetError(datasetName);

  delete _datasets.take(datasetName);
  delete _views.take(datasetName);
}


QStringList Cyclops::datasetNames() const {
  return _datasets.keys();
}

void Cyclops::setReferenceDataSet(const QString& dataset, const QString& referenceDataset) {
  if (!_datasets.contains(dataset)) datasetError(dataset);
  if (!_datasets.contains(referenceDataset)) datasetError(referenceDataset);

  _datasets.value(dataset)->setReferenceDataSet(_datasets.value(referenceDataset), false);
}


QString Cyclops::layout(const QString& dataset) const {
  if (!_datasets.contains(dataset)) datasetError(dataset);

  return _datasets.value(dataset)->layout().toYaml();
}

int Cyclops::size(const QString& dataset) const {
  if (!_datasets.contains(dataset)) datasetError(dataset);

  return _datasets.value(dataset)->size();
}


void Cyclops::createDistance(const QString& distanceName,
                             const QString& datasetName,
                             const QString& distance,
                             const gaia2::ParameterMap& params) {
  clog() << "Creating distance:" << distanceName;

  DataSet* ds = _datasets.value(datasetName);
  if (!ds) datasetError(datasetName);

  DistanceFunction* dist = MetricFactory::create(distance, ds->layout(), params);

  // first delete previous distance with the same name, if any
  delete _dfuncs.value(distanceName, 0);

  _dfuncs.insert(distanceName, dist);
}

void Cyclops::deleteDistance(const QString& distanceName) {
  if (!_dfuncs.contains(distanceName)) distanceError(distanceName);

  delete _dfuncs.take(distanceName);
}

QStringList Cyclops::distanceNames() const {
  return _dfuncs.keys();
}


void Cyclops::indexOn(const QString& datasetName, const QString& descriptorName) {
  View* view = _views.value(datasetName);
  if (!view) datasetError(datasetName);

  view->indexOn(descriptorName);
}


CyclopsResults Cyclops::chainedSearch(const yaml::Sequence& queries, const yaml::Sequence& header, int numberResults, int offset) {
  if (queries.empty()) throw InvalidParams("query list is empty!");

  // queries is a sequence of (methodName, args)
  ResultSet result = nnSearch(queries[0][0].scalar(), queries[0][1].sequence());
  for (int i=0; i<queries.size(); i++) {
    const QString& methodName = queries[i][0];
    const yaml::Sequence& args = queries[i][1].sequence();

    // we could have a limiter here, do not forward it to the nnSearch method but perform it here
    if (methodName == "thresholdLimit") {
      float maxDist = args[0].scalar().toDouble();
      result.thresholdLimit(maxDist);
    }
    else {
      // we assume it's a normal nnSearch query
      result = nnSearch(methodName, args, &result);
    }
  }

  // build CyclopsResults
  CyclopsResultsHeader cyclopsHeader;
  for (int i=0; i<header.size(); i++) {
    cyclopsHeader << header[i];
  }

  CyclopsResults cyclopsResults;
  cyclopsResults.results = result.get(numberResults, offset);
  cyclopsResults.header = cyclopsHeader;

  if (header.size() > 0) {
    // we want to get the name of the last dataset in which we performed the query
    // this means any method except thresholdLimit
    int qidx = queries.size() - 1;
    while (queries[qidx][0] == "thresholdLimit") qidx--;

    QString dsName = queries[qidx][1][1];
    // no need to check for dsName validity, as the query should already have complained if it didn't exist
    const DataSet* referenceDataSet = _datasets.value(dsName)->referenceDataSet();

    for (int i=0; i<cyclopsResults.results.size(); i++) {
      const QString& id = cyclopsResults.results[i].first;
      const Point* p = referenceDataSet->point(id);
      QList<QVariant> v;
      foreach (const QString& name, cyclopsHeader) {
        try {
          v << p->label(name).toSingleValue();
        }
        catch (GaiaException&) {
          v << p->value(name).toSingleValue();
        }
      }
      cyclopsResults.values << v;
    }
  }
  else {
    // we still need to send empty values for each point otherwise we end up with too
    // many exceptional cases...
    for (int i=0; i<cyclopsResults.size(); i++) cyclopsResults.values << QList<QVariant>();
  }

  return cyclopsResults;
}

void checkArgs(const yaml::Sequence& args, int nargs, const QString& methodName) {
  if (args.size() != nargs) {
    throw InvalidParams(QStringList()
                        << "In " << methodName
                        << ": expected " << QString::number(nargs)
                        << " arguments, received " << QString::number(args.size()));
  }
}

#define RET if (throwAwayDistance) delete dist; return result


ResultSet Cyclops::nnSearch(const QString& methodName, const yaml::Sequence& args, const ResultSet* inputSpace) {
  // check this now, also makes sure that next call to args[2] won't segfault
  if (args.size() < 3) throw InvalidParams("Wrong number of arguments for call to method: ", methodName);

  QString datasetName, distanceName;
  DistanceFunction* dist = 0;
  bool throwAwayDistance;

  // first make sure the dataset name is valid
  try {
    datasetName = args[1];
  }
  catch (GaiaException&) {
    throw InvalidParams("You need to specify dataset name (2nd arg) as a string");
  }

  View* v = _views.value(datasetName);
  if (!v) datasetError(datasetName);

  // if we have a valid dataset, then look at the distance asked for
  try{
    if (args[2].type() == yaml::ScalarType) {
      distanceName = args[2];

      // type(dist) == string: get a preexisting distance
      throwAwayDistance = false;

      dist = _dfuncs.value(distanceName);
      if (!dist) distanceError(distanceName);
    }
    else if (args[2].type() == yaml::MappingType) {
      // type(dist) == dict: create the distance now, we'll throw it away later
      throwAwayDistance = true;

      yaml::Mapping params = args[2].mapping();
      dist = MetricFactory::create(params["distance"],
                                   _datasets.value(datasetName)->layout(),
                                   toParameterMap(params["params"]));

    }
    else {
      throw GaiaException("Invalid distance specification");
    }
  }
  catch (GaiaException& e) {
    throw InvalidParams("You need to specify a distance (3rd arg) as either a string or a map of parameters to create a temporary distance", e.what());
  }


  // now that we have our view and our distance, let's roll!
  if (methodName == "nnSearchById") {
    checkArgs(args, 3, methodName);
    if (inputSpace) { ResultSet result = v->nnSearch(args[0].scalar(), *inputSpace, dist); RET; }
    else            { ResultSet result = v->nnSearch(args[0].scalar(), dist); RET; }
  }
  if (methodName == "nnSearchByIdWithFilter") {
    checkArgs(args, 4, methodName);
    if (inputSpace) { ResultSet result = v->nnSearch(args[0].scalar(), *inputSpace, dist, args[3].scalar()); RET; }
    else            { ResultSet result = v->nnSearch(args[0].scalar(), dist, args[3].scalar()); RET; }
  }

  QString point64 = args[0];
  Point query;
  query.fromBase64(point64.toLatin1());

  if (methodName == "nnSearchByExample") {
    checkArgs(args, 3, methodName);
    if (inputSpace) { ResultSet result = v->nnSearch(query, *inputSpace, dist); RET; }
    else            { ResultSet result = v->nnSearch(query, dist); RET; }
  }
  if (methodName == "nnSearchByExampleWithFilter") {
    checkArgs(args, 4, methodName);
    if (inputSpace) { ResultSet result = v->nnSearch(query, *inputSpace, dist, args[3].scalar()); RET; }
    else            { ResultSet result = v->nnSearch(query, dist, args[3].scalar()); RET; }
  }

  // maybe we still created a distance for an unknown nn-search, so we need to
  // make sure it's properly taken care of
  if (throwAwayDistance) delete dist;

  throw InvalidParams("Unknown nn-search method name: ", methodName);
}


SearchResults Cyclops::nnSearchById(const QString& queryID,
                                    const QString& datasetName,
                                    const QString& distanceName,
                                    int numberResults,
                                    int offset) {
  View* v = _views.value(datasetName);
  if (!v) datasetError(datasetName);

  DistanceFunction* dist = _dfuncs.value(distanceName);
  if (!dist) distanceError(distanceName);

  return v->nnSearch(queryID, dist).get(numberResults, offset);
}

SearchResults Cyclops::nnSearchByIdWithFilter(const QString& queryID,
                                              const QString& datasetName,
                                              const QString& distanceName,
                                              const QString& filter,
                                              int numberResults,
                                              int offset) {
  View* v = _views.value(datasetName);
  if (!v) datasetError(datasetName);

  DistanceFunction* dist = _dfuncs.value(distanceName);
  if (!dist) distanceError(distanceName);

  return v->nnSearch(queryID, dist, filter).get(numberResults, offset);
}


SearchResults Cyclops::nnSearchByExample(const gaia2::Point& query,
                                         const QString& datasetName,
                                         const QString& distanceName,
                                         int numberResults,
                                         int offset) {
  View* v = _views.value(datasetName);
  if (!v) datasetError(datasetName);

  DistanceFunction* dist = _dfuncs.value(distanceName);
  if (!dist) distanceError(distanceName);

  return v->nnSearch(query, dist).get(numberResults, offset);
}


SearchResults Cyclops::nnSearchByExampleWithFilter(const gaia2::Point& query,
                                                   const QString& datasetName,
                                                   const QString& distanceName,
                                                   const QString& filter,
                                                   int numberResults,
                                                   int offset) {
  View* v = _views.value(datasetName);
  if (!v) datasetError(datasetName);

  DistanceFunction* dist = _dfuncs.value(distanceName);
  if (!dist) distanceError(distanceName);

  return v->nnSearch(query, dist, filter).get(numberResults, offset);
}


const Point* Cyclops::getPoint(const QString& datasetName, const QString& pointName) const {
  if (!_datasets.contains(datasetName)) datasetError(datasetName);

  return _datasets.value(datasetName)->point(pointName);
}


std::string Cyclops::getPoints(const QString& datasetName, const QStringList& pointNames, bool failOnError) const {
  if (!_datasets.contains(datasetName)) datasetError(datasetName);
  DataSet* ds = _datasets.value(datasetName);

  QVector<Point*> points;
  if (failOnError) {
    foreach (const QString& name, pointNames) points << ds->point(name);
  }
  else {
    foreach (const QString& name, pointNames) {
      try {
        points << ds->point(name);
      }
      catch (GaiaException&) {}
    }
  }

  DataSet result;
  result.addPoints(points);

  return result.toBase64();
}



void Cyclops::setup(const QString& filename) {
  QFile file(filename);
  if (!file.open(QIODevice::ReadOnly)) {
    throw GaiaException("Could not open file: ", filename);
  }

  yaml::Mapping config = yaml::load(file.readAll()).mapping();

  setupFromYaml(config);
}


void Cyclops::setupFromYaml(const yaml::Mapping& config) {
  // need to keep them somehow, as we can only set them once all the datasets have been loaded
  QMap<QString, QString> refs;

  yaml::Mapping datasets = config.value("datasets", yaml::Mapping()).mapping();
  yaml::Mapping::const_iterator it = datasets.constBegin();
  for (; it != datasets.constEnd(); ++it) {
    const yaml::Mapping& ds = it.value().mapping();
    QString filename = ds.value("filename");
    if (ds.contains("ref")) {
      refs.insert(it.key(), ds.value("ref"));
    }
    int idx = ds.value("idx", "0").scalar().toInt();
    int total = ds.value("total", "1").scalar().toInt();
    loadNthPart(it.key(), filename, idx, total);
  }

  // add the references now
  QMap<QString, QString>::const_iterator itref = refs.constBegin();
  for (; itref != refs.constEnd(); ++itref) {
    setReferenceDataSet(itref.key(), itref.value());
  }


  // create the distances
  yaml::Mapping distances = config.value("distances", yaml::Mapping()).mapping();
  for (it = distances.constBegin(); it != distances.constEnd(); ++it) {
    const yaml::Mapping& dist = it.value().mapping();
    createDistance(it.key(), dist.value("dataset"), dist.value("name"),
                   toParameterMap(dist.value("params", yaml::Mapping())));
  }
}

//#include "cyclops.moc"
