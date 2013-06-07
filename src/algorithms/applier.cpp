#include "applier.h"

// appliers
#include "removedesc.h"
#include "selectdesc.h"
#include "mergeregionapplier.h"
#include "dotproduct.h"
#include "matmult.h"
#include "merge.h"
#include "addfieldapplier.h"
#include "fixlengthapplier.h"
#include "enumerateapplier.h"
#include "distribute.h"
#include "svmpredict.h"
#include "extractapplier.h"
#include "renameapplier.h"

namespace gaia2 {

#define REGISTER(classname, name) ApplierFactory::Registrar<classname> register##classname(name)

void registerAppliers() {
  REGISTER(RemoveDesc, "RemoveDesc");
  REGISTER(SelectDesc, "SelectDesc");
  REGISTER(MergeRegionApplier, "MergeRegionApplier");
  REGISTER(DotProduct, "DotProduct");
  REGISTER(MatMult, "MatMult");
  REGISTER(Merge, "Merge");
  REGISTER(AddFieldApplier, "AddFieldApplier");
  REGISTER(FixLengthApplier, "FixLengthApplier");
  REGISTER(EnumerateApplier, "EnumerateApplier");
  REGISTER(Distribute, "Distribute");
  REGISTER(SVMPredict, "SVMPredict");
  REGISTER(ExtractApplier, "ExtractApplier");
  REGISTER(RenameApplier, "RenameApplier");
}


DataSet* Applier::mapDataSet(const DataSet* dataset) const {
  MAPDATASET_PREPARE;

  MAPDATASET_LOOP_POINT;
  Point* newp = mapPoint(p);
  MAPDATASET_END_LOOP;
}

void Applier::checkLayout(const PointLayout& layout) const {
  if (layout != _transfo.layout) {
    throw GaiaException("Cannot apply transfo to DataSet/Point because its layout is different than the one which this transformation analyzed. Different descriptors: ", layout.symmetricDifferenceWith(_transfo.layout).join(", "));
  }
}

void Applier::checkLayout(const DataSet* dataset) const {
  checkLayout(dataset->layout());

  dataset->checkAllPointsShareSameLayout();
}

void Applier::transferHistory(const DataSet* original, DataSet* destination) const {
  destination->setHistoryNoCheck(original->history());
  destination->addTransformation(_transfo);
}


void Applier::addPointsNoLayoutCheck(DataSet* dataset, const QVector<Point*>& points) const {
  dataset->addPoints(points, false, false, false, true);
}



} // namespace gaia2
