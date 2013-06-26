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

#ifndef GAIA_STL_POINT_H
#define GAIA_STL_POINT_H

#include <vector>
#include "origtypes.h"

namespace gaia2std {

class Point {
  DECLARE_PROXY(Point);
  friend class DataSet;
  friend class Collection;
  friend class Transformation;
  friend class DistanceFunction;
  friend class View;

 public:
  Point();

  std::string name() const;
  void setName(const std::string& name);

  void load(const std::string& name);

  std::vector<float> value(const std::string& name) const;
  std::vector<std::string> label(const std::string& name) const;

  std::vector<float> value(int nsegment, const std::string& name) const;
  std::vector<std::string> label(int nsegment, const std::string& name) const;

  void setValue(const std::string& name, float value);
  void setValue(const std::string& name, const std::vector<float>& value);
  void setLabel(const std::string& name, const std::string& label);
  void setLabel(const std::string& name, const std::vector<std::string>& label);

  void setValue(int nsegment, const std::string& name, float value);
  void setValue(int nsegment, const std::string& name, const std::vector<float>& value);
  void setLabel(int nsegment, const std::string& name, const std::string& label);
  void setLabel(int nsegment, const std::string& name, const std::vector<std::string>& label);

 private:
  // @todo should these go into DECLARE_PROXY?
  Point(const Point&);
  Point& operator=(const Point&);

};

} // namespace gaia2std


#endif // GAIA_STL_POINT_H
