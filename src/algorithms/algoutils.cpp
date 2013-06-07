#include "algoutils.h"

namespace gaia2 {


QPair<Point, Point> getRegionMeanVar(const DataSet& dataset,
                                     const Region& region) {
  G_DEBUG(GAlgorithms, "getting mean/var");

  Region realRegion = region.select(RealType);
  realRegion.checkLengthTypeOnlyFrom(FixedLength, &dataset.layout());
  const QList<Segment>& flsegments = realRegion.segments;

  Point meanPoint, varPoint;
  PointLayout layoutCopy = dataset.layout().copy();

  meanPoint.setLayout(layoutCopy);
  varPoint.setLayout(layoutCopy);

  RealDescriptor& fmean = meanPoint.frealData();
  RealDescriptor& fvar  = varPoint.frealData();

  // initialization of variables
  foreach (const Segment& seg, flsegments) {
    for (int i=seg.begin; i<seg.end; i++) {
      fmean[i] = fvar[i] = 0;
    }
  }

  // first pass, get mean
  const int nPoints = dataset.size();
  const int nSegments = dataset.totalSegments();

  for (int i=0; i<nPoints; i++) {
    const Point* p = dataset.at(i);
    FORSEG(*p) {
      fmean += p->frealData(nseg);
    }
  }

  fmean /= nSegments;

  // second pass, get variance
  for (int i=0; i<nPoints; i++) {
    const Point* p = dataset.at(i);
    FORSEG(*p) {
      RealDescriptor fx(p->frealData(nseg));
      fx -= fmean;
      fvar += (fx *= fx);
    }
  }

  fvar /= (nSegments - 1);

  G_DEBUG(GAlgorithms, "done getting mean/var");

  return qMakePair(meanPoint, varPoint);
}



QPair<Point, Point> getRegionMinMax(const DataSet& dataset,
                                    const Region& region) {
  G_DEBUG(GAlgorithms, "getting min/max");

  Region realRegion = region.select(RealType);
  realRegion.checkLengthTypeOnlyFrom(FixedLength, &dataset.layout());
  const QList<Segment>& flsegments = realRegion.segments;

  Point minPoint, maxPoint;
  PointLayout layoutCopy = dataset.layout().copy();

  minPoint.setLayout(layoutCopy);
  maxPoint.setLayout(layoutCopy);

  RealDescriptor& fmin = minPoint.frealData();
  RealDescriptor& fmax = maxPoint.frealData();
  const Point* sp = dataset.samplePoint();

  // initialization of variables
  foreach (const Segment& seg, flsegments) {
    for (int i=seg.begin; i<seg.end; i++) {
      fmin[i] = fmax[i] = sp->frealData().at(i);
    }
  }

  // first pass, get mean
  const int nPoints = dataset.size();

  for (int i=0; i<nPoints; i++) {
    const Point* p = dataset.at(i);
    FORSEG(*p) {
      const RealDescriptor& fdata = p->frealData(nseg);
      fmin.minimum(fdata);
      fmax.maximum(fdata);
    }
  }

  G_DEBUG(GAlgorithms, "done getting min/max");

  return qMakePair(minPoint, maxPoint);
}


QStringList findVariableLengthDescriptors(const DataSet* dataset) {
  const Point* samplePoint = dataset->samplePoint();

  Array<int> realSize(samplePoint->vrealData().size(), 0);
  Array<int> stringSize(samplePoint->vstringData().size(), 0);
  Array<int> enumSize(samplePoint->venumData().size(), 0);

  Array<bool> realVarLength(realSize.size(), false);
  Array<bool> stringVarLength(stringSize.size(), false);
  Array<bool> enumVarLength(enumSize.size(), false);

  // initialization of variables
  G_DEBUG(GAlgorithms, "initializing sizes");
  for (int j=0; j<realSize.size(); j++) realSize[j] = samplePoint->vrealData()[j].size();
  for (int j=0; j<stringSize.size(); j++) stringSize[j] = samplePoint->vstringData()[j].size();
  for (int j=0; j<enumSize.size(); j++) enumSize[j] = samplePoint->venumData()[j].size();


  // check which have a different size
  G_DEBUG(GAlgorithms, "checking points sizes");

  for (int i=0; i<dataset->size(); i++) {
    const Point* p = dataset->at(i);

    FORSEG(*p) {
      for (int j=0; j<realSize.size(); j++) {
        if (p->vrealData(nseg)[j].size() != realSize[j]) realVarLength[j] = true;
      }

      for (int j=0; j<stringSize.size(); j++) {
        if (p->vstringData(nseg)[j].size() != stringSize[j]) stringVarLength[j] = true;
      }

      for (int j=0; j<enumSize.size(); j++) {
        if (p->venumData(nseg)[j].size() != enumSize[j]) enumVarLength[j] = true;
      }
    }
  }


  // list of variable-length descriptors
  QStringList varlength;

  for (int j=0; j<realVarLength.size(); j++) {
    if (realVarLength[j] || samplePoint->vrealData()[j].size() == 0)
      varlength.append(dataset->layout().descriptorName(RealType, VariableLength, j));
  }

  for (int j=0; j<stringVarLength.size(); j++) {
    if (stringVarLength[j] || samplePoint->vstringData()[j].size() == 0)
      varlength.append(dataset->layout().descriptorName(StringType, VariableLength, j));
  }

  for (int j=0; j<enumVarLength.size(); j++) {
    if (enumVarLength[j] || samplePoint->venumData()[j].size() == 0)
      varlength.append(dataset->layout().descriptorName(EnumType, VariableLength, j));
  }

  return varlength;
}

} // namespace gaia2
