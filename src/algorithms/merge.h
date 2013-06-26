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

#ifndef GAIA_MERGE_H
#define GAIA_MERGE_H

#include "applier.h"

namespace gaia2 {

/**
 * @ingroup appliers
 * Merge applier class. Merges together 2 points which have had a different
 * history of transformations, but which both stem from the same original
 * point.
 */
class Merge : public Applier {

 protected:
  TransfoChain _history1, _history2;

 public:
  Merge(const Transformation& transfo);
  virtual ~Merge();

  virtual Point* mapPoint(const Point* p) const;

  /**
   * Optimized version so that we don't have to recompute the index mappings
   * for each point.
   */
  DataSet* mapDataSet(const DataSet* dataset) const;

};

} // namespace gaia2

#endif // GAIA_MERGE_H
