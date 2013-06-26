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

#ifndef GAIA_COSINESIMILARITY_H
#define GAIA_COSINESIMILARITY_H

#include "distancefunction.h"

namespace gaia2 {

/**
 * @ingroup metrics standardmetrics
 *
 * This class computes the cosine similarity of a given Region.
 *
 * @param descriptorNames the names of the descriptors to be included in the
 *        computation of the distance. Only accepts real descriptors, can use
 *        wildcards.
 * @param except the names of the descriptors to be removed from the selected
 *        list specified before. Can use wildcards.
 * @param defaultValue when 1 of the 2 points is null, then the similarity is not
 *        defined. If defaultValue is given, then it will be returned,
 *        otherwise an exception will be thrown.
 */
class CosineSimilarity : public DistanceFunction {
 public:
  CosineSimilarity(const PointLayout& layout, const ParameterMap& params);
  Real operator()(const Point& p1, const Point& p2, int seg1, int seg2) const;

 protected:
  bool _useDefaultValue;
  Real _defaultValue;
};

} // namespace gaia2

#endif // GAIA_COSINESIMILARITY_H
