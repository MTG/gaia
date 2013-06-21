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
#include "gaiamath.h"
#include "alglib/normaldistr.h"
#include "alglib/chisquaredistr.h"

namespace gaia2 {

double erfinv(double P) {
  return inverf(P);
}

double norminv(double P, double mu, double sigma) {
  return mu + sigma * invnormaldistribution(P);
}

double chi2inv(double P, int v) {
  return invchisquaredistribution(v, 1.0-P);
}

} // namespace gaia2
