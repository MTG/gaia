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
