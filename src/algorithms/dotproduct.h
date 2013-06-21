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
#ifndef GAIA_DOTPRODUCT_H
#define GAIA_DOTPRODUCT_H

#include "applier.h"

namespace gaia2 {

typedef QMap<Segment, QPair<RealDescriptor, RealDescriptor> > CoeffMap;

/**
 * @ingroup appliers
 * DotProduct applier class. Multiplies each coordinate of the given point
 * by the ones given in the constructor and adds (optional) initial values.
 */
class DotProduct : public Applier {

 public:
  DotProduct(const Transformation& transfo);
  virtual ~DotProduct();

  /**
   * Ownership of resulting point is handed to caller of this function.
   */
  virtual Point* mapPoint(const Point* p) const;


 protected:
  Point* mapPoint(const Point* p, const CoeffMap& coeffs) const;

  CoeffMap prepareCoeffMap(const PointLayout& layout) const;

  bool _applyClipping;
  CoeffMap _coeffMapCache;

};

} // namespace gaia2

#endif // GAIA_DOTPRODUCT_H
