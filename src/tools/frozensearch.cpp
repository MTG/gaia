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
#include "frozensearch.h"
#include "view.h"
using namespace std;
using namespace gaia2;


void gaia2::normalSearch(DataSet& dataset, const DistanceFunction* dist, const Point& query, int N) {
  View v(&dataset);

  Timer chrono;

  chrono.start();
  for (int i=0; i<N; i++) {
    v.nnSearch(query, dist);
  }
  chrono.stop();

  G_INFO("Average on " << N << " runs using normal Gaia datasets and views: "
         "      " << RTIME(chrono.elapsed()/N) << "ms / query");

  SearchResults results = v.nnSearch(query, dist).get(5);
  for (int i=0; i<5; i++) {
    G_DEBUG(GTools, i << " - " << results[i].second << " - " << results[i].first);
  }
  G_DEBUG(GTools, "");
}


float EuclideanDistance::operator()(const float* const p1, const float* const p2) const {
  float dist = 0.0;
  for (int i=0; i<dim; i++) {
    float d = p1[i] - p2[i];
    dist += d*d;
  }
  return sqrt(dist);
}


float CosineDistance::operator()(const float* const p1, const float* const p2) const {
  float dist = 0.0;
  float norm1 = 0.0, norm2 = 0.0;

  for (int i=0; i<dim; i++) {
    Real v1 = p1[i];
    Real v2 = p2[i];
    dist += v1*v2;
    norm1 += v1*v1;
    norm2 += v2*v2;
  }

  if (norm1*norm2 < 1e-12) {
    return 0.0;
  }

  dist /= sqrt(norm1 * norm2);

  // make sure we don't overflow stupidly because of precision issues...
  dist = clip(dist, -1.0, 1.0);

  return acos(dist)/M_PI;
}


//   0-100 : colfilter  cosine dist
// 100-150 : context    cosine dist
// 150-175 : content    euclidean dist
FrozenBmatDistance::FrozenBmatDistance(const FrozenDataSet& dataset, int offset, int size) :
  FrozenDistance(dataset),
  colfiltDist(dataset, "colfilter"),
  contextDist(dataset, "context"),
  contentDist(dataset, "pca") {}


void FrozenBmatDistance::prepare(const Eigen::RowVectorXf& query) {
  colfiltDist.prepare(query);
  contextDist.prepare(query);
  contentDist.prepare(query);
}

Real FrozenBmatDistance::operator()(int i, const Eigen::RowVectorXf& query) const {
  return 0.4*colfiltDist(i, query) + 0.4*contextDist(i, query) + 0.2*contentDist(i, query);
}


void gaia2::deepFreeze(FrozenDataSet& frozenDS, const FrozenDistance& dist,
                       const Eigen::RowVectorXf& query, int N) {
  const int npoints = frozenDS.rows();

  vector<fspoint> result(npoints);

  FrozenView view(&frozenDS);

  Timer chrono;
  chrono.start();
  for (int n=0; n<N; n++) {
    view.nnSearch(query, &dist);
  }
  chrono.stop();


  G_INFO("Average on " << N << " runs using gaia2::FrozenDataSets and views: "
         "     " << RTIME(chrono.elapsed()/N) << "ms / query");

  SearchResults results = view.nnSearch(query, &dist).get(5);
  for (int i=0; i<5; i++) {
    G_DEBUG(GTools, i << " - " << results[i].second << " - " << results[i].first);
  }
  G_DEBUG(GTools, "");
}

void gaia2::deepMultiFreeze(FrozenDataSet& frozenDS, const QList<FrozenDistance*>& dists,
                            const QList<Eigen::RowVectorXf> queries, int N) {
  const int npoints = frozenDS.rows();
  const int nqueries = queries.size();

  vector<vector<fspoint> > results(queries.size(), vector<fspoint>(npoints));

  // can't use this here because we want the order of the loop to be reversed, ie:
  // point by point instead of query by query
  // vector<FrozenView> views(queries.size(), FrozenView(&frozenDS));

  Timer chrono;
  chrono.start();
  for (int n=0; n<N; n++) {
    for (int q=0; q<nqueries; q++) {
      const_cast<FrozenDistance*>(dists[q])->prepare(queries[q]);
    }

    // do it in this order and not the other one (for query - for point) so that whenever
    // we need the data from a point, it is hot in the cache and we don't need to refetch it
    // each time from the RAM
    for (int i=0; i<npoints; i++) {
      for (int q=0; q<nqueries; q++) {
        results[q][i].first = (*dists[q])(i, queries[q]);
        results[q][i].second = i;
      }
    }


    for (int q=0; q<nqueries; q++) {
      partial_sort(results[q].begin(), results[q].begin() + 100, results[q].end());
    }
  }
  chrono.stop();

  G_INFO("Average on " << N << " runs using gaia2::FrozenDataSets and views: "
         "     " << RTIME(chrono.elapsed()/(N*nqueries)) << "ms / query");

  for (int i=0; i<5; i++) {
    G_DEBUG(GTools, i << " - " << results[0][i].first << " - " << frozenDS.pointName(results[0][i].second));
  }
  G_DEBUG(GTools, "");
}

void gaia2::deepFoldedFreeze(FrozenDataSet& frozenDS, const FrozenDistance& dist,
                             const Eigen::RowVectorXf& query, int N) {
  const int npoints = frozenDS.rows();

  vector<fspoint> result(npoints);

  Timer chrono;
  chrono.start();
  for (int n=0; n<N; n++) {
    const_cast<FrozenDistance&>(dist).prepare(query);

    for (int i=0, fi=0; i<npoints; i++, fi++) {
      if (fi >= 1000) fi -= 1000; // make sure we stay in L2 cache
      result[i].first = dist(fi, query);
      result[i].second = i;
    }

    partial_sort(result.begin(), result.begin() + 100, result.end());
  }
  chrono.stop();

  G_INFO("Average on " << N << " runs using gaia2::FrozenDataSets and views: "
         "     " << RTIME(chrono.elapsed()/N) << "ms / query");

  for (int i=0; i<5; i++) {
    G_DEBUG(GTools, i << " - " << result[i].first << " - " << frozenDS.pointName(result[i].second));
  }
  G_DEBUG(GTools, "");
}
