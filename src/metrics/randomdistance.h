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

/* <copyright entity="UPF">
# UPF. All Right Reserved, http://www.upf.edu/
#
# This source is subject to the Contributor License Agreement of the Essentia project.
# Please see the CLA.txt file available at http://essentia.upf.edu/contribute/
# for more
# information.
#
# THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
# KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
# PARTICULAR PURPOSE.
#
# </copyright>
*/


#ifndef GAIA_RANDOMDISTANCE_H
#define GAIA_RANDOMDISTANCE_H

#include "distancefunction.h"
#include "../3rdparty/MersenneTwister.h"

namespace gaia2 {

/**
 * @ingroup metrics refmetrics
 *
 * This class computes a distance which returns a random number between
 * 0 and the specified range. It uses a uniform distribution.
 *
 * @param range the max value this distance can return (minimum being 0).
 */
class RandomDistance : public DistanceFunction {
 public:
  RandomDistance(const PointLayout&, const ParameterMap&);

  Real operator()(const Point& p1, const Point& p2, int seg1, int seg2) const;

 protected:
  mutable MTRand _mtrand;
  Real _range;
};

} // namespace gaia2

#endif // GAIA_RANDOMDISTANCE_H
