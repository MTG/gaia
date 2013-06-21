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
#include "gaia2std.h"
#include "../gaia.h"

namespace gaia2std {

void init() {
  ::gaia2::init();
}

bool verbose() {
  return ::gaia2::verbose;
}

void setVerbose(bool verbosity) {
  ::gaia2::verbose = verbosity;
}

void setDebugLevel(int levels) {
  ::gaia2::setDebugLevel(levels);
}

void unsetDebugLevel(int levels) {
  ::gaia2::unsetDebugLevel(levels);
}


} // namespace gaia2std
