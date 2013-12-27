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

#include <QCoreApplication>
#include <iostream>
#include "dataset.h"
#include "frozendataset.h"
#include "frozensearch.h"
#include "distancefunction.h"
#include "timer.h"
#include "view.h"
using namespace std;
using namespace gaia2;

ParameterMap parseOptions(int argc, char* argv[]) {
  if (argc < 2) {
    G_ERROR("not enough arguments...\n\n"
            "Usage: " << argv[0] << " [-v] dataset_file");
  }

  ParameterMap options;
  options.insert("verbose", false);

  if (argc >= 3) {
    if (QString(argv[1]) == "-v") {
      options.insert("verbose", true);
      options.insert("dataset", argv[2]);
    }
    else if (QString(argv[2]) == "-v") {
      options.insert("verbose", true);
      options.insert("dataset", argv[1]);
    }
  }
  else {
    options.insert("dataset", argv[1]);
  }

  return options;
}


// number of queries on which to average timing results
int N = 20;


QList<FrozenDistance*> dists;
QList<Eigen::RowVectorXf> queries;
int queryPointIds[] = { 1234, 9876, 37652, 1234566, 87634, 2854, 43987, 278654, 82654, 2609 };


template <typename Distance, int offset, int size>
void multiQuery(FrozenDataSet& dataset, int n) {
  G_INFO("(Doing multiple (" << n << ") queries at once)");
  dists.clear();
  queries.clear();
  for (int i=0; i<n; i++) {
    queries << dataset.row(queryPointIds[i%10] % dataset.rows());
    dists << new Distance(dataset, offset, size);
  }

  deepMultiFreeze(dataset, dists, queries, N);

  foreach (FrozenDistance* dist, dists) delete dist;
}



int main(int argc, char* argv[]) {
  QCoreApplication app(argc, argv);

  ParameterMap options = parseOptions(argc, argv);

  gaia2::init();
  gaia2::verbose = true;
  if (options.value("verbose").toBool()) {
    setDebugLevel(GTools);
  }

  DataSet dataset;
  int npoints;
  int dimension;

  try {
    G_INFO("Loading dataset: " << options.value("dataset"));
    dataset.load(options.value("dataset"));
    npoints = dataset.size();
    dimension = dataset.layout().correspondingRegion().dimension();
  }
  catch (GaiaException& e) {
    G_ERROR(e.msg());
    exit(1);
  }

  G_INFO("DataSet name: " << dataset.name());
  G_INFO("Total number of points: " << npoints);
  G_INFO("Dimension: " << dimension);
  G_INFO("");

  // ---------------------------------------------------------------------------

  G_INFO("Copying data into SuperMatrix (naive FrozenDataset)...");

  G_INFO("fixed real data dimension = " << dataset[0]->frealData().size());
  G_INFO("(note: should be 175 for gaiabench)");
  G_INFO("");

  float* frozenDataSet = new float[npoints*dimension];
  for (int i=0; i<npoints; i++) {
    const int offset = i*dimension;
    const RealDescriptor& d = dataset[i]->frealData();
    for (int j=0; j<dimension; j++) {
      frozenDataSet[offset+j] = d[j];
    }
  }

  float* frozenDataSet2 = new float[npoints*25];
  for (int i=0; i<npoints; i++) {
    const int offset = i*25;
    const RealDescriptor& d = dataset[i]->frealData();
    for (int j=0; j<25; j++) {
      frozenDataSet2[offset+j] = d[150+j];
    }
  }


  G_INFO("Copying data into gaia::FrozenDataSet...");

  FrozenDataSet frozenDS;
  frozenDS.fromDataSet(dataset);


  gaia2::verbose = false;
  ParameterMap eucp;
  eucp.insert("descriptorNames", "pca");
  DataSet* eucds = transform(&dataset, "select", eucp);
  FrozenDataSet frozeuc;
  frozeuc.fromDataSet(*eucds);
  delete eucds;
  gaia2::verbose = true;

  G_INFO("FrozenDataSets ready");
  G_INFO("");

  // ---------------------------------------------------------------------------
  G_INFO(G_HLINE "\n");

  const char* hybridDistanceParams =
    "colfilter:                                          \n"
    "    weight: 0.4                                     \n"
    "    distance: cosineangle                           \n"
    "    params:                                         \n"
    "        descriptorNames: colfilter                  \n"
    "        defaultValue: 0.0                           \n"
    "                                                    \n"
    "context:                                            \n"
    "    weight: 0.4                                     \n"
    "    distance: cosineangle                           \n"
    "    params:                                         \n"
    "        descriptorNames: context                    \n"
    "        defaultValue: 0.0                           \n"
    "                                                    \n"
    "content:                                            \n"
    "    weight: 0.2                                     \n"
    "    distance: euclidean                             \n"
    "    params:                                         \n"
    "        descriptorNames: pca                        \n"
    "                                                    \n";


  // pick a point for doing all the queries
  const Point& query = *dataset.at(1234);
  const float* fquery = query.frealData().constBegin();

  G_DEBUG(GTools, "query point: " << query.name() << '\n');


  // ---------------------------------------------------------------------------

  G_INFO('\n' << G_TITLE("USING HYBRID DISTANCE") << '\n');

  ParameterMap params = toParameterMap(yaml::load(hybridDistanceParams));
  DistanceFunction* dist = MetricFactory::create("LinearCombination", dataset.layout(), params);

  normalSearch(dataset, dist, query, N);

  // ---------------------------------------------------------------------------

  G_INFO('\n' << G_TITLE("USING ONLY EUCLIDEAN DISTANCE") << '\n');
  N = 100;

  DistanceFunction* content = MetricFactory::create("Euclidean", dataset.layout(),
                                                    params["content"].toParameterMap()["params"].toParameterMap());

  normalSearch(dataset, content, query, N);

  // ---------------------------------------------------------------------------

  frozenSearch(dataset, frozenDataSet+150, 175, fquery+150, N, EuclideanDistance(25));

  // ---------------------------------------------------------------------------

  G_INFO("(Using compacted datasets without context/colfilt data)");
  frozenSearch(dataset, frozenDataSet2, 25, fquery+150, N, EuclideanDistance(25));

  // ---------------------------------------------------------------------------

  deepFreeze(frozeuc, FrozenEuclideanDistance(frozeuc, 0, 25),
             Eigen::RowVectorXf::Map(fquery+150, 25), N);

  // ---------------------------------------------------------------------------

  G_INFO("(Folding data so that it can fit into L2 cache, but doing the\n"
         " exact same amount of computation as the previous line)");
  G_DEBUG(GTools, "[[[ Note that the results here will be incorrect ]]]");
  deepFoldedFreeze(frozeuc, FrozenEuclideanDistance(frozeuc, 0, 25),
                   Eigen::RowVectorXf::Map(fquery+150, 25), N);

  // ---------------------------------------------------------------------------


  multiQuery<FrozenEuclideanDistance, 0, 25>(frozeuc, 2);
  multiQuery<FrozenEuclideanDistance, 0, 25>(frozeuc, 5);
  multiQuery<FrozenEuclideanDistance, 0, 25>(frozeuc, 10);


  // ---------------------------------------------------------------------------

  delete dist;
  delete content;
  delete[] frozenDataSet;
  delete[] frozenDataSet2;

  gaia2::shutdown();

  return 0;
}

