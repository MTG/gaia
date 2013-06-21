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
#include "bpfdistance.h"
using namespace gaia2;


bool compareByX(const QPointF& p1, const QPointF& p2) {
  return p1.x() < p2.x();
}

void BPFDistance::BPF::setBreakPoints(QList<QPointF> bpoints) {
  sort(bpoints.begin(), bpoints.end(), compareByX);
  int size = bpoints.size();

  // assert no two X are equals
  for (int i=0; i<size-1; i++) {
    if (bpoints[i].x() >= bpoints[i+1].x()) {
      throw GaiaException("BPF: you can't have 2 points with the same X coordinate.");
    }
  }

  _xpoints.resize(size);
  _ypoints.resize(size);

  for (int i=0; i<size; i++) {
    _xpoints[i] = bpoints[i].x();
    _ypoints[i] = bpoints[i].y();
  }
}

Real BPFDistance::BPF::operator()(Real x) const {
  int size = (int)_xpoints.size();

  if (x <= _xpoints[0]) return _ypoints[0];
  if (x >= _xpoints[size-1]) return _ypoints[size-1];

  // binary search because we might have quite a few bins
  int start = 0, end = size;
  while (start < end) {
    int middle = (start + end) / 2;
    if (_xpoints[middle] < x) {
      start = middle + 1;
    }
    else {
      end = middle;
    }
  }

  // start = index of the first bin which is >= x
  Q_ASSERT(start > 0); // can't be 0 because we removed that option when entering the function
  Q_ASSERT(start < size); // can't be size because we removed that option when entering the function

  Real xa = _xpoints[start-1], ya = _ypoints[start-1];
  Real xb = _xpoints[start], yb = _ypoints[start];

  return ya + (yb-ya)/(xb-xa) * (x - xa);
}



BPFDistance::BPFDistance(const PointLayout& layout,
                         const ParameterMap& params) : DistanceFunction(layout, params) {
  validParams = QStringList() << "distance" << "params" << "breakPoints";

  _bpf.setBreakPoints(params.value("breakPoints").toCoords());

  _dist = MetricFactory::create(params.value("distance"),
                                layout,
                                params.value("params", ParameterMap()).toParameterMap());
}


BPFDistance::~BPFDistance() {
  delete _dist;
}


Real BPFDistance::operator()(const Point& p1, const Point& p2, int seg1, int seg2) const {
  return _bpf((*_dist)(p1, p2, seg1, seg2));
}
