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

#include "region.h"
#include "utils.h"
using namespace gaia2;

// the default undefined segment
Segment Segment::undefined("", UndefinedType);

QString Segment::toString() const {
  QStringList repr;
  repr << typeToString(type);
  if (ltype == FixedLength) {
    repr << "FL";
  }
  else {
    repr << "VL";
  }
  repr << "[" << QString::number(begin) << QString::number(end) << "]";

  return repr.join(" ");
}


QDataStream& gaia2::operator<<(QDataStream& out, const Segment& segment) {
  out << segment.name << (qint32)segment.type << (qint32)segment.ltype
      << (qint32)segment.begin << (qint32)segment.end;
  return out;
}


QDataStream& gaia2::operator>>(QDataStream& in, Segment& segment) {
  qint32 t, l;
  switch (gaiaVersion(in)) {

  case Gaia_2_1:
  case Gaia_2_3:
    G_DEBUG(GIO, "Loading segment, gaia 2.1 format");
    in >> segment.name >> t >> l >> segment.begin >> segment.end;
    segment.type = (DescriptorType)t;
    segment.ltype = (DescriptorLengthType)l;
    return in;

  case Gaia_2_0:
    G_DEBUG(GIO, "Loading segment, gaia 2.0 format");
    in >> segment.name >> t >> segment.begin >> segment.end;
    segment.type = (DescriptorType)t;
    return in;

  default:
    throw GaiaException("Unsupported data format when loading segment");
  }
}



DescriptorType Region::type() const {
  if (segments.empty()) {
    throw GaiaException("Impossible to take the type from an empty region");
  }

  DescriptorType t = segments[0].type;
  try {
    checkTypeOnlyFrom(t);
  }
  catch (GaiaException&) {
    throw GaiaException("Taking the type of a region composed of multiple segments of different types is undefined...");
  }

  return t;
}

DescriptorLengthType Region::lengthType() const {
  if (segments.empty()) {
    throw GaiaException("Impossible to take the length type from an empty region");
  }

  DescriptorLengthType t = segments[0].ltype;
  try {
    checkLengthTypeOnlyFrom(t);
  }
  catch (GaiaException&) {
    throw GaiaException("Taking the length type of a region composed of multiple segments of different types is undefined...");
  }

  return t;
}

QString Region::toString() const {
  QString result("[");
  foreach (Segment seg, segments) {
    result += QString("(") + seg.name + ", " + typeToString(seg.type) + ", " +
      lengthTypeToString(seg.ltype) + ", " +
      QString::number(seg.begin) + ", " + QString::number(seg.end) + ")";
  }
  result += "]";
  return result;
}

int Region::index() const {
  if ((segments.empty()) ||
      (segments.size() > 1) ||
      (segments[0].end - segments[0].begin > 1)) {
    throw GaiaException("Cannot get the index of region because it doesn't consist in a single one ", toString());
  }
  return segments[0].begin;
}

int Region::index(DescriptorLengthType ltype) const {
  // first try to actually get the index, so we know next line is valid
  int idx = index();
  if (ltype != segments[0].ltype) {
    QStringList msg;
    msg << "Cannot get index of this region using type " << lengthTypeToString(ltype)
        << " because it is of type " << lengthTypeToString(this->segments[0].ltype);
    throw GaiaException(msg);
  }
  return idx;
}

int Region::index(DescriptorType type, DescriptorLengthType ltype) const {
  if (type != this->type() || ltype != this->lengthType()) {
    QStringList msg;
    msg << "Cannot get index of this region using type " << typeToString(type)
        << " because it is of type " << typeToString(this->type());
    throw GaiaException(msg);
  }

  return index(ltype);
}

void Region::checkTypeOnlyFrom(DescriptorType type, const PointLayout* layout) const {
  QSet<QString> wrongType;
  foreach (const Segment& seg, segments) {
    if (seg.type != type) {
      if (layout) wrongType << layout->descriptorName(seg.type, seg.ltype, seg.begin);
      else        wrongType << seg.name;
    }
  }

  // if everything is good, return now, else throw an informative exception
  if (wrongType.isEmpty()) return;

  QStringList msg;
  msg << "Region contains segments (" << QStringList(QStringList::fromSet(wrongType)).join(", ") << ") "
      << "which are not from " << typeToString(type) << " type";
  throw GaiaException(msg);
}

bool Region::isTypeOnlyFrom(DescriptorType type) const {
  try {
    checkTypeOnlyFrom(type);
    return true;
  }
  catch (GaiaException& e) {
    return false;
  }
}

void Region::checkLengthTypeOnlyFrom(DescriptorLengthType ltype, const PointLayout* layout) const {
  QSet<QString> wrongLengthType;
  foreach (const Segment& seg, segments) {
    if (seg.ltype != ltype) {
      if (layout) wrongLengthType << layout->descriptorName(seg.type, seg.ltype, seg.begin);
      else        wrongLengthType << seg.name;
    }
  }

  // if everything is good, return now, else throw an informative exception
  if (wrongLengthType.isEmpty()) return;

  QStringList msg;
  msg << "Region contains segments (" << QStringList(QStringList::fromSet(wrongLengthType)).join(", ") << ") "
      << "which are not from " << lengthTypeToString(ltype) << " type";
  throw GaiaException(msg);
}


bool Region::isLengthTypeOnlyFrom(DescriptorLengthType type) const {
  try {
    checkLengthTypeOnlyFrom(type);
    return true;
  }
  catch (GaiaException& e) {
    return false;
  }
}

void Region::checkSingleDescriptor() const {
  if ((segments.size() != 1) ||
      ((segments[0].ltype == VariableLength) && (segments[0].end - segments[0].begin > 1))) {
    throw GaiaException("This region is not composed of a single descriptor.");
  }
}


int Region::dimension(DescriptorType type, const Point* p) const {
  int dim = 0;
  foreach (const Segment& seg, segments) {
    if (type == UndefinedType || seg.type == type) {
      switch (seg.ltype) {

      case FixedLength:
        dim += seg.size();
        break;

      case VariableLength:
        if (!p) throw GaiaException("Region::dimension: you need to specify a sample point when trying to compute dimension on variable-length descriptors");
        switch (seg.type) {
        case RealType:   dim += p->vrealData()[seg.begin].size(); break;
        case EnumType:   dim += p->venumData()[seg.begin].size(); break;
        case StringType: dim += p->vstringData()[seg.begin].size(); break;
        default: throw GaiaException("Region::dimension: internal error.");
        }
        break;
      }
    }
  }

  return dim;
}



const Segment& Region::segment() const {
  checkSingleDescriptor();
  return segments[0];
}

const Segment& Region::segment(DescriptorType type) const {
  checkSingleDescriptor();
  if (segments[0].type != type) {
    throw GaiaException("Descriptor is not of type ", typeToString(type));
  }
  return segments[0];
}


int Region::size(DescriptorType type, DescriptorLengthType ltype) const {
  int result = 0;
  foreach (const Segment& seg, segments) {
    if (seg.type == type && seg.ltype == ltype) {
      result += seg.size();
    }
  }
  return result;
}


QVector<int> Region::listIndices(DescriptorType type, DescriptorLengthType ltype) const {
  QVector<int> result;
  foreach (const Segment& seg, segments) {
    if (seg.type == type && seg.ltype == ltype) {
      for (int i=seg.begin; i<seg.end; i++) {
        result.append(i);
      }
    }
  }
  return result;
}

Region Region::select(DescriptorType type) const {
  Region result;
  result.name = this->name;
  foreach (const Segment& seg, segments) {
    if (seg.type == type) {
      result.segments << seg;
    }
  }
  return result;
}

Region Region::select(DescriptorType type, DescriptorLengthType ltype) const {
  Region result;
  result.name = this->name;
  foreach (const Segment& seg, segments) {
    if ((seg.type == type || type == UndefinedType) && seg.ltype == ltype) {
      result.segments << seg;
    }
  }
  return result;
}



/**
 * Surely there is an algorithmically "smart" way to do this...
 */
Region& Region::merge(const Region& r) {
  this->segments += r.segments;
  canonical();
  return *this;
}


bool compareSegments(const Segment& s1, const Segment& s2) {
  return s1.begin < s2.begin;
}

QList<Segment> mergeContiguous(DescriptorType type, DescriptorLengthType ltype,
                                 const QList<Segment>& segments) {
  QList<Segment> segs;
  foreach (const Segment& seg, segments) {
    if (seg.type == type && seg.ltype == ltype) segs.append(seg);
  }

  sort(range(segs), compareSegments);

  // first, check there is no overlap, and throw an exception if there is
  for (int i=0; i<segs.size()-1; i++) {
    if (segs[i].end > segs[i+1].begin) {
      QString msg = "Trying to merge overlapping segments: [%1; %2[ with [%3; %4[";
      throw GaiaException(msg.arg(segs[i].begin)
                          .arg(segs[i].end)
                          .arg(segs[i+1].begin)
                          .arg(segs[i+1].end));
    }
  }

  // then, merge those which are contiguous
  /** @todo implement me */
  // WARNING: when implemented, there are some algorithms that might fail, such
  //          as Normalize & Center, because they assume that distinct segments
  //          in a region correspond to different descriptors, which might not
  //          hold true anymore when we do merge those.
  return segs;
}

void Region::canonical() {
  // empty at the moment, which is not really a problem, because it is correct
  // not to do anything, it is just done for performance reasons.
  // this arises the question: should this method be made const and the list of
  // segments mutable or sth similar? (probably not)

  /** @todo even though we don't merge anything, make sure there is no overlap between regions, and if there is, then remove the overlap */

  // first, check there is no overlap, and throw an exception if there is

  // doesn't make so much sense for var-length descriptors
  mergeContiguous(RealType, FixedLength, this->segments);
  mergeContiguous(StringType, FixedLength, this->segments);
  mergeContiguous(EnumType, FixedLength, this->segments);
}


void Region::debug() {
  G_DEBUG(GRegion, segments.size() << "segments");
  foreach (const Segment& s, segments) {
    G_DEBUG(GRegion, s.name << s.begin << s.end);
  }
}
