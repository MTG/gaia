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

#ifndef GAIA_FROZENSEARCH_H
#define GAIA_FROZENSEARCH_H

#include <iostream>
#include "dataset.h"
#include "frozendataset.h"
#include "frozendistance.h"
#include "frozeneuclideandistance.h"
#include "frozencosineangledistance.h"
#include "frozenlinearcombinationdistance.h"
#include "distancefunction.h"
#include "timer.h"

// Some utility functions for use with the gaiabench tool

namespace gaia2 {

// return a right-aligned formatted string with fixed precision that goes up to 3 digits in front
// also converts from seconds to milliseconds
inline QString RTIME(double t) {
  QString num = QString::number(t*1000, 'f', 4);
  return QString(8-num.size(), ' ') + num;
}


void normalSearch(DataSet& dataset, const DistanceFunction* dist, const Point& query, int N);


class EuclideanDistance {
  int dim;
public:
  EuclideanDistance(int dimension) : dim(dimension) {}

  float operator()(const float* const p1, const float* const p2) const;
};

class CosineDistance {
  int dim;
public:
  CosineDistance(int dimension) : dim(dimension) {}

  float operator()(const float* const p1, const float* const p2) const;
};



//typedef pair<float, qint32> fspoint;
typedef std::pair<float, int> fspoint;


template <typename DistanceFunction>
void frozenSearch(const DataSet& dataset, const float* frozenDataSet, int dimension,
                  const float* queryPoint, int N, const DistanceFunction& dist) {
  int npoints = dataset.size();

  std::vector<fspoint> result(npoints);

  Timer chrono;
  chrono.start();
  for (int n=0; n<N; n++) {

    for (int i=0; i<npoints; i++) {
      result[i].first = dist(&frozenDataSet[i*dimension], queryPoint);
      result[i].second = i;
    }

    partial_sort(result.begin(), result.begin() + 100, result.end());
  }
  chrono.stop();

  G_INFO("Average on " << N << " runs using naive frozen Gaia datasets and views: "
         << RTIME(chrono.elapsed()/N) << "ms / query");

  for (int i=0; i<5; i++) {
    G_DEBUG(GTools, i << " - " << result[i].first << " - " << dataset[result[i].second]->name());
  }
  G_DEBUG(GTools, "");
}





void deepFreeze(FrozenDataSet& frozenDS, const FrozenDistance& dist, const Eigen::RowVectorXf& queryPoint, int N);
void deepFoldedFreeze(FrozenDataSet& frozenDS, const FrozenDistance& dist, const Eigen::RowVectorXf& queryPoint, int N);
void deepMultiFreeze(FrozenDataSet& frozenDS, const QList<FrozenDistance*>& dists,
                     const QList<Eigen::RowVectorXf> queries, int N);

} // namespace gaia2

#endif // GAIA_FROZENSEARCH_H
