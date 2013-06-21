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
#ifndef GAIA_DISTRIBUTE_H
#define GAIA_DISTRIBUTE_H

#include "applier.h"

namespace gaia2 {

/**
 * @ingroup appliers
 * Distribute applier class. Rerank descriptor and gives it a new value so that
 * it fits in a specific distribution (gaussian for now, but could be extended
 * to any type of distribution).
 */
class Distribute : public Applier {

 public:
  Distribute(const Transformation& transfo);
  virtual ~Distribute();

  /**
   * Ownership of resulting point is handed to caller of this function.
   */
  virtual Point* mapPoint(const Point* p) const;

  //virtual DataSet* mapDataSet(const DataSet* dataset) const;

 protected:
  QMap<QString, Array<Real> > _distMap;

};

} // namespace gaia2

#endif // GAIA_DISTRIBUTE_H
