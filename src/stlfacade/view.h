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

#ifndef GAIA_STL_VIEW_H
#define GAIA_STL_VIEW_H

#include <vector>
#include "origtypes.h"

namespace gaia2std {

typedef std::pair<std::string, float> Result;
typedef std::vector<Result> SearchResults;


class View {
  DECLARE_PROXY(View);

 public:
  View(const DataSet* dataset);

  SearchResults nnSearch(const Point& p, const DistanceFunction& dfunc,
                         int numNeighbors, const std::string& filter = "");

  SearchResults nnSearch(const std::string& id, const DistanceFunction& dfunc,
                         int numNeighbors, const std::string& filter = "");

};


} // namespace gaia2std

#endif // GAIA_STL_VIEW_H
