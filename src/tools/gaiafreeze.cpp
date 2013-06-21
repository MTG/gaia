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
#include <iostream>
#include "dataset.h"
#include "frozendataset.h"
#include "distancefunction.h"
#include "timer.h"
#include "view.h"
using namespace std;
using namespace gaia2;

ParameterMap parseOptions(int argc, char* argv[]) {
  if (argc < 3) {
    G_ERROR("not enough arguments...");
    G_ERROR("");
    G_ERROR("Usage: " << argv[0] << " dataset_file frozen_output_dataset_file");
  }

  ParameterMap options;
  options.insert("inputDataSet", argv[1]);
  options.insert("outputDataSet", argv[2]);

  return options;
}


int main(int argc, char* argv[]) {
  QCoreApplication app(argc, argv);

  gaia2::verbose = true;

  ParameterMap options = parseOptions(argc, argv);

  DataSet dataset;

  try {
    QString inputDataSet = options.value("inputDataSet").toString();
    G_INFO("Loading dataset: " << inputDataSet);
    dataset.load(inputDataSet);

    // do this here so we can catch the msg nicely and write it to stderr
    // it will be done again when we try to convert it, but that's fine
    FrozenDataSet::checkValidFrozenDataSet(dataset);

  }
  catch (GaiaException& e) {
    G_ERROR(e.msg());
    exit(1);
  }

  int npoints = dataset.size();
  int dimension = dataset.layout().correspondingRegion().dimension();

  G_INFO("DataSet name: " << dataset.name());
  G_INFO("Total number of points: " << npoints);
  G_INFO("Dimension: " << dimension);
  G_INFO("");


  // ---------------------------------------------------------------------------

  G_INFO("Copying data into FrozenDataset...");

  FrozenDataSet frozen;
  frozen.fromDataSet(dataset);

  QString outputDataSet = options.value("outputDataSet");
  G_INFO("Saving FrozenDataSet to file " << outputDataSet << "...");

  try {
    frozen.save(outputDataSet);
  }
  catch (GaiaException& e) {
    G_ERROR(e.msg());
    exit(1);
  }

  G_INFO("DataSet successfully frozen!");

  return 0;
}

