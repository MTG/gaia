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
#ifndef GAIA_STL_GAIA2STD_H
#define GAIA_STL_GAIA2STD_H

#include "origtypes.h"

namespace gaia2std {

/**
 * Call this function before doing anything with gaia. If you forget it, you
 * will not be able to instantiate analyzers, distance functions, etc...
 */
void init();

/**
 * Returns whether the global verbosity flag is set to true or false.
 */
bool verbose();

/**
 * Set the global verbosity flag.
 */
void setVerbose(bool verbosity);


// Debugging functions

enum DebuggingModules {
  // general modules
  GMemory         = 1 << 0,
  GAlgorithms     = 1 << 1,
  GMath           = 1 << 2,
  GParser         = 1 << 3,
  GIO             = 1 << 4,

  // class specific modules
  GDataSet        = 1 << 10,
  GDescriptorTree = 1 << 11,
  GRegion         = 1 << 12,
  GPoint          = 1 << 13,
  GView           = 1 << 14,

  GNone           = 0,
  GAll            = (1 << 30) -1
};

void setDebugLevel(int levels);
void unsetDebugLevel(int levels);

} // namespace gaia2std


#endif // GAIA_STL_GAIA2STD_H
