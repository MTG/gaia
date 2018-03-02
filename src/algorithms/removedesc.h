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

#ifndef GAIA_REMOVEDESC_H
#define GAIA_REMOVEDESC_H

#include "applier.h"
#include "../utils.h"

namespace gaia2 {

/**
 * @ingroup appliers
 * RemoveDesc applier class. Removes specified dimensions from given point.
 */
class RemoveDesc : public Applier {
 protected:
  QStringList _remove;

 public:
  RemoveDesc(const Transformation& transfo);
  virtual ~RemoveDesc();

  /**
   * Ownership of resulting point is handed to caller of this function.
   */
  virtual Point* mapPoint(const Point* p) const;

 protected:
  PointLayout mapLayout(const PointLayout& layout) const;
  PointLayout _newLayout;
  IndexMap _realMap, _stringMap, _enumMap;

};

} // namespace gaia2

#endif // GAIA_REMOVEDESC_H
