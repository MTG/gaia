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

#include <QVector>
#include <cmath>
#include "normalize.h"
#include "utils.h"
#include "algoutils.h"

namespace gaia2 {


Normalize::Normalize(const ParameterMap& params) : Analyzer(params) {
  validParams << "type" << "outliers" << "independent" << "applyClipping";

  _outliers = _params.value("outliers", -1).toDouble();

  if (_outliers != -1 &&
      !(_outliers >= 0.0 && _outliers <= 1.0)) {
    throw GaiaException("Outliers ratio should be between 0 and 1, or -1 if no outliers are desired");
  }
}


void Normalize::getStats(const DataSet& dataset,
                         const Region& region,
                         Point& mminPoint,
                         Point& mmaxPoint,
                         Point& meanPoint,
                         Point& varPoint) const {

  G_DEBUG(GAlgorithms, "Normalize: getting stats");

  QPair<Point, Point> mv = getRegionMeanVar(dataset, region);
  QPair<Point, Point> mm = getRegionMinMax(dataset, region);

  meanPoint = mv.first;
  varPoint  = mv.second;
  mminPoint = mm.first;
  mmaxPoint = mm.second;

  G_DEBUG(GAlgorithms, "Normalize: getting stats OK");
}

void Normalize::getStatsWithOutliers(const DataSet& dataset,
                                     const Region& region,
                                     Point& mminPoint,
                                     Point& mmaxPoint,
                                     Point& meanPoint,
                                     Point& varPoint) const {

  G_DEBUG(GAlgorithms, "getting stats with outliers");

  PointLayout layoutCopy = dataset.layout().copy();

  mminPoint.setLayout(layoutCopy);
  mmaxPoint.setLayout(layoutCopy);
  meanPoint.setLayout(layoutCopy);
  varPoint.setLayout(layoutCopy);

  RealDescriptor& fmin  = mminPoint.frealData();
  RealDescriptor& fmax  = mmaxPoint.frealData();
  RealDescriptor& fmean = meanPoint.frealData();
  RealDescriptor& fvar  = varPoint.frealData();

  int nSegments = dataset.totalSegments();
  QVector<Real> dist(nSegments);

  foreach (const Segment& seg, region.segments) {
    for (int didx=seg.begin; didx<seg.end; didx++) {
      int pidx = 0; // point index in distribution

      for (int i=0; i<dataset.size(); i++) {
        const Point* p = dataset.at(i);
        FORSEG(*p) {
          dist[pidx++] = p->frealData(nseg)[didx];
        }
      }

      Q_ASSERT(pidx == nSegments);

      sort(dist);

      int idxFirst = (int)((      _outliers/2.0) * (nSegments-1));
      int idxLast  = (int)((1.0 - _outliers/2.0) * (nSegments-1));
      int nump = idxLast - idxFirst;
      fmin[didx] = dist[idxFirst];
      fmax[didx] = dist[idxLast];
      fmean[didx] = gaia2::mean(dist.constData() + idxFirst, dist.size() - nump);
      fvar[didx] = gaia2::variance(dist.constData() + idxFirst, dist.size() - nump);
    }
  }

  G_DEBUG(GAlgorithms, "done getting stats with outliers");
}


Transformation Normalize::analyze(const DataSet* dataset, const Region& descs) const {
  G_INFO("Doing normalize analysis...");
  checkDataSet(dataset);
  checkMinPoints(dataset, 2);
  Region region = checkFixedLength(descs.select(RealType), dataset->layout());

  // get statistics on the input dataset
  Point mmin, mmax, mean, var;
  if (!_params.contains("outliers") || _params.value("outliers").toDouble() < 0.0) {
    getStats(*dataset, region, mmin, mmax, mean, var);
  }
  else {
    getStatsWithOutliers(*dataset, region, mmin, mmax, mean, var);
  }

  Transformation result(dataset->layout());
  result.analyzerName = "normalize";
  result.analyzerParams = _params;
  result.applierName = "dotproduct";

  // insert info values inside Transfo object: mean & var
  ParameterMap stats;

  foreach (const Segment& seg, region.segments) {
    // FIXME: not necessarily correct if some adjacent descriptors have been
    //        merged into a single segment
    QString descriptorName = mmin.layout().descriptorName(RealType, FixedLength, seg.begin);
    ParameterMap meanvar;
    meanvar.insert("mean", mean.value(descriptorName));
    meanvar.insert("var", var.value(descriptorName));
    stats.insert(descriptorName, meanvar);
  }

  result.info = stats;

  // insert actual Transformation parameters
  ParameterMap& applierParams = result.params;
  applierParams.insert("applyClipping", _params.value("applyClipping", false));
  ParameterMap coeffMap;

  foreach (const Segment& seg, region.segments) {
    QString descriptorName = mmin.layout().descriptorName(RealType, FixedLength, seg.begin);
    coeffMap.insert(descriptorName, getNormalizationCoeffs(descriptorName, mmin, mmax, mean, var));
  }

  applierParams.insert("coeffs", coeffMap);

  G_DEBUG(GAlgorithms, "normalization success!");
  return result;
}


ParameterMap Normalize::getNormalizationCoeffs(const QString& descriptorName,
                                               const Point& mmin, const Point& mmax,
                                               const Point& mean, const Point& var) const {
  RealDescriptor a, b;

  if (!_params.contains("type") ||   // default case: type was not defined
      _params.value("type") == "range") {
    // normalization using minimum and maximum
    // y = ax+b, y = (x - min)/(max - min), a=1/(max-min) and b = -min/(max-min)

    // normalization as a vector (default)
    if (!_params.contains("independent") || // default case
        !_params.value("independent").toBool()) {
      RealDescriptor maxd = mmax.value(descriptorName);
      RealDescriptor mind = mmin.value(descriptorName);
      Real maxv = maxd[0], minv = mind[0];

      for (int i=1; i<maxd.size(); i++) maxv = std::max(maxv, maxd[i]);
      for (int i=1; i<mind.size(); i++) minv = std::min(minv, mind[i]);

      a = RealDescriptor(maxd.size(), 1 / (maxv-minv) );
      b = RealDescriptor(mind.size(), -minv / (maxv - minv) );
    }
    // normalization each dimension separately
    else {
      a = (mmax.value(descriptorName) - mmin.value(descriptorName)).inverse();
      b = -mmin.value(descriptorName) * a;
    }

  }
  else if (_params.value("type") == "variance") {
    // normalization using 4 * stddev, between 0 and 1 (centered around 0.5)
    // y = ax+b, y = (x - mean)/(4*stddev) + 0.5, thus
    // a=1/(4*stddev) and b = -mean/(4*stddev)+0.5

    // normalization as a vector (default)
    if (!_params.contains("independent") || // default case
        !_params.value("independent").toBool()) {
      throw GaiaException("This type of normalization (type=variance, independent=false)"
                          " is not implemented at the moment");
    }
    // normalization each dimension separately
    else {
      a = Real(1.0) / (Real(4.0) * sqrt(var.value(descriptorName)));
      b = -mean.value(descriptorName) * a + 0.5;
    }

  }
  else {
    throw GaiaException("Normalize: unknown normalization type: ", _params.value("type").toString());
  }


  //G_DEBUG(GAlgorithms, "a:" << a << "b:" << b);

  // SECURITY CHECK - shouldn't happen, apply cleaner transfo before if it does...
  // happens for 2D arrays, where 2 first values are dimensions, hence constant, but
  // as a vector doesn't get cleaned
  if (a.isinf() || a.isnan()) {
    throw GaiaException("Normalize: Apply \"cleaner\" transformation before normalization. Division by zero in ", descriptorName);
  }

  ParameterMap coeffs;
  coeffs.insert("a", a);
  coeffs.insert("b", b);

  return coeffs;
}


} // namespace gaia2
