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
