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

#include <QStringList>
#include "pointlayout.h"
#include "utils.h"
using namespace gaia2;

PointLayoutData::PointLayoutData() {
  tree = new DescriptorTree;
}

PointLayoutData::PointLayoutData(const PointLayoutData& rhs) {
  Q_UNUSED(rhs);
  throw GaiaException("who calls PLData::copy_ctor???");
}

PointLayoutData::~PointLayoutData() {
  G_DEBUG(GMemory, "deleting layout with n ref =" << (int)this->ref);
  delete tree;
}

PointLayout::PointLayout() {
  _d = new PointLayoutData;
}

PointLayout::PointLayout(const PointLayout& other) : _d(other._d) {}

PointLayout::~PointLayout() {
  // automatic unref
}

PointLayout PointLayout::copy() const {
  PointLayout newLayout;
  // we created a RegionTree that is empty and that we will discard, because
  // we need a copy of the old one. hence the delete (otherwise we leak).
  delete newLayout._d->tree;
  newLayout._d->tree = _d->tree->copy();
  newLayout._d->enum2str = _d->enum2str;
  newLayout._d->str2enum = _d->str2enum;
  return newLayout;
}

Region PointLayout::descriptorLocation(const QString& name) const {
  return _d->tree->find(name)->correspondingRegion();
}

Region PointLayout::descriptorLocation(const QStringList& names) const {
  Region result;
  foreach (QString name, names) {
    result.merge(descriptorLocation(name));
  }
  result.canonical();
  return result;
}

QString PointLayout::descriptorName(DescriptorType type, DescriptorLengthType ltype, int index) const {
  return _d->tree->descriptorName(type, ltype, index);
}

QStringList PointLayout::descriptorNames() const {
  return _d->tree->descriptorNames();
}

QStringList PointLayout::descriptorNames(const Region& region) const {
  QSet<QString> descs;
  foreach (const Segment& seg, region.segments) {
    for (int i=seg.begin; i<seg.end; i++) {
      descs.insert(descriptorName(seg.type, seg.ltype, i));
    }
  }

  // preserve the alphabetical order that got screwed because of the QSet
  QStringList result = QStringList::fromSet(descs);
  result.sort();

  return result;
}

QStringList PointLayout::descriptorNames(const QStringList& patterns) const {
  return descriptorNames(UndefinedType, patterns);
}

QStringList PointLayout::descriptorNames(DescriptorType type,
                                         const QString& pattern) const {
  return descriptorNames(type, QStringList() << pattern);
}

QStringList PointLayout::descriptorNames(DescriptorType type,
                                         const QStringList& patterns,
                                         bool failOnUnmatched) const {
  QStringList result, fromType;
  QStringList allNames = descriptorNames();

  // find all descriptor names which are of the specified type...º
  foreach (QString name, allNames) {
    if (type == UndefinedType ||
        _d->tree->findLeaf(name)->type() == type) {
      fromType.append(name);
    }
  }

  // ...and look among them the ones which have a matching pattern
  foreach (QString pattern, patterns) {
    QRegExp rexp;
    bool exactMatch = true;

    QString escapedPattern = pattern;
    escapedPattern.replace("[", "[[");
    escapedPattern.replace("]", "]]");
    escapedPattern.replace("[[", "[[]");
    escapedPattern.replace("]]", "[]]");

    if (pattern.startsWith(".")) {
      // no wildcard matching at the beginning, we only want a full match
      rexp.setPattern(escapedPattern);
    }
    else if (pattern.startsWith("*")) {
      // we specified wildcard at the beginning, thus we accept all matches
      rexp.setPattern(escapedPattern);
    }
    else {
      // we want to have wildcard matching at the beginning, but will only accept
      // one match type and throw an exception if there's an ambiguity
      rexp.setPattern("." + escapedPattern);
      exactMatch = false;
    }

    rexp.setPatternSyntax(QRegExp::Wildcard);

    QStringList matches;
    if (exactMatch) {
      foreach (QString name, fromType) {
        if (rexp.exactMatch(name)) {
          matches << name;
        }
      }
    }
    else { // !exactMatch
      QSet<QString> prefixes;
      foreach (QString name, fromType) {
        if ((rexp.indexIn(name) >= 0) && // we found a match...
            (rexp.pos() + rexp.matchedLength() == name.size())) { // ...and nothing's trailing
          matches << name;
          prefixes << name.left(rexp.pos());
        }
      }

      if (prefixes.size() > 1) {
        QStringList msg;
        msg << "PointLayout: pattern \"" << pattern << "\" is ambiguous.\n"
            << "Possible matches are:";
        foreach (const QString& m, matches) msg << " " << m;
        throw GaiaException(msg);
      }
    }

    // pattern = "*" returning no match on an empty list is not an error condition
    if (failOnUnmatched && matches.empty() && pattern != "*") {
      throw GaiaException("PointLayout: pattern \"", pattern, "\" didn't match any descriptor");
    }

    // insert matches only if they're not already in there
    foreach (QString name, matches) {
      if (!result.contains(name)) result.append(name);
    }

  }

  return result;
}


StringDescriptor PointLayout::enumToString(const QString& name, const EnumDescriptor& enumd) const {
  static QString valueNotFound("Internal error @ PointLayout::enumToString");
  StringDescriptor result(enumd.size(), QString());
  EnumMap& enum2str = _d->enum2str[name];
  for (int i=0; i<enumd.size(); i++) {
    result[i] = enum2str.value(enumd[i], valueNotFound);
  }
  return result;
}

QString PointLayout::enumToString(const QString& name, const Enum& enumv) const {
  static QString valueNotFound("Internal error @ PointLayout::enumToString");
  return _d->enum2str[name].value(enumv, valueNotFound);
}


EnumDescriptor PointLayout::stringToEnum(const QString& name, const StringDescriptor& strd) const {
  EnumDescriptor result(strd.size(), Enum());
  ReverseEnumMap& str2enum = _d->str2enum[name];

  for (int i=0; i<strd.size(); i++) {
    const QString& str = strd[i];

    // insert new enum value if needed (not present in the translation maps before)
    if (!str2enum.contains(str)) {
      Enum newv = str2enum.size();
      Q_ASSERT(!_d->enum2str[name].keys().contains(newv));
      str2enum.insert(str, newv);
      _d->enum2str[name].insert(newv, str);
    }

    result[i] = str2enum.value(strd[i]);
  }
  return result;
}

Enum PointLayout::stringToEnum(const QString& name, const QString& str) const {
  ReverseEnumMap& str2enum = _d->str2enum[name];

  // insert new enum value if needed (not present in the translation maps before)
  if (!str2enum.contains(str)) {
    Enum newv = str2enum.size();
    Q_ASSERT(!_d->enum2str[name].keys().contains(newv));
    str2enum.insert(str, newv);
    _d->enum2str[name].insert(newv, str);
  }

  return str2enum.value(str);
}


Region PointLayout::correspondingRegion() const {
  return _d->tree->correspondingRegion();
}


int PointLayout::regionSize(DescriptorType type, DescriptorLengthType ltype) const {
  return _d->tree->regionSize(type, ltype);
}


void PointLayout::addNoUpdate(const QString& parentName, const QString& childName,
                              DescriptorType type, DescriptorLengthType ltype, int size) {
  DescriptorTree* node = _d->tree->find(parentName);
  node->addChild(childName, type, ltype, size);
}

void PointLayout::addNoUpdateFromRoot(const QString& parentName, const QString& childName,
                                      DescriptorType type, DescriptorLengthType ltype, int size) {
  DescriptorTree* node = _d->tree->findFromRoot(parentName);
  node->addChild(childName, type, ltype, size);
}

void PointLayout::removeNoUpdate(const QString& name) {
  _d->tree->removeNode(name);
}

void PointLayout::add(const QString& childName,
                      DescriptorType type, DescriptorLengthType ltype, int size) {
  if (_d->ref > 1) throw GaiaException("PointLayout: layout is shared hence immutable");
  QString parent = "";
  QStringList nodes = splitName(childName);
  if (nodes.isEmpty()) return;

  for (int i=0; i<nodes.size()-1; i++) {
    add(parent, nodes[i], type);
    parent = parent + "." + nodes[i];
  }
  add(parent, nodes[nodes.size()-1], type, ltype, size);
}

void PointLayout::add(const QString& parentName, const QString& childName,
                      DescriptorType type, DescriptorLengthType ltype, int size) {
  if (_d->ref > 1) throw GaiaException("PointLayout: layout is shared hence immutable");
  addNoUpdate(parentName, childName, type, ltype, size);
  try {
    update();
  }
  catch (GaiaException&) {
    // there was a leaf of unknown type, remove it and throw a new exception
    // with a nice message
    QString fullName = mergeName(QStringList(parentName) << childName);
    _d->tree->removeNode(fullName, false); // don't recurse
    throw GaiaException("Cannot add leaf node ", childName, " with undefined type");
  }
}

void PointLayout::remove(const QString& name) {
  if (_d->ref > 1) throw GaiaException("PointLayout: layout is shared hence immutable");
  removeNoUpdate(name);
  update();
}

void PointLayout::remove(const QStringList& names) {
  if (_d->ref > 1) throw GaiaException("PointLayout: layout is shared hence immutable");
  _d->tree->removeNodes(names);
  update();
}


void PointLayout::fixLength(const QString& name, int size) {
  fixLength(QList<QPair<QString, int> >() << qMakePair(name, size));
}

void PointLayout::fixLength(const QList<QPair<QString, int> >& descs) {
  if (_d->ref > 1) throw GaiaException("PointLayout: layout is shared hence immutable");

  // first find all the descriptors, as this needs the layout to be in a valid state
  QList<QPair<DescriptorTree*, int> > dtrees;
  for (int i=0; i<descs.size(); i++) {
    dtrees << qMakePair(_d->tree->findLeaf(descs[i].first), descs[i].second);
  }

  // now we can fix the length of them all at once
  for (int i=0; i<dtrees.size(); i++) {
    dtrees[i].first->fixLength(dtrees[i].second);
  }

  update();
}


void PointLayout::enumerate(const QString& name) {
  if (_d->ref > 1) throw GaiaException("PointLayout: layout is shared hence immutable");
  DescriptorTree* desc = _d->tree->findLeaf(name);
  desc->enumerate();
  update();
}


QStringList PointLayout::symmetricDifferenceWith(const PointLayout& other) const {
  QSet<QString> diff;
  QSet<QString> desc1 = QSet<QString>::fromList(this->descriptorNames());
  QSet<QString> desc2 = QSet<QString>::fromList(other.descriptorNames());

  // get descriptors in this dataset and not the other one
  diff |= (desc1 - desc2);

  // add descriptors in the other one but not in this one
  diff |= (desc2 - desc1);

  // add those which are in both but with different types
  foreach (QString desc, (desc1 & desc2)) {
    if ((this->descriptorLocation(desc).type() != other.descriptorLocation(desc).type()) ||
        (this->descriptorLocation(desc).lengthType() != other.descriptorLocation(desc).lengthType())) {
      diff << desc;
    }
  }

  return diff.toList();
}

QStringList PointLayout::differenceWith(const PointLayout& other) const {
  QSet<QString> diff;
  QSet<QString> desc1 = QSet<QString>::fromList(this->descriptorNames());
  QSet<QString> desc2 = QSet<QString>::fromList(other.descriptorNames());

  // get descriptors in this dataset and not the other one
  diff |= (desc1 - desc2);

  // add those which are in both but with different types
  foreach (QString desc, (desc1 & desc2)) {
    if ((this->descriptorLocation(desc).type() != other.descriptorLocation(desc).type()) ||
        (this->descriptorLocation(desc).lengthType() != other.descriptorLocation(desc).lengthType())) {
      diff << desc;
    }
  }

  return diff.toList();
}


PointLayout PointLayout::operator&(const PointLayout& other) const {
  PointLayout result = this->copy();

  foreach (const QString& descname, differenceWith(other)) {
    result.remove(descname);
  }

  return result;
}


bool PointLayout::canMorphInto(const PointLayout& targetLayout) const {
  if (*this == targetLayout) return true;

  QSet<QString> desc1 = QSet<QString>::fromList(this->descriptorNames());
  QSet<QString> desc2 = QSet<QString>::fromList(targetLayout.descriptorNames());

  // get descriptors in this dataset and not the other one
  if (!(desc1 - desc2).empty()) return false;

  // get descriptors in the other one but not in this one
  if (!(desc2 - desc1).empty()) return false;

  // get those which are in both but with different types
  QSet<QString> diff;
  foreach (const QString& desc, desc1) {
    if ((this->descriptorLocation(desc).type() != targetLayout.descriptorLocation(desc).type()) ||
        (this->descriptorLocation(desc).lengthType() != targetLayout.descriptorLocation(desc).lengthType())) {
      diff << desc;
    }
  }

  // basic check (which should cover most, if not all the cases) in which the parser
  // could fail to auto-detect the correct type: look whether all different descriptors
  // are actually string descriptors which have been autodetected as real values
  // TODO: also deal with enum values, which would involve changing the morphPoint method too
  foreach (const QString& desc, diff) {
    if (!(targetLayout.descriptorLocation(desc).type() == StringType &&
          this->descriptorLocation(desc).type() == RealType)) {
      return false;
    }
  }

  return true;
}


Point* PointLayout::morphPoint(const Point* p) const {
  Point* morphedPoint = new Point();
  morphedPoint->setName(p->name());
  morphedPoint->setLayout(*this, p->numberSegments());

  QStringList diff = symmetricDifferenceWith(p->layout());

  // copy all real descriptors which are in the dataset layout
  foreach (const QString& desc, this->descriptorNames(RealType)) {
    FORSEG(*p) morphedPoint->setValue(nseg, desc, p->value(nseg, desc));
  }
  // copy the different descriptors that are string in dataset, real in point
  foreach (const QString& desc, diff) {
    FORSEG(*p) {
      RealDescriptor oldv = p->value(nseg, desc);
      StringDescriptor newv(oldv.size(), "");
      for (int i=0; i<oldv.size(); i++) {
        newv = QString::number(oldv[i]);
      }
      morphedPoint->setLabel(nseg, desc, newv);
    }
  }
  // copy the string descriptors which are in the point
  foreach (const QString& desc, p->layout().descriptorNames(StringType)) {
    FORSEG(*p) morphedPoint->setLabel(nseg, desc, p->label(nseg, desc));
  }
  // and finally copy all enum descriptors
  FORSEG(*p) {
    morphedPoint->fenumData(nseg) = p->fenumData(nseg);
    morphedPoint->venumData(nseg) = p->venumData(nseg);
  }

  return morphedPoint;
}


void PointLayout::filter(const QStringList& select, const QStringList& exclude) {
  if (_d->ref > 1) throw GaiaException("PointLayout: layout is shared hence immutable");
  QStringList toKeep = selectDescriptors(*this, UndefinedType, select, exclude);
  QStringList all = descriptorNames();

  foreach (QString name, toKeep) {
    all.removeAll(name);
  }

  foreach (QString name, all) {
    removeNoUpdate(name);
  }

  update();
}

bool PointLayout::subsetOf(const PointLayout& layout) const {
  return _d->tree->subtreeOf(layout._d->tree);
}

QString PointLayout::fullName(const QString& name) const {
  return _d->tree->find(name)->fullName();
}



void PointLayout::debug() const {
  _d->tree->printLeafSegments();
}


QString PointLayout::toYaml() const {
  QStringList result;
  result << "enums:";
  foreach (QString descName, _d->enum2str.keys()) {
    result << QString("  ") + descName + ":";
    const QMap<int, QString>& emap = _d->enum2str[descName];
    foreach (int enumv, emap.keys()) {
      QStringList line;
      line << "    " << QString::number(enumv) << ": " << emap.value(enumv);
      result << line.join("");
    }
  }
  result << _d->tree->toYaml();

  return result.join("\n");
}



bool PointLayout::operator==(const PointLayout& rhs) const {
  return (*_d->tree == *rhs._d->tree);
}

bool PointLayout::operator!=(const PointLayout& rhs) const {
  return !operator==(rhs);
}

bool PointLayout::sameObjectAs(const PointLayout& layout) const {
  return _d == layout._d;
}

bool PointLayout::sameEnumMap(const PointLayout& layout, const QString& enumName) const {
  return (_d->enum2str[enumName] == layout._d->enum2str[enumName] &&
          _d->str2enum[enumName] == layout._d->str2enum[enumName]);
}


QDataStream& gaia2::operator<<(QDataStream& out, const PointLayout& layout) {
  return out << *layout._d->tree << layout._d->enum2str << layout._d->str2enum;
}

QDataStream& gaia2::operator>>(QDataStream& in, PointLayout& layout) {
  switch (gaiaVersion(in)) {

  case Gaia_2_1:
  case Gaia_2_3:
    G_DEBUG(GIO, "Loading point layout, gaia 2.1 format");
    in >> *layout._d->tree >> layout._d->enum2str >> layout._d->str2enum;
    break;

  case Gaia_2_0:
    G_DEBUG(GIO, "Loading point layout, gaia 2.0 format");
    in >> *layout._d->tree;;
    break;

  default:
    throw GaiaException("Unsupported data format when loading point");
  }

  // should be useless, but just to make sure
  layout.update();
  return in;
}
