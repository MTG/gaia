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
