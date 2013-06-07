#include "frozendistance.h"

#include "frozeneuclideandistance.h"
#include "frozencosineangledistance.h"
#include "frozenlinearcombinationdistance.h"
#include "frozenexponentialcompressdistance.h"

#define REGISTER_FROZEN_METRIC(name) FrozenMetricFactory::Registrar<Frozen##name##Distance> register##name(#name);

namespace gaia2 {

void registerFrozenMetrics() {
  REGISTER_FROZEN_METRIC(Euclidean);
  REGISTER_FROZEN_METRIC(CosineAngle);
  REGISTER_FROZEN_METRIC(LinearCombination);
  REGISTER_FROZEN_METRIC(ExponentialCompress);
}

} // namespace gaia2
