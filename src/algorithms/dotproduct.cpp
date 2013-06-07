#include <QPair>
#include "dotproduct.h"
using namespace std;
using namespace gaia2;

DotProduct::~DotProduct() {}

DotProduct::DotProduct(const Transformation& transfo) : Applier(transfo) {
  _applyClipping = transfo.params.value("applyClipping", false).toBool();

  _coeffMapCache = prepareCoeffMap(transfo.layout);
}

CoeffMap DotProduct::prepareCoeffMap(const PointLayout& layout) const {
  ParameterMap params = _transfo.params.value("coeffs").toParameterMap();
  CoeffMap coeffs;

  foreach (QString desc, params.keys()) {
    RealDescriptor a = params[desc].toParameterMap()["a"].toRealDescriptor();
    RealDescriptor b = params[desc].toParameterMap()["b"].toRealDescriptor();

    G_DEBUG(GAlgorithms, desc << a << b);

    coeffs.insert(layout.descriptorLocation(desc).segment(), qMakePair(a, b));
  }

  return coeffs;
}

Point* DotProduct::mapPoint(const Point* p, const CoeffMap& coeffs) const {
  Point* result = new Point(*p);

  FORSEG(*result) {
    RealDescriptor& fdata = result->frealData(nseg);
    Array<RealDescriptor>& vdata = result->vrealData(nseg);

    foreach (const Segment& seg, coeffs.keys()) {
      const QPair<RealDescriptor, RealDescriptor>& vcoeff = coeffs.value(seg);
      switch (seg.ltype) {
      case FixedLength: {
        for (int i=0; i<seg.size(); i++) {
          fdata[seg.begin + i] *= vcoeff.first[i];
          fdata[seg.begin + i] += vcoeff.second[i];
        }
        if (_applyClipping) {
          for (int i=0; i<seg.size(); i++) {
            fdata[seg.begin + i] = std::min(fdata[seg.begin + i], Real(1.0));
            fdata[seg.begin + i] = std::max(fdata[seg.begin + i], Real(0.0));
          }
        }
        break;
      }
      case VariableLength: {
        //data[i] = data[i] * _coeffs[i].first + _coeffs[i].second;
        // does this optimize much?
        int i = seg.begin;

        vdata[i] *= vcoeff.first;
        vdata[i] += vcoeff.second;

        if (_applyClipping) {
          vdata[i].minimum(1);
          vdata[i].maximum(0);
        }
        break;
      }

      }
    }
  }

  return result;
}

Point* DotProduct::mapPoint(const Point* p) const {
  checkLayout(p->layout());
  return mapPoint(p, _coeffMapCache);
}
