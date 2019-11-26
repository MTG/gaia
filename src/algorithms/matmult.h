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

#ifndef GAIA_MATMULT_H
#define GAIA_MATMULT_H

#include "applier.h"
#include <Eigen/Core>

namespace gaia2 {

/**
 * @ingroup appliers
 * MatMult class.
 */
class MatMult : public Applier {

 public:
  MatMult(const Transformation& transfo);
  virtual ~MatMult();

  /**
   * Ownership of resulting point is handed to caller of this function.
   */
  virtual Point* mapPoint(const Point* p) const;

 protected:
  QStringList _descriptorNames;
  Eigen::MatrixXf _matrix;
  PointLayout _layout;

  Point* mapPoint(const Point* p, const Region& region) const;

  Region _regionCache;
};

} // namespace gaia2

#endif // GAIA_MATMULT_H
