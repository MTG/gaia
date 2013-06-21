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
#ifndef GAIA_BPFCOMPOSE_H
#define GAIA_BPFCOMPOSE_H

#include "distancefunction.h"

namespace gaia2 {

/**
 * @ingroup metrics compositemetrics
 *
 * This class composes the breakpoint function specified by its pairs of (split point, value)
 * with the given distance.
 *
 * @param distance the name of the distance to apply the BPF to.
 * @param params the parameters for creating the distance we compose with.
 * @param breakPoints the pairs (split point, value) that define the BPF.
 */
class BPFDistance : public DistanceFunction {
 public:
  BPFDistance(const PointLayout& layout, const ParameterMap& params);
  ~BPFDistance();

  Real operator()(const Point& p1, const Point& p2, int seg1, int seg2) const;

 protected:

  class BPF {
   protected:
    std::vector<Real> _xpoints, _ypoints;

   public:
    void setBreakPoints(QList<QPointF> bpoints);
    Real operator()(Real x) const;
  };

  BPF _bpf;
  DistanceFunction* _dist;
};

} // namespace gaia2

#endif // GAIA_BPFCOMPOSE_H
