#ifndef GAIA_KEYDISTANCE_H
#define GAIA_KEYDISTANCE_H

#include "distancefunction.h"

namespace gaia2 {

/**
 * @ingroup metrics refmetrics
 *
 * This class computes the harmonic distance between keys. Range is from 0 to 1.
 * This distance has been written by Nico and is more likely to serve as an
 * example than a useful distance, but still...
 *
 * @param name the name of the descriptor containing the key value. Default is
 *        "key_key".
 *
 */
class KeyDistance : public DistanceFunction {
 public:
  // @todo use keyIndex or Region?
  KeyDistance(const PointLayout& layout, const ParameterMap& params);
  Real operator()(const Point& p1, const Point& p2, int seg1, int seg2) const;

 protected:
  int _keyIndex;

};

} // namespace gaia2

#endif // GAIA_KEYDISTANCE_H
