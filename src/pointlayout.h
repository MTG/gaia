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

#ifndef GAIA_POINTLAYOUT_H
#define GAIA_POINTLAYOUT_H

#include <QSharedData>
#include <QDataStream>
#include <QString>
#include "descriptortree.h"
#include "descriptor.h"

namespace gaia2 {


typedef QMap<Enum, QString> EnumMap;
typedef QMap<QString, Enum> ReverseEnumMap;

class PointLayoutData : public QSharedData {
 public:
  QString name; // only used for debugging

  // each node is a QString, each leaf is a segment.
  // an entire region doesn't have to be stored in each node, as it can be
  // created dynamically on demand by merging all leaves children of the
  // specified node
  // contains real, label, etc... which are in fact encoded in each individual
  // segment
  DescriptorTree* tree;

  // global maps that hold the conversions enum<->string for all descriptors
  QMap<QString, EnumMap> enum2str;
  QMap<QString, ReverseEnumMap> str2enum;

  PointLayoutData();
  PointLayoutData(const PointLayoutData& rhs);
  ~PointLayoutData();
};

/**
 * This class describes the layout of a point. It maps identifier names (QString)
 * to Region.
 */
class PointLayout {

 public:

  PointLayout();
  PointLayout(const PointLayout& other);
  ~PointLayout();

  /**
   * @todo make sure that this does what it's supposed to correctly, in
   *       particular that we didn't forget to copy anything.
   */
  PointLayout copy() const;

  /**
   * Returns the region corresponding to the location of the specified
   * descriptor or group of descriptors.
   */
  Region descriptorLocation(const QString& name) const;

  /**
   * Returns the locations of all the specified descriptors merged into one
   * region.
   */
  Region descriptorLocation(const QStringList& names) const;

  /**
   * Returns the name of the descriptor located at specified position.
   */
  QString descriptorName(DescriptorType type, DescriptorLengthType ltype, int index) const;

  /**
   * Returns all descriptor names from this layout.
   */
  QStringList descriptorNames() const;

  /**
   * Returns the names of the descriptors which are contained in the given region.
   */
  QStringList descriptorNames(const Region& region) const;

  /**
   * Returns the descriptor names from this layout which match any of the given
   * patterns. Patterns are matched using wildcard matching.
   */
  QStringList descriptorNames(const QStringList& patterns) const;

  /**
   * Returns the descriptor names from this layout which match the given
   * pattern and which also are of the specified type. Patterns are matched
   * using wildcard matching.
   * The value @c UndefinedType for the type matches all types.
   */
  QStringList descriptorNames(DescriptorType type, const QString& pattern) const;

  /**
   * Returns the descriptor names from this layout which match any of the given
   * patterns and which also are of the specified type. Patterns are matched
   * using wildcard matching.
   * The value @c UndefinedType for the type matches all types.
   * @param failOnUnmatched if set to true, will throw an exception if a pattern
   *        didn't match any descriptor. If false, it will ignore this pattern and
   *        go on as normal.
   */
  QStringList descriptorNames(DescriptorType type,
                              const QStringList& patterns = QStringList("*"),
                              bool failOnUnmatched = true) const;

  /**
   * Converts the given Enum descriptor into its String representation.
   */
  StringDescriptor enumToString(const QString& name, const EnumDescriptor& value) const;

  /**
   * Converts the given single Enum value into its String representation.
   */
  QString enumToString(const QString& name, const Enum& value) const;

  /**
   * Converts the given String descriptor into an enum. If there was no suitable
   * value for it before, add one in the enumMap.
   */
  EnumDescriptor stringToEnum(const QString& name, const StringDescriptor& str) const;

  /**
   * Converts the given single String value into an enum. If there was no suitable
   * value for it before, add one in the enumMap.
   */
  Enum stringToEnum(const QString& name, const QString& str) const;

  /**
   * Returns true when this layout is the empty layout (eg: just contains a root
   * node without children).
   */
  bool isEmpty() const { return _d->tree->isLeaf(); }

  /**
   * Returns whether the given layout is a subset of this one.
   */
  bool subsetOf(const PointLayout& layout) const;

  /**
   * Adds the given node to this PointLayout. If the name contains dots, this
   * method will split the name on them and add the corresponding intermediate
   * nodes (if necessary).
   */
  void add(const QString& nodeName, DescriptorType type = UndefinedType,
           DescriptorLengthType ltype = VariableLength, int size = 1);

  /**
   * Adds the given node to this PointLayout, attaching it to the node called
   * parentName. If the name contains dots, this method will split the name on
   * them and add the corresponding intermediate nodes (if necessary).
   */
  void add(const QString& parentName, const QString& childName,
           DescriptorType type = UndefinedType,
           DescriptorLengthType ltype = VariableLength, int size = 1);


  /**
   * Remove the given node from this PointLayout.
   */
  void remove(const QString& name);

  /**
   * Remove the given list of nodes from this PointLayout.
   * This is substantially faster than repeated calls to remove(const QString&).
   */
  void remove(const QStringList& names);

  /**
   * Fixes the length of the specified descriptor by changing its length type
   * from VariableLength to FixedLength.
   * WARNING: you cannot call this method if the layout is already been used
   *          by a point or dataset.
   */
  void fixLength(const QString& name, int size);

  /**
   * Fixes the length of the specified descriptors by changing their length type
   * from VariableLength to FixedLength using the associated size.
   */
  void fixLength(const QList<QPair<QString, int> >& descs);

  /**
   * Changes the type of the specified descriptor from StringType to EnumType.
   * WARNING: you cannot call this method if the layout is already been used
   *          by a point or dataset.
   */
  void enumerate(const QString& name);

  /**
   * Returns a new layout which contains the intersection of this layout with
   * the given one.
   */
  PointLayout operator&(const PointLayout& other) const;

  /**
   * Returns the list of descriptors which are different in these two layouts.
   * In particular, returns all descriptors which are in one layout and not in
   * the other, or ones which are in both but with different types.
   */
  QStringList symmetricDifferenceWith(const PointLayout& layout) const;

  /**
   * Returns the list of descriptors which are in this layout, but not in the
   * other one. Also returns the list of descriptors which are in both but with
   * different types.
   */
  QStringList differenceWith(const PointLayout& layout) const;

  /**
   * Returns whether this layout can morph into the given target layout. There
   * are various ways one could achieve this, but at the moment the conditions
   * we check for this function to succeed are the following:
   *  - both layouts have the same descriptor names
   *  - all the ones which don't have the same type have a Real type in this
   *    layout and a String type in the target layout.
   *
   * This can happen when reading a string which can actually be interpreted
   * as a real value (the yaml parser usually does that if not told otherwise).
   */
  bool canMorphInto(const PointLayout& targetLayout) const;

  /**
   * Morphs the Point given as input so that the returned Point has the same
   * layout as this layout.
   *
   * @throw GaiaException if point could not be morphed
   */
  Point* morphPoint(const Point* point) const;

  /**
   * Filters the elements contained in this layout so that the resulting layout
   * only contains those which name match one of the pattern in the select
   * argument and doesn't match any of the patterns in the exclude argument.
   * You can use shell style globbing in the patterns.
   */
  void filter(const QStringList& select, const QStringList& exclude);

  void debug() const;

  /**
   * Returns a yaml string representing:
   *  -# the enumeration maps ("enums") that are being used by this layout
   *  -# the hierachy of descriptors ("descriptors") that this layout contains.
   */
  QString toYaml() const;

  /**
   * Returns the fully qualified name of the given descriptor name.
   */
  QString fullName(const QString& name) const;

  /**
   * Returns the full size of the specified Region. This is mostly useful for
   * the Point class, so it knows how much space it needs to allocate.
   */
  int regionSize(DescriptorType type, DescriptorLengthType ltype) const;

  Region correspondingRegion() const;

  /**
   * needed for merging DataSets, operation allowed only iff
   * ds1.layout == dss2.layout
   */
  bool operator==(const PointLayout& rhs) const;
  bool operator!=(const PointLayout& rhs) const;

  /**
   * Return whether both layout point to the same object in memory.
   * We might want to distinguish it from operator==() because the latter
   * doesn't check for enum2str and str2enum maps.
   */
  bool sameObjectAs(const PointLayout& layout) const;

  bool sameEnumMap(const PointLayout& layout, const QString& enumName) const;

  /**
   * Reserved for internal testing. Should not be used.
   */
  int ref() const { return _d->ref; }

  friend QDataStream& operator<<(QDataStream& out, const PointLayout& layout);
  friend QDataStream& operator>>(QDataStream& in, PointLayout& layout);

 protected:

  QExplicitlySharedDataPointer<PointLayoutData> _d;

  friend class Point; // for access to *NoUpdate methods
  friend PointLayout mergeLayouts(const PointLayout& layout1, const PointLayout& layout2);

  void addNoUpdate(const QString& parentName, const QString& childName,
                   DescriptorType type = UndefinedType,
                   DescriptorLengthType ltype = VariableLength, int size = 1);

  void addNoUpdateFromRoot(const QString& parentName, const QString& childName,
                           DescriptorType type = UndefinedType,
                           DescriptorLengthType ltype = VariableLength, int size = 1);

  void removeNoUpdate(const QString& name);

  void update() { _d->tree->updateLayout(); }

};

QDataStream& operator<<(QDataStream& out, const PointLayout& layout);
QDataStream& operator>>(QDataStream& in, PointLayout& layout);


} // namespace gaia2

#endif // GAIA_POINTLAYOUT_H
