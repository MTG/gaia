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
#ifndef GAIA_SELECTDESC_H
#define GAIA_SELECTDESC_H

#include "applier.h"
#include "utils.h"

namespace gaia2 {

/**
 * @ingroup appliers
 * SelectDesc applier class. Selects specified dimensions from given point.
 */
class SelectDesc : public Applier {
 protected:
  QStringList _select;

 public:
  SelectDesc(const Transformation& transfo);
  virtual ~SelectDesc();

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

#endif // GAIA_SELECTDESC_H
