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
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see http://www.gnu.org/licenses/
 */
#ifndef GAIA_REGION_H
#define GAIA_REGION_H

#include <QString>
#include <QList>
#include <QDataStream>
#include "gaia.h"

namespace gaia2 {

class Point;
class PointLayout;

/**
 * The possible types of descriptors accepted.
 */
enum DescriptorType {
  UndefinedType,
  RealType,
  StringType,
  EnumType
};

/**
 * Either fixed-length or variable-length.
 */
enum DescriptorLengthType {
  FixedLength,
  VariableLength
};

inline QString typeToString(DescriptorType type) {
  switch (type) {
  case UndefinedType: return "Undefined";
  case RealType: return "Real";
  case StringType: return "String";
  case EnumType: return "Enum";
  }
  return QString("Error in type conversion");
}

inline QString lengthTypeToString(DescriptorLengthType ltype) {
  switch (ltype) {
  case FixedLength: return "fixed-length";
  case VariableLength: return "variable-length";
  }
  return QString("Error in length type conversion");
}

/**
 * A segment is a physical contiguous location of descriptors of the same type
 * in the point layout. It has an additional property which is whether the
 * segment is to be taken in the fixed-length or variable-length part of the
 * data (for memory optimization reasons).
 */
class Segment {
 public:
  QString name;
  DescriptorType type;
  DescriptorLengthType ltype;
  int begin;
  int end;

  Segment() : name(""), type(UndefinedType), ltype(VariableLength), begin(-1), end(-1) {}

  Segment(QString name, DescriptorType type) :
    name(name), type(type), ltype(VariableLength),
    begin(-1), end(-1) {}

  Segment(QString name, DescriptorType type, DescriptorLengthType ltype,
          int begin, int end) :
    name(name), type(type), ltype(ltype), begin(begin), end(end) {}

  bool operator==(const Segment& rhs) const {
    return (type == rhs.type) && (begin == rhs.begin) && (ltype == rhs.ltype) &&
           (end == rhs.end);
  }

  bool operator<(const Segment& rhs) const {
    return ((this->type < rhs.type) ||
            ((this->type == rhs.type) && (this->ltype < rhs.ltype)) ||
            ((this->type == rhs.type) && (this->ltype == rhs.ltype) && (this->begin < rhs.begin)) ||
            ((this->type == rhs.type) && (this->ltype == rhs.ltype) && (this->begin == rhs.begin) && (this->end < rhs.end)));
  }

  inline int size() const {
    return end - begin;
  }

  QString toString() const;

  /**
   * Defines a default undefined Segment.
   */
  static Segment undefined;

  friend QDataStream& operator<<(QDataStream& out, const Segment& segment);
  friend QDataStream& operator>>(QDataStream& in, Segment& segment);
};


/**
 * A region is a physical location in the point layout which consists in a list
 * of segments. It does not require all the segments to be of the same type.
 */
class Region {
 public:
  QString name;
  QList<Segment> segments;

  /**
   * Merges another Region with this one.
   */
  Region& merge(const Region& r);

  /**
   * Returns the descriptor type if all segments within this region are of the
   * same type.
   * @throws GaiaException if region is empty or if there are at least
   * 2 segments of different types in this region.
   */
  DescriptorType type() const;

  /**
   * Returns the descriptor length type if all segments within this region are
   * of the same type.
   * @throws GaiaException if region is empty or if there are at
   * least 2 segments of different types in this region.
   */
  DescriptorLengthType lengthType() const;

  /**
   * Returns the index corresponding to the descriptor contained in this region
   * if and only if this region is composed of a single segment of size 1 (eg:
   * it only contains one single descriptor). Throws an error if the region is
   * empty or contains more than one descriptor.
   */
  int index() const;

  /**
   * Does the same thing as the index() method without arguments, but also
   * checks if the region type is of the specified type and the length-type is
   * the same also, otherwise throws an error.
   */
  int index(DescriptorType type, DescriptorLengthType ltype) const;

  /**
   * Does the same thing as the index() method without arguments, but also
   * checks if the length type is the same.
   */
  int index(DescriptorLengthType ltype) const;

  /**
   * Checks that all segments in this region are of the same type. Does nothing
   * on success, and throws an exception if this condition is not fulfilled.
   *
   * You can optionally specify a PointLayout, which is only used for giving
   * full descriptor names in case of an error.
   */
  void checkTypeOnlyFrom(DescriptorType type, const PointLayout* layout = 0) const;

  /**
   * Returns whether all segments in this region are of the same given type.
   */
  bool isTypeOnlyFrom(DescriptorType type) const;

  /**
   * Checks that all segments in this region are of the same length type. Does
   * nothing on success, and throws an exception if this condition is not
   * fulfilled.
   *
   * You can optionally specify a PointLayout, which is only used for giving
   * full descriptor names in case of an error.
   */
  void checkLengthTypeOnlyFrom(DescriptorLengthType ltype, const PointLayout* layout = 0) const;

  /**
   * Returns whether all segments in this region are of the same given length type.
   */
  bool isLengthTypeOnlyFrom(DescriptorLengthType type) const;

  /**
   * Checks that this Region is composed of only 1 descriptor. Does nothing on
   * success and throws an exception if this condition is not fulfilled.
   */
  void checkSingleDescriptor() const;

  /**
   * Returns the total dimension of all the fields of the specified type in the
   * given point. Note: you need to specify a point because of variable-length
   * fields which change from one point to another.
   */
  int dimension(DescriptorType type = UndefinedType, const Point* p = 0) const;


  /**
   * If this region corresponds to a single descriptor, returns its corresponding
   * Segment. Throws an exception otherwise.
   */
  const Segment& segment() const;

  /**
   * If this region corresponds to a single descriptor of the specified type,
   * returns its corresponding Segment. Throws an exception otherwise.
   */
  const Segment& segment(DescriptorType type) const;

  /**
   * Returns the size of the part of the region which is of the specified type
   * and length type, eg: the sum of the sizes of its segments.
   */
  int size(DescriptorType type, DescriptorLengthType ltype) const;

  // isn't it better to access directly to the segments member variable?
  QVector<int> listIndices(DescriptorType type, DescriptorLengthType ltype) const;

  /**
   * Returns a new region which is a subset of this one and contains all the
   * segments of the specified type.
   */
  Region select(DescriptorType type) const;

  /**
   * Returns a new region which is a subset of this one and contains all the
   * segments of the specified type.
   */
  Region select(DescriptorType type, DescriptorLengthType ltype) const;

  QString toString() const;


  // debug purposes only
  void debug();

  /**
   * Tries to simplify segments if there are contiguous segments, ie:
   * [0,10[ U [10,20[ = [0,20[
   */
  void canonical();

};


} // namespace gaia2

#endif // GAIA_REGION_H
