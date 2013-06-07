#include <iostream>
#include <cmath>
#include <QSet>
#include "cleaner.h"
#include "utils.h"
#include "algoutils.h"
using namespace std;

namespace gaia2 {

const float Cleaner::MIN_RANGE = 1e-7f;

Cleaner::Cleaner(const ParameterMap& params) : Analyzer(params) {}


  Transformation Cleaner::analyze(const DataSet* dataset, const Region& region) const {
  G_INFO("Doing cleaner analysis...");
  checkDataSet(dataset);

  const PointLayout& layout = dataset->layout();
  QMap<QString, int> isInf, isNan; // name -> number of points which comply
  QList<Segment> flsegments = region.select(RealType, FixedLength).segments;

  // clean fixed-length descriptors
  for (int n=0; n<dataset->size(); n++) {
    const Point* p = dataset->at(n);

    FORSEG(*p) {
      const RealDescriptor& fdata = p->frealData(nseg);

      foreach (const Segment& seg, flsegments) {
        for (int i=seg.begin; i<seg.end; i++) {

          Real x = fdata.at(i);

          if (isnan(x)) {
            QString name = layout.descriptorName(RealType, FixedLength, i);
            isNan[name] += 1;
            G_DEBUG(GAlgorithms, "ERROR:" << p->name() << " in dim " << name << " has NaN value");
            continue;
          }
          if (isinf(x)) {
            QString name = layout.descriptorName(RealType, FixedLength, i);
            isInf[name] += 1;
            G_DEBUG(GAlgorithms, "ERROR:" << p->name() << " in dim " << name << " has Inf value");
            continue;
          }
        }
      }
    }
  }

  // clean variable-length descriptors
  QVector<int> indices = region.listIndices(RealType, VariableLength);

  // we don't want to check for min & max on variable-length descriptors, but only
  // for isnan and isinf. So we require to apply the fixlength transfo before, so
  // as to make sure min & max are checked for all descriptors for which they can
  // be checked.
  if (!indices.isEmpty() && !dataset->history().contains("fixlength")) {
    throw GaiaException("The Cleaner transformation does not work anymore on variable-length descriptors. "
                        "Please apply Cleaner only after a FixLength transformation.");
  }

  for (int i=0; i<dataset->size(); i++) {
    const Point* p = dataset->at(i);

    FORSEG(*p) {
      const Array<RealDescriptor>& pdata = p->vrealData(nseg);
      foreach (int idx, indices) {

        const RealDescriptor& x = pdata[idx];

        if (x.isnan()) {
          QString name = layout.descriptorName(RealType, VariableLength, idx);
          isNan[name] += 1;
          G_ERROR("ERROR:" << p->name() << " in dim " << name << " has NaN value");
          continue;
        }
        if (x.isinf()) {
          QString name = layout.descriptorName(RealType, VariableLength, idx);
          isInf[name] += 1;
          G_ERROR("ERROR:" << p->name() << " in dim " << name << " has Inf value");
          continue;
        }
      }
    }
  }

  // list of descriptors to remove
  QSet<QString> toRemove;
  foreach (const QString& name, isInf.keys()) {
    G_INFO("Removing" << name << "because it has Inf values");
    toRemove.insert(name);
  }
  foreach (const QString& name, isNan.keys()) {
    G_INFO("Removing" << name << "because it has NaN values");
    toRemove.insert(name);
  }

  // also remove descriptors which are constant
  QPair<Point, Point> minmax = getRegionMinMax(*dataset, region.select(RealType, FixedLength));
  RealDescriptor& fmin = minmax.first.frealData();
  RealDescriptor& fmax = minmax.second.frealData();

  // for fixed-length descriptors, we need to make sure that all dimensions are
  // constant before removing them, otherwise we might do stuff that we don't
  // actually want to do, such as removing all matrices (because first & second
  // values are the dimension of the matrix, usually constant)
  QSet<QString> toRemoveCandidates;
  foreach (const Segment& seg, flsegments) {
    for (int i=seg.begin; i<seg.end; i++) {
      if ((fmax[i] - fmin[i]) < MIN_RANGE) {
        QString name = layout.descriptorName(RealType, FixedLength, i);
        toRemoveCandidates.insert(name);
      }
    }
  }
  // make sure that the candidates are actually constant on all dimensions
  foreach (const QString& name, toRemoveCandidates) {
    Segment seg = layout.descriptorLocation(name).segment();
    bool remove = true;
    for (int i=seg.begin; i<seg.end; i++) {
      if ((fmax[i] - fmin[i]) >= MIN_RANGE) {
        remove = false;
        break;
      }
    }
    if (remove) {
      G_INFO("Removing" << name << "because it is a constant descriptor");
      toRemove.insert(name);
    }
  }

  Transformation result(dataset->layout());
  result.analyzerName = "cleaner";
  result.analyzerParams = _params;
  result.applierName = "removedesc";

  QStringList toRemoveList;
  foreach (QString name, toRemove) toRemoveList.append(name);

  ParameterMap params;
  params.insert("descriptorNames", toRemoveList);

  result.params = params;

  return result;
}

} // namespace gaia2
