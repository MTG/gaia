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

#ifndef GAIA_MERGEREGIONAPPLIER_H
#define GAIA_MERGEREGIONAPPLIER_H

#include "applier.h"

namespace gaia2 {

/**
 * @ingroup appliers
 * MergeRegion applier class. Merges multiple real descriptors into a single one.
 */
class MergeRegionApplier : public Applier {
 protected:
  QString _resultName;
  QStringList _select;
  PointLayout _layout;

 public:
  MergeRegionApplier(const Transformation& transfo);
  virtual ~MergeRegionApplier();

  /**
   * Ownership of resulting point is handed to caller of this function.
   */
  virtual Point* mapPoint(const Point* p) const;

 protected:
  Point* mergePoint(const Point* p, const Region& region) const;

  // WARNING: this makes it non-thread safe
  mutable PointLayout _layoutCache;
  mutable Region _regionCache;

};

} // namespace gaia2

#endif // GAIA_MERGEREGIONAPPLIER_H
