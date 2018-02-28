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

#include <QFile>
#include <QStringList>
#include <QTextStream>
#include <QBuffer>
#include "gaia.h"
#include "point.h"
#include "utils.h"
using namespace gaia2;


QDataStream& gaia2::operator<<(QDataStream& out, const Scope& scope) {
  return out << scope.name << scope.start << scope.end;
}

QDataStream& gaia2::operator>>(QDataStream& in, Scope& scope) {
  switch (gaiaVersion(in)) {

  case Gaia_2_1:
  case Gaia_2_3:
    G_DEBUG(GIO, "Loading scope, gaia 2.1 format");
    return in >> scope.name >> scope.start >> scope.end;

#ifdef GAIA_QT5
  case Gaia_2_4:
    G_DEBUG(GIO, "Loading scope, gaia 2.1 format");
    return in >> scope.name >> scope.start >> scope.end;
#endif

  case Gaia_2_0:
    G_DEBUG(GIO, "Loading scope, gaia 2.0 format");
    return in;

  default:
    throw GaiaException("Unsupported data format when loading point");
  }
}



QDataStream& gaia2::operator<<(QDataStream& out, const ScopedData& s) {
  return out << s.scope
             << s.data.vreal << s.data.vlabel << s.data.venumeration
             << s.data.freal << s.data.flabel << s.data.fenumeration;
}

QDataStream& gaia2::operator>>(QDataStream& in, ScopedData& s) {
  switch (gaiaVersion(in)) {

  case Gaia_2_1:
  case Gaia_2_3:
    G_DEBUG(GIO, "Loading scoped data, gaia 2.1 format");
    return in >> s.scope
              >> s.data.vreal >> s.data.vlabel >> s.data.venumeration
              >> s.data.freal >> s.data.flabel >> s.data.fenumeration;
#ifdef GAIA_QT5
  case Gaia_2_4:
    G_DEBUG(GIO, "Loading scoped data, gaia 2.1 format");
    return in >> s.scope
              >> s.data.vreal >> s.data.vlabel >> s.data.venumeration
              >> s.data.freal >> s.data.flabel >> s.data.fenumeration;
#endif

  case Gaia_2_0:
    G_DEBUG(GIO, "Loading scoped data, gaia 2.0 format");
    return in >> s.scope >> s.data.vreal >> s.data.vlabel >> s.data.venumeration;

  default:
    throw GaiaException("Unsupported data format when loading scoped data");
  }
}



Point::Point(const PointLayout& layout) {
  setLayout(layout);
}

Point::Point(const QString& name, const PointLayout& layout) {
  setName(name);
  setLayout(layout);
}

Point::Point(const std::string& name, const PointLayout& layout) {
  setName(QString::fromUtf8(name.c_str(), name.size()));
  setLayout(layout);
}

Point::~Point() {
  G_DEBUG(GMemory, "destroying point with name" << _name);
}

bool Point::operator==(const Point& rhs) const {
  return (this->_data == rhs._data) && (this->_layout == rhs._layout);
}


void Point::checkValidSegment(const Point* point, int segment) {
  if ((segment >= 0) && (segment < point->numberSegments())) {
    return;
  }

  QStringList msg;
  msg << "In point '" << point->name() << "': "
      << "You cannot get segment index " << QString::number(segment)
      << " because point only has " << QString::number(point->numberSegments()) << " segment";
  if (point->numberSegments() > 1) msg << "s";

  throw GaiaException(msg);
}

RealDescriptor Point::value(const QString& name) const {
  return value(0, name);
}

RealDescriptor Point::value(int nsegment, const QString& name) const {
  checkValidSegment(this, nsegment);

  Segment location = _layout.descriptorLocation(name).segment(RealType);

  switch (location.ltype) {
  case FixedLength:
    return RealDescriptor(&frealData(nsegment)[location.begin], location.size());

  case VariableLength:
    return vrealData(nsegment)[location.begin];
  }

  throw GaiaException("Internal error @ Point::value");
}

StringDescriptor Point::label(const QString& name) const {
  return label(0, name);
}

StringDescriptor Point::label(int nsegment, const QString& name) const {
  checkValidSegment(this, nsegment);

  Segment location = _layout.descriptorLocation(name).segment();

  if (location.type == StringType) {
    switch (location.ltype) {
    case FixedLength:
      return StringDescriptor(&fstringData(nsegment)[location.begin], location.size());

    case VariableLength:
      return vstringData(nsegment)[location.begin];
    }

    throw GaiaException("Internal error @ Point::label");
  }
  else if (location.type == EnumType) {
    switch (location.ltype) {
    case FixedLength:
      return _layout.enumToString(_layout.descriptorName(EnumType, FixedLength, location.begin),
                                  EnumDescriptor(&fenumData(nsegment)[location.begin],
                                                 location.size()));

    case VariableLength:
      return _layout.enumToString(_layout.descriptorName(EnumType, VariableLength, location.begin),
                                  venumData(nsegment)[location.begin]);
    }

    throw GaiaException("Internal error @ Point::label");
  }

  throw GaiaException("Descriptor '", name, "' is not a String descriptor");
}

void Point::setValue(const QString& name, const RealDescriptor& value) {
  setValue(0, name, value);
}

void Point::setValue(int nsegment, const QString& name, const RealDescriptor& value) {
  checkValidSegment(this, nsegment);

  Segment location = _layout.descriptorLocation(name).segment(RealType);

  switch (location.ltype) {
  case FixedLength: {
    // assert the value we want to set has the correct size
    if (value.size() != location.size()) {
      QStringList msg;
      msg << "Trying to set a value of size " << QString::number(value.size()) << " to "
          << name << ", which is a fixed-length descriptor of size " << QString::number(location.size());
      throw GaiaException(msg);
    }
    Real* desc = &frealData(nsegment)[location.begin];
    for (int i=0; i<value.size(); i++) {
      desc[i] = value[i];
    }
    break;
  }
  case VariableLength:
    vrealData(nsegment)[location.begin] = value;
  }
}

void Point::setLabel(const QString& name, const StringDescriptor& label) {
  setLabel(0, name, label);
}

void Point::setLabel(int nsegment, const QString& name, const StringDescriptor& label) {
  checkValidSegment(this, nsegment);

  Segment location = _layout.descriptorLocation(name).segment();

  if (location.type == StringType) {
    switch (location.ltype) {
    case FixedLength: {
      // assert the value we want to set has the correct size
      if (label.size() != location.size()) {
        QStringList msg;
        msg << "Trying to set a label of size " << QString::number(label.size()) << " to "
            << name << ", which is a fixed-length descriptor of size " << QString::number(location.size());
        throw GaiaException(msg);
      }
      QString* desc = &fstringData(nsegment)[location.begin];
      for (int i=0; i<label.size(); i++) {
        desc[i] = label[i];
      }
      return;
    }

    case VariableLength:
      vstringData(nsegment)[location.begin] = label;
      return;

    }

    throw GaiaException("Internal error @ Point::setLabel");
  }
  else if (location.type == EnumType) {
    switch (location.ltype) {
    case FixedLength: {
      if (label.size() != location.size()) {
        QStringList msg;
        msg << "Trying to set a label of size " << QString::number(label.size()) << " to "
            << name << ", which is a fixed-length descriptor of size " << QString::number(location.size());
        throw GaiaException(msg);
      }
      Enum* desc = &fenumData(nsegment)[location.begin];
      EnumDescriptor enumLabel = _layout.stringToEnum(_layout.descriptorName(EnumType, FixedLength, location.begin),
                                                      label);
      for (int i=0; i<label.size(); i++) {
        desc[i] = enumLabel[i];
      }
      return;
    }

    case VariableLength:
      venumData(nsegment)[location.begin] = _layout.stringToEnum(_layout.descriptorName(EnumType, VariableLength, location.begin),
                                                                 label);
      return;

    }

    throw GaiaException("Internal error @ Point::setLabel");
  }

  throw GaiaException("Descriptor '", name, "' is not a String descriptor");
}



Point* Point::fromSingleSegment(const Point* point, int nsegment) {
  checkValidSegment(point, nsegment);

  Point* result = new Point();
  result->setName(point->name() + "_" + point->data()[nsegment].scope.name);
  result->setLayout(point->layout());
  result->_data.resize(1);
  result->_data[0] = point->data()[nsegment];

  return result;
}

void Point::setSegment(int nsegment, const Point* point, int origsegment) {
  checkValidSegment(point, origsegment);

  if (layout() != point->layout()) {
    throw GaiaException("The two points don't share the same layout, cannot set segment from one to the other");
  }

  if (nsegment < 0) {
    throw GaiaException("Trying to set data for segment ", nsegment, " which is negative");
  }

  if (numberSegments() == 0) {
    setLayout(point->layout());
  }

  if (nsegment >= numberSegments()) {
    applyLayout(nsegment+1);
  }

  this->data()[nsegment] = point->data()[origsegment];
}

void Point::setLayout(const PointLayout& layout, int nsegments) {
  if (nsegments > 0) {
    _data.resize(nsegments);
  }

  remapLayoutAndEnums(layout);

  if (_data.size() == 0) {
    applyLayout(1); // as we don't know yet, let's assume there is only 1 segment
  }
  else {
    applyLayout(_data.size());
  }
}

// make protected as it doesn't check for a valid layout
void Point::remapLayoutAndEnums(const PointLayout& layout) {
  // adapt the values of the enums to the map of the new layout, adding
  // missing mappings if necessary
  FORSEG(*this) {
    // fixed-length enums
    EnumDescriptor& fenums = fenumData(nseg);
    for (int i=0; i<fenums.size(); i++) {
      QString dname = _layout.descriptorName(EnumType, FixedLength, i);
      fenums[i] = layout.stringToEnum(dname, _layout.enumToString(dname, fenums[i]));
    }

    // variable-length enums
    Array<EnumDescriptor>& venums = venumData(nseg);
    for (int i=0; i<venums.size(); i++) {
      QString dname = _layout.descriptorName(EnumType, VariableLength, i);
      venums[i] = layout.stringToEnum(dname, _layout.enumToString(dname, venums[i]));
    }
  }

  // actually assign the new layout
  _layout = layout;
}

void Point::switchLayout(const PointLayout& layout) {
  if (_layout != layout) {
    throw GaiaException("Can only switch a point layout with an identical layout");
  }
  remapLayoutAndEnums(layout);
}


DescriptorType detectSequenceType(const yaml::Sequence& seq) {
  if (seq.empty()) return UndefinedType;
  switch (seq[0].type()) {

  case yaml::ScalarType: {
    bool ok;
    seq[0].scalar().toFloat(&ok);
    if (ok) return RealType;
    else return StringType;
  }

  case yaml::SequenceType:
    return detectSequenceType(seq[0].sequence());

  case yaml::MappingType:
    throw GaiaException("Error while parsing: found a map inside of a sequence. What is this??");

  }

  throw GaiaException("You just reached an unreachable place in Point::detectSequenceType...");
}

int sequenceDimension(const yaml::Sequence& seq, int n = 1) {
  if (seq.empty()) return n;

  switch (seq[0].type()) {
  case yaml::ScalarType: return n;
  case yaml::SequenceType: return sequenceDimension(seq[0].sequence(), n+1);
  case yaml::MappingType:
    throw GaiaException("Error while parsing: found a map inside of a sequence. What is this??");
  }
  throw GaiaException("You just reached an unreachable place in Point::sequenceDimension...");
}


/**
 * @todo rewrite me, I'm ugly!!!!!!!!
 */
RealDescriptor parseYamlSequenceReal(const yaml::Sequence& seq) {
  bool ok;
  int size = seq.size();
  if (size == 0) return RealDescriptor(0, Real(0.0));

  if (sequenceDimension(seq) > 2) {
    throw GaiaException("Parsing of arrays of dimension > 2 is not allowed yet");
  }

  // dimension 1: vector
  if (seq[0].type() == yaml::ScalarType) {
    RealDescriptor result(size, Real(0.0));
    for (int i=0; i<size; i++) {
      Q_ASSERT(seq[i].type() == yaml::ScalarType);
      result[i] = seq[i].scalar().toDouble(&ok);
      Q_ASSERT(ok);
    }
    return result;
  }
  // dimension 2: matrix
  else {
    RealDescriptor d(0, Real(0.0));
    int nrows = seq.size();
    int ncols = 0;
    if (nrows > 0) {
      ncols = seq[0].sequence().size();
    }
    d << nrows << ncols;
    foreach (const yaml::Node& row, seq) {
      foreach (const yaml::Node& value, row.sequence()) {
        Q_ASSERT(value.type() == yaml::ScalarType);
        d.append(value.scalar().toFloat(&ok));
        Q_ASSERT(ok);
      }
    }
    return d;
  }

  throw GaiaException("You just reached an unreachable place in Point::parseYamlSequence...");
}

/**
 * @todo rewrite me, I'm ugly!!!!!!!!
 */
StringDescriptor parseYamlSequenceString(const yaml::Sequence& seq) {
  if (seq.size() == 0) return StringDescriptor();

  // dimension 1: vector
  if (seq[0].type() == yaml::ScalarType) {
    StringDescriptor d(seq.size(), "");
    for (int i=0; i<seq.size(); i++) {
      const QString& str = seq[i].scalar();
      if (str.at(0).isNull()) {
        d[i] = str.mid(1);
      }
      else {
        d[i] = str;
      }
    }
    return d;
  }

  throw GaiaException("You just reached an unreachable place in Point::parseYamlSequence...");
}

void Point::yamlNodeToLayout(const yaml::Node& root, PointLayout& layout,
                             const QString& parentName) {
  Q_ASSERT(root.type() == yaml::MappingType);

  bool ok;
  const yaml::Mapping& subnodes = root.mapping();

  foreach (const QString& name, subnodes.keys()) {

    // if it is a map, we're still recursing down the hierarchy of descriptor,
    // then go on recursively, otherwise (scalar or sequence), we are at a leaf
    // and we have to auto-detect the type of the leaf
    const yaml::Node& node = subnodes[name];
    switch (node.type()) {

    case yaml::ScalarType: {
      const QString& scalar = node.scalar();
      if (scalar.at(0).isNull()) {
        // this scalar had been quoted, do not attempt to cast into a float
        layout.addNoUpdateFromRoot(parentName, name, StringType);
      }
      else {
        scalar.toFloat(&ok);
        if (ok) {
          layout.addNoUpdateFromRoot(parentName, name, RealType);
        }
        else { // not a real problem, just that the scalar is actually a string
          layout.addNoUpdateFromRoot(parentName, name, StringType);
        }
      }
    }
      break;

    case yaml::SequenceType: {
      DescriptorType type = detectSequenceType(node.sequence());

      if (type == UndefinedType) {
        // TODO: find a way to specify certain descriptors for which we know the
        //       type beforehand, so empty lists are of a known type
        layout.addNoUpdateFromRoot(parentName, name, RealType);
      }
      else {
        layout.addNoUpdateFromRoot(parentName, name, type);
      }
      break;
    }

    case yaml::MappingType:
      layout.addNoUpdateFromRoot(parentName, name, UndefinedType);
      yamlNodeToLayout(subnodes[name], layout, parentName + NAME_SEPARATOR + name);
      break;

    } // switch
  }
}

void Point::fillLayout(const yaml::Node& root, const PointLayout& layout,
                       UnscopedData& data, const QString& parentName) {
  Q_ASSERT(root.type() == yaml::MappingType);

  bool ok;
  const yaml::Mapping& subnodes = root.mapping();
  Region r;

  foreach (const QString& name, subnodes.keys()) {

    // if it is a map, we're still recursing down the hierarchy of descriptor,
    // then go on recursively, otherwise (scalar or sequence), we are at a leaf
    // and we must fill in the values (and auto-detect their types)
    const yaml::Node& node = subnodes[name];
    QString fullName = parentName + NAME_SEPARATOR + name;
    switch (node.type()) {

    case yaml::ScalarType: {
      try { r = layout.descriptorLocation(fullName); }
      catch (GaiaException&) { break; }

      if (r.type() == RealType) {
        double value = node.scalar().toDouble(&ok);
        Q_ASSERT(ok); // it worked before, should work now as well!!
        data.vreal[r.index()] = value;
      }
      else { // if it's not a real, it's a string
        const QString& str = node.scalar();
        if (str.at(0).isNull()) {
          data.vlabel[r.index()] = str.mid(1);
        }
        else {
          data.vlabel[r.index()] = str;
        }
      }
      break;
    }

    case yaml::SequenceType: {
      try { r = layout.descriptorLocation(fullName); }
      catch (GaiaException&) { break; }

      DescriptorType type = detectSequenceType(node.sequence());

      if (type == RealType) {
        data.vreal[r.index()] = parseYamlSequenceReal(node.sequence());
      }
      else if (type == StringType) {
        data.vlabel[r.index()] = parseYamlSequenceString(node.sequence());
      }
      else if (type == UndefinedType) {
        // @TODO should do better than that!! getlayout assumes it's real, so ok
        data.vreal[r.index()].resize(0); // empty list
      }
      else {
        Q_ASSERT(false);
      }
      break;
    }

    case yaml::MappingType:
      fillLayout(subnodes[name], layout, data, fullName);
      break;

    }

  }
}

void Point::loadFromString(const std::string& str,
                           const QStringList& select, const QStringList& exclude) {
  yaml::Node yamldesc = yaml::loadFromString(str, true);
  load(yamldesc, select, exclude);
}

void Point::load(const QString& filename,
                 const QStringList& select, const QStringList& exclude) {
  G_DEBUG(GPoint, "--------------- loading" << filename);
  yaml::Node yamldesc = yaml::loadFromFile(filename, true);
  load(yamldesc, select, exclude);
}

void Point::load(yaml::Node& yamldesc,
                 const QStringList& select, const QStringList& exclude) {
  yaml::Mapping segments;
  try {
    segments = yamldesc["segments"].mapping();
  }
  catch (GaiaException&) {
    // don't do nothing, just go on with an empty list of segments
  }

  // remove segments descriptor
  yamldesc.mapping().remove("segments");
  //yamldesc.mapping().remove("regions");

  // remove configuration descriptor
  yamldesc.mapping().remove("configuration");

  _layout = PointLayout();
  yamlNodeToLayout(yamldesc, _layout);
  _layout.filter(select, exclude); // layout.update() contained in filter

  // TODO: sort the segments map by segment name

  // check that all segments actually have the same layout
  bool checkSegmentsLayout = true;
  if (checkSegmentsLayout) {
    foreach (const QString& segn, segments.keys()) {
      PointLayout layout;
      yamlNodeToLayout(segments[segn]["descriptors"], layout);
      if (!layout.subsetOf(_layout)) {
        throw GaiaException("Layout from segment ", segn, " is not compatible with main layout");
      }
    }
  }

  applyLayout(segments.size()+1);

  fillLayout(yamldesc, _layout, _data[0].data);

  int i = 1;
  foreach (const QString& segn, segments.keys()) {
    _data[i].scope.name = segments[segn]["name"];
    yaml::Sequence scope = segments[segn]["scope"].sequence();
    _data[i].scope.start = scope[0].scalar().toFloat();
    _data[i].scope.end = scope[1].scalar().toFloat();
    fillLayout(segments[segn]["descriptors"], _layout, _data[i].data);
    i++;
  }

}

void Point::applyLayout(int nsegments) {
  _data.resize(nsegments);

  // resize all the different descriptor type vectors
  for (int i=0; i<nsegments; i++) {
    UnscopedData& data = _data[i].data;
    int oldRSize = data.freal.size();
    data.freal.resize(_layout.regionSize(RealType, FixedLength));
    data.flabel.resize(_layout.regionSize(StringType, FixedLength));
    int oldESize = data.fenumeration.size();
    data.fenumeration.resize(_layout.regionSize(EnumType, FixedLength));
    data.vreal.resize(_layout.regionSize(RealType, VariableLength));
    data.vlabel.resize(_layout.regionSize(StringType, VariableLength));
    data.venumeration.resize(_layout.regionSize(EnumType, VariableLength));

    // QVarLengthArray doesn't initialize its elements if they're from a basic type
    // do this here to avoid weird effects, price to pay shouldn't be that high...
    int size = data.freal.size();
    if (size > oldRSize) memset(&data.freal[oldRSize], 0, (size-oldRSize)*sizeof(Real));
    size = data.fenumeration.size();
    if (size > oldESize) memset(&data.fenumeration[oldESize], 0, (size-oldESize)*sizeof(Real));
  }
}


void Point::fromBase64(const std::string& data) {
  fromBase64(QByteArray(data.c_str(), data.size()));
}

void Point::fromBase64(const QByteArray& data) {
  QByteArray array = QByteArray::fromBase64(data);
  QBuffer buffer(&array);
  buffer.open(QIODevice::ReadOnly);

  QDataStream in(&buffer);
  checkValidDataStream(in);

  PointLayout layout;
  in >> layout >> *this;

  setLayoutUnsafe(layout);
}

std::string Point::toBase64() const {
  QByteArray array;
  QBuffer buffer(&array);
  buffer.open(QIODevice::WriteOnly);

  QDataStream out(&buffer);
  setDataStreamVersion(out);

  out << this->_layout << *this;

  return array.toBase64().data();
}



QDataStream& gaia2::operator<<(QDataStream& out, const Point& point) {
  // do we output the layout or not? how can we specify it as an option?
  return out << point._name << point._data;
}

QDataStream& gaia2::operator>>(QDataStream& in, Point& point) {
  in >> point._name;
  G_DEBUG(GIO, "Loading point" << point._name);
  return in >> point._data;
}
