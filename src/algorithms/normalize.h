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
#ifndef GAIA_NORMALIZE_H
#define GAIA_NORMALIZE_H

#include "analyzer.h"

namespace gaia2 {

/**
 * @ingroup analyzers datatransformationalgos
 *
 * The Normalize analyzer normalizes real descriptors.
 * There are various ways of doing normalization, the most common being to just
 * normalize the values so that they fall within the range [0; 1].
 *
 * @b NB: This algorithm does not yet work with multi-segments points.
 *
 * @param collections the list of collections from which to gather the stats.
 *        the transformation will still apply to all of them, though.
 *        (default: [], meaning all of them)
 * @param descriptorNames the names of the descriptors to be included in the
 *        normalization. Only accepts real descriptors, can use wildcards.
 *        (default: "*").
 * @param except the names of the descriptors to be removed from the selected
 *        list specified before. Can use wildcards. (default: "").
 * @param type the type of normalization. Valid types are:
 *        @li @c "range" which normalizes using the minimum and maximum values.
 *        @li @c "variance" which normalizes using the variance of the values.
 *
 *        (default: "range").
 * @param independent indicates whether to normalize each dimension from a
 *        single descriptor independently, or if the vector should be normalized
 *        as a whole (default: false).
 * @param outliers the ratio (between 0 and 1) of outliers when computing the
 *        statistics used for normalization. Outliers are the values which are
 *        not taken into account because they have too extreme values. A value
 *        of -1 means do not use outliers (default: -1).
 * @param applyClipping whether to apply clipping, eg: fitting the values into
 *        the [0;1] range (default: false).
 */
class Normalize : public Analyzer {

 public:
  Normalize(const ParameterMap& params);
  virtual ~Normalize() {};

  Transformation analyze(const DataSet* data, const Region& region) const;


 protected:
  Real _outliers;


  void getStats(const DataSet& dataset, const Region& region,
                Point& mmin, Point& mmax,
                Point& mean, Point& var) const;

  void getStatsWithOutliers(const DataSet& dataset, const Region& region,
                            Point& mmin, Point& mmax,
                            Point& mean, Point& var) const;

  ParameterMap getNormalizationCoeffs(const QString& descriptorName,
                                      const Point& mmin, const Point& mmax,
                                      const Point& mean, const Point& var) const;

};

} // namespace gaia2

#endif // GAIA_NORMALIZE_H
