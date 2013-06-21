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
#include <QCoreApplication>
#include <QDataStream>
#include <QFile>
#include <iostream>
#include "dataset.h"
using namespace std;
using namespace gaia2;


ParameterMap parseOptions(int argc, char* argv[]) {
  if (argc < 2) {
    qFatal("ERROR: not enough arguments...\n\n\
Usage: %s dataset_file", argv[0]);
  }

  ParameterMap options;
  options.insert("dataset", argv[1]);

  return options;
}


class EmptyDataSet : public DataSet {
 public:

  int load(const QString& filename) {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
      throw GaiaException("Could not open file: ", filename);
    }

    QDataStream in(&file);
    return DataSet::load(in, 0, 0);
  }
};



int main(int argc, char* argv[]) {
  QCoreApplication app(argc, argv);

  ParameterMap options = parseOptions(argc, argv);

  gaia2::verbose = true;

  EmptyDataSet dataset;
  int size;

  try {
    size = dataset.load(options.value("dataset"));
  }
  catch (GaiaException& e) {
    G_ERROR(e.msg());
    exit(1);
  }

  G_INFO("DataSet name: " << dataset.name());
  G_INFO("Total number of points: " << size);
  G_INFO("");

  G_INFO("History:");
  foreach (Transformation transfo, dataset.history()) {
    G_INFO(" -");
    G_INFO("  transfo: " << transfo.analyzerName);

    if (transfo.analyzerParams.isEmpty()) {
      G_INFO("  params: {}");
      continue;
    }

    G_INFO("  params:");
    foreach (QString paramName, transfo.analyzerParams.keys()) {
      Parameter pvalue = transfo.analyzerParams.value(paramName);
      if (pvalue.canConvert(QVariant::List)) {
        Stringifier str;
        QList<QVariant> plist = pvalue.toList();
        str << "    " << paramName << ": [";
        if (!plist.isEmpty()) str << " \"" << plist[0].toString() << '"';
        for (int i=1; i<plist.size(); i++) {
          str << ", \"" << plist[i].toString() << '"';
        }
        str << " ]";
        G_INFO(str.str());
      }
      else {
        G_INFO("    " << paramName << ": '" << pvalue << "'");
      }
    }
  }

  if (dataset.history().size() == 0) {
    G_INFO("  No transformations applied");
  }

  return 0;
}
