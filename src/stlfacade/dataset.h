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
#ifndef GAIA_STL_DATASET_H
#define GAIA_STL_DATASET_H

#include <vector>
#include <string>
#include "origtypes.h"
#include "point.h"
#include "transformation.h"

namespace gaia2std {

class DataSet {
  DECLARE_PROXY(DataSet);
  friend class Analyzer;
  friend class Transformation;
  friend class DistanceFunction;
  friend class View;

 public:
  DataSet();

  std::string name() const;
  void setName(const std::string& name);
  void setReferenceDataSet(DataSet* dataset);

  std::vector<std::string> pointNames() const;

  proxy_ptr<const Point> point(const std::string& id) const;
  proxy_ptr<Point> point(const std::string& id);

  void addPoint(Point* point);
  void removePoint(const std::string& id);

  bool empty() const;
  int size() const;

  void load(const std::string& filename);
  void save(const std::string& filename);
};

} // namespace gaia2std

#endif // GAIA_STL_DATASET_H
