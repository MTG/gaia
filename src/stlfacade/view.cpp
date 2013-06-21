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
#include "../view.h"
#include "view.h"
#include "dataset.h"
#include "distancefunction.h"
#include "point.h"
using namespace std;

namespace gaia2std {

View::View(const DataSet* dataset) :
  _ownsData(true),
  _d(new ::gaia2::View(dataset->impl())) {}

PROXY_DESTRUCTOR(View);


SearchResults View::nnSearch(const Point& p, const DistanceFunction& dfunc,
                             int numNeighbors, const string& filter) {
  ::gaia2::SearchResults r = _d->nnSearch(*p.impl(),
                                          dfunc.impl(),
                                          QString::fromUtf8(filter.c_str(), filter.size())
                                          ).get(numNeighbors);
  SearchResults result(r.size());
  for (int i=0; i<r.size(); i++) {
    result[i] = make_pair(string(r[i].first.toUtf8().data()), r[i].second);
  }
  return result;
}

SearchResults View::nnSearch(const string& id, const DistanceFunction& dfunc,
                             int numNeighbors, const string& filter) {
  ::gaia2::SearchResults r = _d->nnSearch(QString::fromUtf8(id.c_str(), id.size()),
                                          dfunc.impl(),
                                          QString::fromUtf8(filter.c_str(), filter.size())
                                          ).get(numNeighbors);
  SearchResults result(r.size());
  for (int i=0; i<r.size(); i++) {
    result[i] = make_pair(string(r[i].first.toUtf8().data()), r[i].second);
  }
  return result;
}


} // namespace gaia2std
