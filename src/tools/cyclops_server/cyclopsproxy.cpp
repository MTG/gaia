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

#include "cyclopsproxy.h"
#include "cyclops.h"
using namespace gaia2;

yaml::Node toYaml(const CyclopsResultsValues& values) {
  yaml::Sequence result;
  for (int i=0; i<values.size(); i++) {
    yaml::Sequence r;
    for (int j=0; j<values[i].size(); j++) {
      QVariant v = values[i][j];
      r << yaml::Node(v.toString());
    }
    result << yaml::Node(r);
  }
  return result;
}

yaml::Node toYaml(const CyclopsResultsHeader& header) {
  yaml::Sequence result;
  for (int i=0; i<header.size(); i++) {
    result << yaml::Node(header[i]);
  }
  return result;
}

yaml::Node toYaml(const CyclopsResults& results) {
  yaml::Mapping result;
  result.insert("results", toYaml(results.results));
  result.insert("header", toYaml(results.header));
  result.insert("values", toYaml(results.values));
  return result;
}

CyclopsProxy::CyclopsProxy(Cyclops* cyclops) : _cyclops(cyclops) {}

void checkNParams(const yaml::Sequence& seq, int n) {
  if (seq.size() != n) throw InvalidParams();
}

yaml::Node CyclopsProxy::process(const yaml::Mapping& query) {
  QString method = query["method"];
  yaml::Sequence params = query.value("params", yaml::Sequence()).sequence();

  // Cyclops::setup()
  if (method == "setup") {
    QString filename;
    checkNParams(params, 1);
    try {
      filename = params[0];
    }
    catch (GaiaException&) { throw InvalidParams(); }

    _cyclops->setup(filename);

    return "null"; // corresponds to None
  }

  // Cyclops::setupFromYaml()
  if (method == "setupFromYaml") {
    yaml::Mapping config;
    checkNParams(params, 1);
    try {
      config = params[0].mapping();
    }
    catch (GaiaException& e) { throw InvalidParams(e.msg()); }

    _cyclops->setupFromYaml(config);

    return "null"; // corresponds to None
  }

  // Cyclops::load()
  if (method == "load") {
    QString name, filename;
    int start = 0, end = -1;

    // check params == 2 or 3 or 4, we allow default value for the parameters
    if (params.size() != 2 && params.size() != 3 && params.size() != 4) throw InvalidParams();
    try {
      name = params[0];
      filename = params[1];

      bool ok;
      if (params.size() == 3) {
        start = params[2].scalar().toUInt(&ok);
        if (!ok) throw InvalidParams("start point needs to be a uint");
      }
      if (params.size() == 4) {
        end = params[3].scalar().toUInt(&ok);
        if (!ok) throw InvalidParams("end point needs to be a uint");
      }
    }
    catch (GaiaException&) { throw InvalidParams(); }

    _cyclops->load(name, filename, start, end);

    return "null"; // corresponds to None
  }

  // Cyclops::loadNthPart()
  if (method == "loadNthPart") {
    QString name, filename;
    int idx = 0, total = 1;

    checkNParams(params, 4);
    try {
      name = params[0];
      filename = params[1];

      bool ok;
      idx = params[2].scalar().toUInt(&ok);
      if (!ok) throw InvalidParams("idx needs to be a uint");

      total = params[3].scalar().toUInt(&ok);
      if (!ok) throw InvalidParams("total needs to be a uint");
    }
    catch (GaiaException&) { throw InvalidParams(); }

    _cyclops->loadNthPart(name, filename, idx, total);

    return "null"; // corresponds to None
  }

  // Cyclops::unload()
  if (method == "unload") {
    QString name;
    checkNParams(params, 1);
    try {
      name = params[0];
    }
    catch (GaiaException&) { throw InvalidParams(); }

    _cyclops->unload(name);

    return "null"; // corresponds to None
  }

  // Cyclops::datasetNames()
  if (method == "datasetNames") {
    checkNParams(params, 0);

    yaml::Sequence result;
    foreach (const QString& name, _cyclops->datasetNames()) {
      result << name;
    }

    return result;
  }

  // Cyclops::setReferenceDataSet()
  if (method == "setReferenceDataSet") {
    QString dataset, refDataset;
    checkNParams(params, 2);

    try {
      dataset = params[0];
      refDataset = params[1];
    }
    catch (GaiaException&) { throw InvalidParams(); }

    _cyclops->setReferenceDataSet(dataset, refDataset);

    return "null"; // corresponds to None
  }

  if (method == "layout") {
    QString dataset;
    checkNParams(params, 1);

    try {
      dataset = params[0];
    }
    catch (GaiaException&) { throw InvalidParams(); }

    return _cyclops->layout(dataset);
  }

  if (method == "size") {
    QString dataset;
    checkNParams(params, 1);

    try {
      dataset = params[0];
    }
    catch (GaiaException&) { throw InvalidParams(); }

    return QString::number(_cyclops->size(dataset));
  }

  // Cyclops::createDistance()
  if (method == "createDistance") {
    QString name, datasetName, distance;
    ParameterMap dparams;

    // check params == 3 or 4, we allow default value for the parameters
    if (params.size() != 3 && params.size() != 4) throw InvalidParams();

    try {
      name = params[0];
      datasetName = params[1];
      distance = params[2];

      if (params.size() == 4) {
        dparams = toParameterMap(params[3]);
      }
    }
    catch (GaiaException&) { throw InvalidParams(); }

    _cyclops->createDistance(name, datasetName, distance, dparams);

    return "null"; // corresponds to None
  }

  // Cyclops::deleteDistance()
  if (method == "deleteDistance") {
    QString name;
    checkNParams(params, 1);

    try {
      name = params[0];
    }
    catch (GaiaException&) { throw InvalidParams(); }

    _cyclops->deleteDistance(name);

    return "null"; // corresponds to None
  }

  // Cyclops::distanceNames()
  if (method == "distanceNames") {
    checkNParams(params, 0);

    yaml::Sequence result;
    foreach (const QString& name, _cyclops->distanceNames()) {
      result << name;
    }

    return result;
  }

  // Cyclops::indexOn()
  if (method == "indexOn") {
    QString datasetName, descriptorName;
    checkNParams(params, 2);
    try {
      datasetName = params[0];
      descriptorName = params[1];
    }
    catch (GaiaException&) { throw InvalidParams(); }

    _cyclops->indexOn(datasetName, descriptorName);

    return "null"; // corresponds to None
  }

  // Cyclops::chainedSearch()(
  if (method == "chainedSearch") {
    yaml::Sequence queries;
    yaml::Sequence header;
    int numberResults, offset = 0;

    // check params == 3 or 4, we allow default value for the offset
    if (params.size() != 3 && params.size() != 4) throw InvalidParams();

    try {
      queries = params[0].sequence();
      header = params[1].sequence();

      bool ok;
      numberResults = params[2].scalar().toUInt(&ok);
      if (!ok) throw InvalidParams("3rd parameter not a valid int");

      if (params.size() == 4) {
        offset = params[3].scalar().toUInt(&ok);
        if (!ok) throw InvalidParams("4th parameter not a valid int");
      }
    }
    catch (GaiaException&) { throw InvalidParams("invalid value"); }

    CyclopsResults results = _cyclops->chainedSearch(queries, header, numberResults, offset);

    return toYaml(results);
  }

  // Cyclops::nnSearchById()
  if (method == "nnSearchById") {
    QString pointId, datasetName, distanceName;
    int numberResults, offset = 0;

    // check params == 4 or 5, we allow default value for the offset
    if (params.size() != 4 && params.size() != 5) throw InvalidParams();

    try {
      pointId = params[0];
      datasetName = params[1];
      distanceName = params[2];

      bool ok;
      numberResults = params[3].scalar().toUInt(&ok);
      if (!ok) throw InvalidParams();

      if (params.size() == 5) {
        offset = params[4].scalar().toUInt(&ok);
        if (!ok) throw InvalidParams();
      }
    }
    catch (GaiaException&) { throw InvalidParams(); }

    SearchResults results = _cyclops->nnSearchById(pointId, datasetName, distanceName,
                                                   numberResults, offset);

    return toYaml(results);
  }

  // Cyclops::nnSearchByIdWithFilter()
  if (method == "nnSearchByIdWithFilter") {
    QString pointId, datasetName, distanceName, filter;
    int numberResults, offset = 0;

    // check params == 5 or 6, we allow default value for the offset
    if (params.size() != 5 && params.size() != 6) throw InvalidParams();

    try {
      pointId = params[0];
      datasetName = params[1];
      distanceName = params[2];
      filter = params[3];

      bool ok;
      numberResults = params[4].scalar().toUInt(&ok);
      if (!ok) throw InvalidParams();

      if (params.size() == 6) {
        offset = params[5].scalar().toUInt(&ok);
        if (!ok) throw InvalidParams();
      }
    }
    catch (GaiaException&) { throw InvalidParams(); }

    SearchResults results = _cyclops->nnSearchByIdWithFilter(pointId, datasetName, distanceName,
                                                             filter, numberResults, offset);

    return toYaml(results);
  }

  // Cyclops::getPoint()
  if (method == "getPoint") {
    QString datasetName, pointName;

    checkNParams(params, 2);
    try {
      datasetName = params[0];
      pointName = params[1];
    }
    catch (GaiaException&) { throw InvalidParams(); }

    return _cyclops->getPoint(datasetName, pointName)->toBase64();
  }

  if (method == "getPoints") {
    QString datasetName;
    QStringList pointNames;
    bool failOnError = true;

    if (params.size() != 2 && params.size() != 3) throw InvalidParams();
    try {
      datasetName = params[0];
      for (int i=0; i<params[1].sequence().size(); i++) {
        pointNames << params[1][i];
      }

      if (params.size() == 3) {
        if (params[2] == "true") failOnError = true;
        else if (params[2] == "false") failOnError = false;
        else throw InvalidParams("3rd param (failOnError) for getPoints needs to be a boolean value");
      }
    }
    catch (GaiaException&) { throw InvalidParams(); }

    return _cyclops->getPoints(datasetName, pointNames, failOnError);
  }

  // Cyclops::nnSearchByExample()
  if (method == "nnSearchByExample") {
    Point query;
    QString point64, datasetName, distanceName;
    int numberResults, offset = 0;

    // check params == 4 or 5, we allow default value for the offset
    if (params.size() != 4 && params.size() != 5) throw InvalidParams();

    try {
      point64 = params[0];
      query.fromBase64(point64.toAscii());

      datasetName = params[1];
      distanceName = params[2];

      bool ok;
      numberResults = params[3].scalar().toUInt(&ok);
      if (!ok) throw InvalidParams();

      if (params.size() == 5) {
        offset = params[4].scalar().toUInt(&ok);
        if (!ok) throw InvalidParams();
      }
    }
    catch (GaiaException&) { throw InvalidParams(); }

    SearchResults results = _cyclops->nnSearchByExample(query, datasetName, distanceName,
                                                        numberResults, offset);

    return toYaml(results);
  }

  // Cyclops::nnSearchByExampleWithFilter()
  if (method == "nnSearchByExampleWithFilter") {
    Point query;
    QString point64, datasetName, distanceName, filter;
    int numberResults, offset = 0;

    // check params == 5 or 6, we allow default value for the offset
    if (params.size() != 5 && params.size() != 6) throw InvalidParams();

    try {
      point64 = params[0];
      query.fromBase64(point64.toAscii());

      datasetName = params[1];
      distanceName = params[2];
      filter = params[3];

      bool ok;
      numberResults = params[4].scalar().toUInt(&ok);
      if (!ok) throw InvalidParams();

      if (params.size() == 6) {
        offset = params[5].scalar().toUInt(&ok);
        if (!ok) throw InvalidParams();
      }
    }
    catch (GaiaException&) { throw InvalidParams(); }

    SearchResults results = _cyclops->nnSearchByExampleWithFilter(query, datasetName, distanceName,
                                                                  filter, numberResults, offset);

    return toYaml(results);
  }


  //if (method == "...") {}

  throw NotImplementedError();
}

