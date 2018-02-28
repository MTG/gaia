/* 
 * Copyright (C) 2006-2013  Music Technology Group - Universitat Pompeu Fabra
 *
 * This file is part of Gaia
 * 
 * Gaia is free software: you can redistribute it and/or modify it under 
 * the terms of the GNU Affero General Public License as published by the Free 
 * Software Foundation (FSF), either version 3 of the License, or (at your 
 * option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT 
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more 
 * details.
 * 
 * You should have received a copy of the Affero GNU General Public License
 * version 3 along with this program.  If not, see http://www.gnu.org/licenses/
 */

#ifndef GAIA_UTILS_H
#define GAIA_UTILS_H

#include "dataset.h"
#include "point.h"
#include "pointlayout.h"
#include "3rdparty/Eigen/Core"


// This file contains some useful functions that were not generic enough to go
// inside a class, and that benefit a lot of other classes (most of them are related
// to dataset management)

namespace gaia2 {

/**
 * Utility function that merges a sequence of node names into a single
 * fully-qualified name.
 */
inline QString mergeName(const QStringList& name) {
  return name.join(QString(NAME_SEPARATOR));
}

/**
 * Splits a fully-qualified name into its consecutive node names.
 */
inline QStringList splitName(const QString& name, bool includeRootNode = false) {
  if (includeRootNode) return name.split(NAME_SEPARATOR);
  return name.split(NAME_SEPARATOR, QString::SkipEmptyParts);
}


/**
 * Parse a string representing a descriptor + optional dimension as a QPair<QString, int>.
 * In case the dimension is not specified, 0 will be returned.
 */
QPair<QString, int> parseDimension(const QString& dimension);


/**
 * Returns normally if both dataset are isomorph, meaning that they can be linked
 * together, merged together, etc... In particular, they need to have exactly
 * the same points and the same original layout (we don't care about the
 * transformation history).
 * In the case they aren't, throw an exception with an error message.
 * In case any of the 2 datasets is the null pointer, this doesn't throw an
 * exception.
 */
void checkIsomorphDataSets(const DataSet* dataset1, const DataSet* dataset2,
                           bool checkOriginalLayout = true);


/**
 * This function returns the region of descriptors from a layout that match the
 * patterns given by the select argument but that do not match the ones given
 * by the exclude argument.
 */
QStringList selectDescriptors(const PointLayout& layout,
                              DescriptorType type = UndefinedType,
                              const QStringList& select = QStringList("*"),
                              const QStringList& exclude = QStringList(),
                              bool failOnUnmatched = true);


typedef QList<QPair<Segment, Segment> > IndexMap;

/**
 * Creates mappings from indices in the NEW layout to indices in the OLD layout.
 */
void createIndexMappings(const PointLayout& oldLayout,
                         PointLayout& newLayout,
                         const Region& region, IndexMap& realMap,
                         IndexMap& stringMap, IndexMap& enumMap);


/**
 * Given a set of mappings, transfers the data from a Point to a new one.
 */
void transferPointData(const Point* p, Point* result,
                       const IndexMap& realMap, const IndexMap& stringMap,
                       const IndexMap& enumMap);

/**
 * Creates a new Point that is a mapping of the given one, copying only those
 * descriptors that are in newLayout. The Index maps should be precalculated
 * with the createIndexMappings function.
 */
Point* mapPoint(const Point* p, const PointLayout& newLayout,
                const IndexMap& realMap, const IndexMap& stringMap,
                const IndexMap& enumMap);

/**
 * Merges the descriptors contained into the given region into a single one.
 */
RealDescriptor mergeDescriptors(const Point* p, int nseg, const Region& region);


/**
 * Merges two PointLayouts together, provided that they don't overlap.
 */
PointLayout mergeLayouts(const PointLayout& layout1, const PointLayout& layout2);

/**
 * Merges two points together, provided that their layout don't overlap, and
 * return the resulting point.
 */
Point* mergePoints(const Point* ds1, const Point* ds2);

/**
 * Merges two datasets together, provided that their layout don't overlap, and
 * return the resulting dataset.
 */
DataSet* mergeDataSets(const DataSet* ds1, const DataSet* ds2);


/**
 * Given a DataSet (for reference) and a Region, returns the mapping between
 * the region indices and the names of the corresponding descriptors.
 * This function will throw if the Region is not composed of a single type of
 * descriptors.
 */
QMap<int, QString> mapRegionIndicesToNames(const Region& region,
                                           const DataSet* dataset);

/**
 * Given a list of values and a map from their indices to their respective names,
 * returns a list of pairs containing the participation of each original dimension
 * in the target space, expressed as a percentage of the total (meaning the sum of
 * the participation is equal to 100).
 */
QList<QPair<double, QString> > dimensionParticipation(const Eigen::VectorXd& values,
                                                      const QMap<int, QString>& dnames);

/**
 * Dimension boost after applying the rotation/scaling defined by the given base vectors.
 * Useful in particular in RCA (where the vectors are not an orthonormal base).
 */
QList<QPair<double, QString> > dimensionBoost(const Eigen::MatrixXf& featureVector,
                                              const Region& region,
                                              const DataSet* dataset);

/**
 * Formats the results of the dimensionParticipation function into a nicely
 * printable string.
 * If ratio == true, prints "x 1.23" instead of "1.23%".
 */
QString formatDimensionParticipation(const QList<QPair<double, QString> >& participation,
                                     bool ratio = false);


/**
 * Converts a list of dimensions names in the format @c descriptorName[dim] to
 * a map of descriptor name to list of integers representing the dimensions
 * selected for each one.
 */
QMap<QString, QList<int> > dimensionListToMapping(const QStringList& dims);

/**
 * Sets the version of the QDataStream to the current version of Gaia.
 * This function @b must be called before serializing anything in the stream.
 */
void setDataStreamVersion(QDataStream& out);

/**
 * Checks that the input QDataStream is a valid one (ie: supported by our version
 * of Gaia) and sets its corresponding type so that it's ready to be read.
 */
void checkValidDataStream(QDataStream& in);

enum BinaryFileFormat {
  Gaia_2_0 = 100,
  Gaia_2_1 = 101,
#ifndef GAIA_QT5
  Gaia_2_3 = 102,
  Gaia_2_4 = 103
#else
  Gaia_2_3 = 102
#endif
};

/**
 * Returns the Gaia version of this stream.
 */
int gaiaVersion(const QDataStream& stream);

/**
 * Returns the memory currently used by this process, in KBs.
 * mtype can be either one of:
 *  - VmSize: total virtual memory
 *  - VmRSS: resident segment size
 *
 * NB: Only works on linux at the moment, will return 0 on other platforms.
 */
int memoryUsage(QString mtype = "VmSize");

} // namespace gaia2

#endif // GAIA_UTILS_H
