#ifndef GAIA_FROZENEUCLIDEANDISTANCE_H
#define GAIA_FROZENEUCLIDEANDISTANCE_H

#include "frozendistance.h"

namespace gaia2 {

class FrozenEuclideanDistance : public FrozenDistance {
public:

  FrozenEuclideanDistance(const FrozenDataSet& dataset, int offset, int size)
    : FrozenDistance(dataset), _offset(offset), _size(size) {}


  FrozenEuclideanDistance(const FrozenDataSet& dataset, const QString& descriptorName)
    : FrozenDistance(dataset) {
    init(descriptorName);
  }

  FrozenEuclideanDistance(const FrozenDataSet& dataset, const std::string& descriptorName)
    : FrozenDistance(dataset) {
    init(QString::fromStdString(descriptorName));
  }

  FrozenEuclideanDistance(const FrozenDataSet& dataset, const char* descriptorName)
    : FrozenDistance(dataset) {
    init(QString::fromUtf8(descriptorName));
  }

  FrozenEuclideanDistance(const FrozenDataSet& dataset, const ParameterMap& params)
    : FrozenDistance(dataset) {

    if (params.contains("descriptorName")) {
      init(params.value("descriptorName"));
    }
    else {
      init(0, dataset.dimension());
    }

  }

  void init(const QString& descriptorName) {
    validParams << "descriptorName";

    QPair<int, int> pos = _dataset.descriptorLocation(descriptorName);
    init(pos.first, pos.second - pos.first);
  }

  void init(int offset, int size) {
    _offset = offset;
    _size = size;
  }

  Real operator()(int i, const FrozenPoint& query) const {
     return (_dataset.row(i).segment(_offset, _size) - query.segment(_offset, _size)).norm();
  }

protected:
  int _offset, _size;

};


} // namespace gaia2

#endif // GAIA_FROZENEUCLIDEANDISTANCE_H
