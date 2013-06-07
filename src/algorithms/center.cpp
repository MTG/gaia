#include <cmath>
#include "center.h"
#include "utils.h"
#include "algoutils.h"

namespace gaia2 {

Center::Center(const ParameterMap& params) : Analyzer(params) {}


Transformation Center::analyze(const DataSet* dataset, const Region& descs) const {
  G_INFO("Doing center analysis...");
  checkDataSet(dataset);
  checkMinPoints(dataset, 2);
  Region region = checkFixedLength(descs.select(RealType), dataset->layout());

  QPair<Point, Point> meanVar = getRegionMeanVar(*dataset, region);
  const Point& mean = meanVar.first;
  const Point& var  = meanVar.second;

  Transformation result(dataset->layout());
  result.analyzerName = "center";
  result.analyzerParams = _params;
  result.applierName = "dotproduct";

  ParameterMap stats;

  foreach (const Segment& seg, region.segments) {
    // FIXME: not necessarily correct if some adjacent descriptors have been
    //        merged into a single segment
    QString descriptorName = mean.layout().descriptorName(RealType, FixedLength, seg.begin);
    ParameterMap meanvar;
    meanvar.insert("mean", mean.value(descriptorName));
    meanvar.insert("var", var.value(descriptorName));
    stats.insert(descriptorName, meanvar);
  }

  result.info = stats;

  ParameterMap& applierParams = result.params;
  applierParams.insert("applyClipping", _params.value("applyClipping", false));
  ParameterMap coeffMap;

  foreach (const Segment& seg, region.segments) {
    QString descriptorName = mean.layout().descriptorName(RealType, FixedLength, seg.begin);

    ParameterMap coeffs;
    coeffs.insert("a", RealDescriptor(seg.end - seg.begin, 1.0));
    coeffs.insert("b", -mean.value(descriptorName));

    coeffMap.insert(descriptorName, coeffs);
  }

  applierParams.insert("coeffs", coeffMap);

  return result;
}


} // namespace gaia2
