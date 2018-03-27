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

/* <copyright entity="UPF">
# UPF. All Right Reserved, http://www.upf.edu/
#
# This source is subject to the Contributor License Agreement of the Essentia project.
# Please see the CLA.txt file available at http://essentia.upf.edu/contribute/
# for more
# information.
#
# THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
# KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
# PARTICULAR PURPOSE.
#
# </copyright>
*/

#include <QRegExp>
#include <QString>
#include <cmath>
#include <algorithm>
#include <unistd.h>
#include "utils.h"
using std::abs;
using std::reverse;


namespace gaia2 {


QPair<QString, int> parseDimension(const QString& dimension) {
  int sbopen = dimension.indexOf('[');
  int sbclose = dimension.indexOf(']');

  if (sbopen == -1 || sbclose == -1 || sbclose < sbopen) return qMakePair(dimension, 0);

  QString descname = dimension.left(sbopen);
  int dim = dimension.mid(sbopen+1, sbclose-sbopen-1).toInt();

  return qMakePair(descname, dim);
}


void checkIsomorphDataSets(const DataSet* dataset1, const DataSet* dataset2,
                           bool checkOriginalLayout) {
  if (!dataset1 || !dataset2) {
    throw GaiaException("One of the two datasets is null (the null pointer)");
  }

  // DataSets need to have the same original layout, otherwise the Merge
  // transformation (in particular) cannot work
  if (checkOriginalLayout &&
      (dataset1->originalLayout() != dataset2->originalLayout())) {
    throw GaiaException("DataSets do not have the same original layout");
  }

  int size = dataset1->size();

  if (dataset2->size() != size) {
    QStringList msg;
    msg << "DataSets do not have the same number of points: "
        << "current = " << QString::number(size)
        << "; specified = " << QString::number(dataset2->size());
    throw GaiaException(msg);
  }


  for (int i=0; i<size; i++) {
    const Point* p1 = dataset1->at(i);
    const Point* p2 = dataset2->at(i);

    if (p1->name() != p2->name()) {
      QStringList msg;
      msg << "Point with id \"" << p1->name() << "\" in first dataset "
          << "could not be found in 2nd dataset";
      throw GaiaException(msg);
    }

    if (p1->numberSegments() != p2->numberSegments()) {
      QStringList msg;
      msg << "Point with id \"" << p1->name() << "doesn't have the same "
          << "number of segments in both datasets";
      throw GaiaException(msg);
    }
  }
}


QStringList selectDescriptors(const PointLayout& layout,
                              DescriptorType type,
                              const QStringList& select,
                              const QStringList& exclude,
                              bool failOnUnmatched) {

  QStringList selected = layout.descriptorNames(type, select, failOnUnmatched);
  QStringList excluded = layout.descriptorNames(type, exclude, failOnUnmatched);

  // substract one from the other to get final list of desired descriptors
  foreach (QString name, excluded) {
    selected.removeAll(name);
  }

  return selected;
}

// macro to avoid copy pasting too much in the next function
#define CREATE_VL_MAPPINGS(type, indexMap)                                 \
  indices = region.listIndices(type, VariableLength);                      \
  foreach (int idx, indices) {                                             \
    QString name = newLayout.descriptorName(type, VariableLength, idx);    \
    Segment  newLocation = newLayout.descriptorLocation(name).segment();   \
    Segment origLocation = oldLayout.descriptorLocation(name).segment();   \
    indexMap << qMakePair(newLocation, origLocation);                      \
  }

// macro to avoid copy pasting too much in the next function
#define CREATE_FL_MAPPINGS(type, indexMap)                                  \
  indices = region.listIndices(type, FixedLength);                          \
  for (int i=0; i<indices.size(); i++) {                                    \
    QString name = newLayout.descriptorName(type, FixedLength, indices[i]); \
    Segment  newLocation = newLayout.descriptorLocation(name).segment();    \
    Segment origLocation = oldLayout.descriptorLocation(name).segment();    \
    indexMap << qMakePair(newLocation, origLocation);                       \
    /* only insert this descriptor once */                                  \
    Q_ASSERT(indices[i] == newLocation.begin);                              \
    i += newLocation.size() - 1;                                            \
  }


void createIndexMappings(const PointLayout& oldLayout,
                         PointLayout& newLayout,
                         const Region& region, IndexMap& realMap,
                         IndexMap& stringMap, IndexMap& enumMap) {
  // we can't just iterate over the segments in the region, because we
  // don't know for sure if some segments have been previously merged
  // or not, so we have to loop through all the indices and then get
  // the descriptors from there.

  QVector<int> indices;

  CREATE_VL_MAPPINGS(RealType, realMap);
  CREATE_FL_MAPPINGS(RealType, realMap);

  CREATE_VL_MAPPINGS(StringType, stringMap);
  CREATE_FL_MAPPINGS(StringType, stringMap);

  CREATE_VL_MAPPINGS(EnumType, enumMap);
  CREATE_FL_MAPPINGS(EnumType, enumMap);

}

// macro to avoid copy pasting too much in the next function
// there are 4 possible mapping types for each DescriptorType:
// VarLen -> VarLen (old one), VarLen -> FixLen, FixLen -> FixLen and
// FixLen -> VarLen (probably the less used)
#define TRANSFER_DATA(type, ctype, nseg)                                     \
    const Segment& seg = type##segp.first;                              \
    const Segment& origSeg = type##segp.second;                         \
    if (origSeg.ltype == VariableLength) {                              \
      if (seg.ltype == VariableLength) {                                \
        result->v##type##Data(nseg)[seg.begin] = p->v##type##Data(nseg)[origSeg.begin]; \
      }                                                                 \
      else { /* seg.ltype == FixedLength */                             \
        const ctype##Descriptor& origDesc = p->v##type##Data(nseg)[origSeg.begin]; \
        if (origDesc.size() != seg.size()) {                            \
          QStringList msg;                                              \
          msg << "Trying to map a variable-length descriptor of size "  \
              << QString::number(origDesc.size())                       \
              << " into a fixed-length space of size "                  \
              << QString::number(seg.size()) << " ("                    \
              << p->layout().descriptorName(ctype##Type, VariableLength, origSeg.begin) \
              << ")";                                                   \
          throw GaiaException(msg);                                     \
        }                                                               \
        for (int i=0; i<origDesc.size(); i++) {                         \
          result->f##type##Data(nseg)[seg.begin + i] = origDesc[i];     \
        }                                                               \
      }                                                                 \
    }                                                                   \
  else { /* origSeg.ltype == FixedLength */                             \
      if (seg.ltype == VariableLength) {                                \
        ctype##Descriptor newDesc; newDesc.resize(origSeg.size());      \
        for (int i=0; i<origSeg.size(); i++) {                          \
          newDesc[i] = p->f##type##Data(nseg)[origSeg.begin + i];       \
        }                                                               \
        result->v##type##Data(nseg)[seg.begin] = newDesc;               \
      }                                                                 \
      else { /* seg.ltype == FixedLength */                             \
        if (seg.size() != origSeg.size()) {                             \
          QStringList msg;                                              \
          msg << "Trying to map a fixed-length descriptor of size "     \
              << QString::number(origSeg.size())                        \
              << " into a fixed-length space of size "                  \
              << QString::number(seg.size()) << " ("                    \
              << p->layout().descriptorName(ctype##Type, FixedLength, origSeg.begin) \
              << ")";                                                   \
          throw GaiaException(msg);                                     \
        }                                                               \
        for (int i=0; i<seg.size(); i++) {                              \
          result->f##type##Data(nseg)[seg.begin + i] = p->f##type##Data(nseg)[origSeg.begin + i]; \
        }                                                               \
      }                                                                 \
    }


void transferPointData(const Point* p, Point* result,
                       const IndexMap& realMap, const IndexMap& stringMap, const IndexMap& enumMap) {

  Q_ASSERT(p->numberSegments() == result->numberSegments());

  QPair<Segment, Segment> realsegp;
  foreach (realsegp, realMap) {
    FORSEG(*p) {
      TRANSFER_DATA(real, Real, nseg);
    }
  }

  QPair<Segment, Segment> stringsegp;
  foreach (stringsegp, stringMap) {
    FORSEG(*p) {
      TRANSFER_DATA(string, String, nseg);
    }
  }

  // do not transfer enums in the same way: enums can either be strings
  // which want to be promoted to enum, or as before, transferring an enum to
  // an other one
  QPair<Segment, Segment> enumsegp;

  foreach (enumsegp, enumMap) {
    const Segment& seg = enumsegp.first;
    const Segment& origSeg = enumsegp.second;
    const PointLayout& origLayout = p->layout();
    const PointLayout& targetLayout = result->layout();
    QString name = targetLayout.descriptorName(EnumType, seg.ltype, seg.begin);

    // check which type of mapping we are doing for enums:
    // - enum to enum: simple copy, such as in select or remove transformation
    // - string to enum: we are enumerating a string descriptor

    // enum to enum
    if (origSeg.type == EnumType && seg.type == EnumType) {

      FORSEG(*p) {
        TRANSFER_DATA(enum, Enum, nseg);
      }

      if (!origLayout.sameEnumMap(targetLayout, name)) {
        // enum maps are not equal, we need to convert the values we have from
        // the old enum map to the new one
        FORSEG(*p) {
          if (origSeg.ltype == FixedLength) {
            StringDescriptor str = origLayout.enumToString(name, EnumDescriptor(&p->fenumData(nseg)[origSeg.begin], origSeg.size()));
            EnumDescriptor enumd = targetLayout.stringToEnum(name, str);

            if (seg.ltype == FixedLength) {
              Q_ASSERT(seg.size() == enumd.size());
              for (int i=0; i<seg.size(); i++) {
                result->fenumData(nseg)[seg.begin + i] = enumd[i];
              }
            }
            else { // seg.type == VariableLength
              result->venumData(nseg)[seg.begin] = enumd;
            }
          }
          else { // origSeg.ltype == VariableLength
            StringDescriptor str = origLayout.enumToString(name, p->venumData(nseg)[origSeg.begin]);
            EnumDescriptor enumd = targetLayout.stringToEnum(name, str);

            if (seg.ltype == FixedLength) {
              Q_ASSERT(seg.size() == enumd.size());
              for (int i=0; i<seg.size(); i++) {
                result->fenumData(nseg)[seg.begin + i] = enumd[i];
              }
            }
            else { // seg.type == VariableLength
              result->venumData(nseg)[seg.begin] = enumd;
            }
          }
        }
      }
    }
    // string to enum
    else if (origSeg.type == StringType && seg.type == EnumType && origSeg.ltype == seg.ltype) {

      FORSEG(*p) {

        if (seg.ltype == FixedLength) {
          StringDescriptor str = StringDescriptor(&p->fstringData(nseg)[origSeg.begin], origSeg.size());
          EnumDescriptor enumd = targetLayout.stringToEnum(name, str);
          for (int i=0; i<seg.size(); i++) {
            result->fenumData(nseg)[seg.begin + i] = enumd[i];
          }
        }
        else { // seg.type == VariableLength
          result->venumData(nseg)[seg.begin] = targetLayout.stringToEnum(name, p->vstringData(nseg)[origSeg.begin]);
        }

      }
    }
    // unknown mapping
    else {
      throw GaiaException("You can only map String to Enum at the moment.");
    }
  }
}


Point* mapPoint(const Point* p, const PointLayout& newLayout,
                const IndexMap& realMap, const IndexMap& stringMap, const IndexMap& enumMap) {
  Point* result = new Point;
  result->setName(p->name());
  result->setLayout(newLayout, p->numberSegments());

  transferPointData(p, result, realMap, stringMap, enumMap);

  return result;
}


RealDescriptor mergeDescriptors(const Point* p, int nseg, const Region& region) {
  int size = region.dimension(region.type(), p);
  RealDescriptor result(size, Real(0.0));
  int dim = 0;

  const RealDescriptor& fdata = p->frealData(nseg);
  QList<Segment> segfl = region.select(RealType, FixedLength).segments;
  foreach (const Segment& seg, segfl) {
    for (int i=0; i<seg.size(); i++) {
      result[dim] = fdata[seg.begin + i];
      dim++;
    }
  }

  const Array<RealDescriptor>& vdata = p->vrealData(nseg);
  QVector<int> listIndices = region.listIndices(RealType, VariableLength);
  foreach (int idx, listIndices) {
    for (int i=0; i<vdata[idx].size(); i++) {
      result[dim] = vdata[idx][i];
      dim++;
    }
  }

  Q_ASSERT(dim == size);

  return result;
}


PointLayout mergeLayouts(const PointLayout& layout1, const PointLayout& layout2) {
  // is someone here trying to do useless shit? ;^)
  if (layout1.isEmpty()) return layout2.copy();
  if (layout2.isEmpty()) return layout1.copy();

  // first check that we don't overlap
  QSet<QString> d1 = layout1.descriptorNames().toSet();
  QSet<QString> d2 = layout2.descriptorNames().toSet();
  if (!d1.intersect(d2).isEmpty()) {
    QStringList msg("Cannot merge layouts because they have these descriptors in common:");
    foreach (QString desc, d1) msg << " " << desc;
    throw GaiaException(msg);
  }

  PointLayout newLayout = layout1.copy();

  // foreach descriptor in layout2, add it to newLayout, by adding each part
  // of its branch each time. This won't cause any problem because adding
  // a descriptor that already exists returns silently
  foreach (QString name, layout2.descriptorNames()) {
    QStringList sname = splitName(name, true);
    int ssize = sname.size();
    Q_ASSERT(ssize >= 2);
    for (int i=1; i<ssize-1; i++) {
      newLayout.addNoUpdate(sname[i-1], sname[i]);
    }
    Segment leafDescriptor = layout2.descriptorLocation(name).segment();
    QString leafName = sname.last();
    sname.removeLast();
    newLayout.add(sname.join("."), leafName,
                  leafDescriptor.type, leafDescriptor.ltype, leafDescriptor.size());
  }

  return newLayout;
}


Point* mergePoints(const Point* p1, const Point* p2) {
  if (p1->name() != p2->name() ||
      p1->numberSegments() != p2->numberSegments()) {
    throw GaiaException("Can only merge 2 points which have the same name and same number of segments");
  }

  const PointLayout& layout1 = p1->layout();
  const PointLayout& layout2 = p2->layout();

  PointLayout newLayout = mergeLayouts(layout1, layout2);

  QStringList names1 = layout1.descriptorNames();
  QStringList names2 = layout2.descriptorNames();

  Region region1 = newLayout.descriptorLocation(names1);
  Region region2 = newLayout.descriptorLocation(names2);

  IndexMap realMap1, realMap2, stringMap1, stringMap2, enumMap1, enumMap2;

  createIndexMappings(layout1, newLayout, region1, realMap1, stringMap1, enumMap1);
  createIndexMappings(layout2, newLayout, region2, realMap2, stringMap2, enumMap2);

  Point* p = new Point;
  p->setName(p1->name());
  p->setLayout(newLayout);

  transferPointData(p1, p, realMap1, stringMap1, enumMap1);
  transferPointData(p2, p, realMap2, stringMap2, enumMap2);

  return p;
}


DataSet* mergeDataSets(const DataSet* ds1, const DataSet* ds2) {
  checkIsomorphDataSets(ds1, ds2);
  G_DEBUG(GAlgorithms, "Datasets are isomorph");

  const PointLayout& layout1 = ds1->layout();
  const PointLayout& layout2 = ds2->layout();

  PointLayout newLayout = mergeLayouts(layout1, layout2);
  G_DEBUG(GAlgorithms, "Layouts merged");

  QStringList names1 = layout1.descriptorNames();
  QStringList names2 = layout2.descriptorNames();

  Region region1 = newLayout.descriptorLocation(names1);
  Region region2 = newLayout.descriptorLocation(names2);

  IndexMap realMap1, realMap2, stringMap1, stringMap2, enumMap1, enumMap2;

  createIndexMappings(layout1, newLayout, region1, realMap1, stringMap1, enumMap1);
  createIndexMappings(layout2, newLayout, region2, realMap2, stringMap2, enumMap2);

  DataSet* result = new DataSet;
  QVector<Point*> v(ds1->size());

  for (int i=0; i<ds1->size(); i++) {
    Point* p = new Point;
    const Point* p1 = ds1->at(i);
    const Point* p2 = ds2->at(i);
    p->setName(p1->name());
    p->setLayout(newLayout, p1->numberSegments());

    transferPointData(p1, p, realMap1, stringMap1, enumMap1);
    transferPointData(p2, p, realMap2, stringMap2, enumMap2);

    v[i] = p;
  }

  // no need to clean up v afterwards as we yield ownership to dataset
  result->addPoints(v, false, true, false, true, false);

  G_DEBUG(GAlgorithms, "Datasets merged, appending combined history");

  // We also need to add the Merge transfo to the history of this new DataSet,
  // so that we are able to add new points to it
  Transformation mergeTransfo;
  mergeTransfo.applierName = "merge";
  mergeTransfo.layout = ds1->originalLayout();
  ParameterMap params;
  params.insert("history1", Parameter(ds1->history()));
  params.insert("history2", Parameter(ds2->history()));
  mergeTransfo.params = params;

  TransfoChain history;
  history.append(mergeTransfo);

  result->setHistoryNoCheck(history);
  G_DEBUG(GAlgorithms, "Datasets successfully merged!");

  return result;
}



QMap<int, QString> mapRegionIndicesToNames(const Region& region,
                                           const DataSet* dataset) {
  QMap<int, QString> dnames;
  DescriptorType type = region.type();

  if (type != RealType) {
    throw GaiaException("mapRegionIndicesToNames only implemented for RealDescriptors at the moment.");
  }

  const Point* samplePoint = dataset->samplePoint();
  int dim = 0;

  Region flregion = region.select(type, FixedLength);
  foreach (const Segment& seg, flregion.segments) {
    QString basename = dataset->layout().descriptorName(type, FixedLength, seg.begin);
    if (seg.size() == 1) {
      dnames.insert(dim, basename);
      dim++;
    }
    else {
      for (int i=0; i<seg.size(); i++) {
        dnames.insert(dim, basename + '[' + QString::number(i) + ']');
        dim++;
      }
    }
  }

  QVector<int> listIndices = region.listIndices(type, VariableLength);

  foreach (int idx, listIndices) {
    QString basename = dataset->layout().descriptorName(type, VariableLength, idx);

    const RealDescriptor& desc = samplePoint->vrealData()[idx];
    if (desc.size() == 1) {
      dnames.insert(dim, basename);
      dim++;
    }
    else {
      for (int i=0; i<desc.size(); i++) {
        dnames.insert(dim, basename + '[' + QString::number(i) + ']');
        dim++;
      }
    }
  }

  return dnames;
}


QList<QPair<double, QString> > dimensionParticipation(const Eigen::VectorXd& values,
                                                      const QMap<int, QString>& dnames) {

  QList<QPair<double, QString> > participation;
  double total = 0.0;

  for (int i=0; i<values.size(); i++) {
    double p = values[i];
    QString dname = dnames.value(i);
    total += p;
    // if dname is not in our map, we don't want to consider it, just ignore it
    if (dname.isEmpty()) continue;
    participation.append(qMakePair(p, dnames[i]));
  }

  // normalize values so they represent a percentage of the total
  for (int i=0; i<participation.size(); i++) {
    participation[i].first *= (100 / total);
  }

  // return them sorted by descending order of participation
  sort(range(participation));
  reverse(range(participation));

  return participation;
}


QList<QPair<double, QString> > dimensionBoost(const Eigen::MatrixXf& featureVector,
                                              const Region& region,
                                              const DataSet* dataset) {

  int originalDimension = featureVector.cols();
  QMap<int, QString> dnames = mapRegionIndicesToNames(region, dataset);

  // pairs of (participation, dimensionName)
  QList<QPair<double, QString> > boost;

  // compute boost of each dimension
  bool sumSquared = true;

  for (int i=0; i<originalDimension; i++) {
    double p = 0;

    if (sumSquared) {
      p = featureVector.col(i).norm();
    }
    else { // sum absolute
      p = featureVector.col(i).cwiseAbs().sum();
    }

    boost.append(qMakePair(p, dnames[i]));
  }

  // return them sorted by descending order of boost value
  sort(range(boost));
  reverse(range(boost));

  return boost;
}

QString formatDimensionParticipation(const QList<QPair<double, QString> >& participation, bool ratio) {
  int maxStrSize = 0;
  for (int i=0; i<participation.size(); i++) {
    maxStrSize = qMax(maxStrSize, participation[i].second.size());
  }

  QString fmt = "  %1 : %2%";
  if (ratio) fmt = "  %1 : x %2";

  QStringList result;
  for (int i=0; i<participation.size(); i++) {
    const QPair<double, QString>& p = participation[i];
    result << fmt
      .arg(p.second + QString(maxStrSize-p.second.size(), ' '))
      .arg(p.first);
  }

  return result.join("\n");
}


QMap<QString, QList<int> > dimensionListToMapping(const QStringList& dims) {
  QMap<QString, QList<int> > result;
  QRegExp rexp("^(.*)\\[(\\d*)\\]$");

  foreach (QString dim, dims) {
    int pos = rexp.indexIn(dim);
    if (pos > -1) {
      result[rexp.cap(1)].append(rexp.cap(2).toInt());
    }
    else {
      throw GaiaException("Invalid dimension name: ", dim);
    }
  }
  return result;
}



int gaiaVersion(const QDataStream& stream) {
  int qtVersion = stream.version();
  if (qtVersion == QDataStream::Qt_4_2) return Gaia_2_0;
  if (qtVersion == QDataStream::Qt_4_3) return Gaia_2_1;
  if (qtVersion == QDataStream::Qt_4_4) return Gaia_2_3;
#ifdef GAIA_QT5
  if (qtVersion == QDataStream::Qt_5_6) return Gaia_2_4;
#endif
  throw GaiaException("Unknown stream format");
}

#ifndef GAIA_QT5
void setDataStreamVersion(QDataStream& out) {
  // write version (of the data format, not gaia). Currently: Gaia_2_3 (102)
  // also write Qt's binary data format
  // NB: the following comment is outdated, but you get the idea :-)
  // IMPORTANT NOTE: at the moment, there are 2 versions of the file format:
  // 100 for gaia 2.0.x versions, and 101 for gaia 2.1.x versions. To be able to
  // read both of those, we need to find a way to encode this version number
  // inside the DataStream. The solution at the moment is to use 100 only with
  // Qt stream version 4.2, and 101 only with qt 4.3. This will work fine as long
  // as there are as many qt versions as gaia data format versions. However, in
  // the other case, the correct solution would be to derive from QDataStream and
  // add a setGaiaVersion() and gaiaVersion() method to it.
  out << (qint32)Gaia_2_3;
  out.setVersion(QDataStream::Qt_4_4);
}
#else
void setDataStreamVersion(QDataStream& out) {
  // write version (of the data format, not gaia). Currently: Gaia_2_3 (102)
  // also write Qt's binary data format
  // NB: the following comment is outdated, but you get the idea :-)
  // IMPORTANT NOTE: at the moment, there are 2 versions of the file format:
  // 100 for gaia 2.0.x versions, and 101 for gaia 2.1.x versions. To be able to
  // read both of those, we need to find a way to encode this version number
  // inside the DataStream. The solution at the moment is to use 100 only with
  // Qt stream version 4.2, and 101 only with qt 4.3. This will work fine as long
  // as there are as many qt versions as gaia data format versions. However, in
  // the other case, the correct solution would be to derive from QDataStream and
  // add a setGaiaVersion() and gaiaVersion() method to it.
  out << (qint32)Gaia_2_4;
  out.setVersion(QDataStream::Qt_5_6);
}
#endif


void checkValidDataStream(QDataStream& in) {
  // read version
  qint32 version;
  in >> version;

  switch (version) {

  case Gaia_2_0:
    in.setVersion(QDataStream::Qt_4_2);
    G_DEBUG(GIO, "File has gaia version 100 (gaia 2.0.x)");
    break;

  case Gaia_2_1:
    in.setVersion(QDataStream::Qt_4_3);
    G_DEBUG(GIO, "File has gaia version 101 (gaia 2.1.x && 2.2.x)");
    break;

  case Gaia_2_3:
    in.setVersion(QDataStream::Qt_4_4);
    G_DEBUG(GIO, "File has gaia version 102 (gaia 2.3.x)");
    break;
#ifdef GAIA_QT5
  case Gaia_2_4:
    in.setVersion(QDataStream::Qt_5_6);
    G_DEBUG(GIO, "File has gaia version 103 (gaia 2.4.x)");
    break;
#endif

  default:
    QStringList msg;
    msg << "Unsupported version of the datafile format. File has version "
        << QString::number(version)
        << " but this version of Gaia only supports version 100 (Gaia 2.0.x), version 101 (Gaia 2.1.x and 2.2.x), and version 102 (Gaia 2.3.x)";
    throw GaiaException(msg);

  }

}

int memoryUsage(QString mtype) {
#ifdef OS_LINUX
  // NB: seems like getrusage doesn't work correctly, so here's the DIY way

  QString statfile = QString("/proc/%1/status").arg(getpid());

  FILE* f = fopen(statfile.toLatin1().data(), "r");
  char buf[4096];
  buf[fread(buf, 1, 4096, f)] = '\0';
  fclose(f);

  QString infostr = buf;
  QStringList info = infostr.split('\n');

  foreach (const QString& s, info) {
    QStringList sinfo = s.split(" ", QString::SkipEmptyParts);
    if (sinfo[0].startsWith(mtype)) return sinfo[1].toInt();
  }

  throw GaiaException("Invalid type of memory asked: ", mtype);

#else
  Q_UNUSED(mtype);
  return 0;
#endif
}


} // namespace gaia2
