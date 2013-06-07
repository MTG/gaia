#ifndef GAIA_ALGOUTILS_H
#define GAIA_ALGOUTILS_H

#include "dataset.h"

namespace gaia2 {


/**
 * Gets the mean and variance of the descriptors in the given region.
 * As those concepts only make sense for real numbers, the string descriptors
 * contained in this region will be silently ignored.
 * This function requires all descriptors in the region to be fixed-length,
 * otherwise it throws an exception.
 */
QPair<Point, Point> getRegionMeanVar(const DataSet& dataset,
                                     const Region& region);


/**
 * Gets the mean and variance of the descriptors in the given region.
 * As those concepts only make sense for real numbers, the string descriptors
 * contained in this region will be silently ignored.
 * This function requires all descriptors in the region to be fixed-length,
 * otherwise it throws an exception.
 */
QPair<Point, Point> getRegionMinMax(const DataSet& dataset,
                                    const Region& region);


/**
 * Compare the descriptor names using the following criteria:
 *  1. Fixed-Length come before Variable-Length
 *  2. within the same length type, descriptors are sorted alphabetically
 */
class DescCompare {
  const PointLayout& _layout;
 public:
  DescCompare(const PointLayout& layout) : _layout(layout) {}
  bool operator()(const QString& a, const QString& b) const {
    DescriptorLengthType la = _layout.descriptorLocation(a).lengthType();
    DescriptorLengthType lb = _layout.descriptorLocation(b).lengthType();

    if (la == FixedLength && lb == VariableLength) return true;
    if (la == VariableLength && lb == FixedLength) return false;
    return a < b;
  }
};


/**
 * For all variable-length descriptors in this DataSet, find all the ones which
 * actually are of different lengths, ie: those such that there exists 2 points
 * p1 and p2 such as len(p1.desc) != len(p2.desc).
 *
 * If there is a descriptor which has len == 0 for all points, then this
 * descriptor is considered variable-length as well. The rationale is the
 * following: if they're all empty, it only makes sense if they're actually
 * lists of values which (by chance) happen to be all empty, but it can in no
 * case be a fixed-length descriptor, because that would be completely stupid
 * to have such an empty descriptor.
 */
QStringList findVariableLengthDescriptors(const DataSet* dataset);

} // namespace gaia2

#endif // GAIA_ALGOUTILS_H
