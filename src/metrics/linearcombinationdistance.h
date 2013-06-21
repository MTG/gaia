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
#ifndef GAIA_LINEARCOMBINATIONDISTANCE_H
#define GAIA_LINEARCOMBINATIONDISTANCE_H

#include "distancefunction.h"

namespace gaia2 {

/**
 * @ingroup metrics compositemetrics
 *
 * This class computes a distance that is the linear combination of any number
 * of other DistanceFunctions.
 * For instance, in a recommender system, a set of weights could be seen as a
 * user profile, each user having its own preferences regarding which distance
 * is more useful to him.
 *
 * @param parameterMap the parameterMap passed as argument should contain mapping
 *        from a name to a map containing the following fields: "distance", "params"
 *        and "weight".
 * @param distance the name of the metric to create.
 * @param params the ParameterMap to be used when creating the specified metric.
 * @param weight the weight to be assigned to this metric in the linear
 *        combination.
 */
class LinearCombinationDistance : public DistanceFunction {
 protected:
  QVector<QPair<DistanceFunction*, Real> > _weights;

 public:
  LinearCombinationDistance(const PointLayout& layout, const ParameterMap& params);
  ~LinearCombinationDistance();
  void addDistance(DistanceFunction* f, Real coeff);
  Real operator()(const Point& p1, const Point& p2, int seg1, int seg2) const;
};

} // namespace gaia2

#endif // GAIA_LINEARCOMBINATIONDISTANCE_H
