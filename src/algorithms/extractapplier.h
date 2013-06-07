#ifndef GAIA_EXTRACTAPPLIER_H
#define GAIA_EXTRACTAPPLIER_H

#include "applier.h"
#include "3rdparty/libsvm/svm.h"
#include "utils.h"

namespace gaia2 {

class InnerDim {
public:
  InnerDim(DescriptorLengthType ltype_, int desc_, int dim_) :
    ltype(ltype_), desc(desc_), dim(dim_) {}
  DescriptorLengthType ltype;
  int desc;
  int dim;
};


/**
 * @ingroup appliers
 * Extract applier class. Extracts specified dimensions from descriptors and
 * promotes them to individual descriptors.
 */
class ExtractApplier : public Applier {

 public:
  ExtractApplier(const Transformation& transfo);
  virtual ~ExtractApplier();

  /**
   * Ownership of resulting point is handed to caller of this function.
   */
  virtual Point* mapPoint(const Point* p) const;

 protected:
  QMap<QString, QList<int> > _dims;
  PointLayout _newLayout;
  IndexMap _realMap, _stringMap, _enumMap;

  typedef QList<QPair<InnerDim, InnerDim> > Mapping;
  Mapping _mapping;

  Mapping getMappings(const PointLayout& layout) const;
  void transferData(Point* p, const Mapping& m) const;

};

} // namespace gaia2

#endif // GAIA_EXTRACTAPPLIER_H
