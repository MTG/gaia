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

#ifndef GAIA_TYPES_H
#define GAIA_TYPES_H

#include <QMap>
#include <QStringList>
#include <QPointF>
#include "Eigen/Dense"
#include "osdetect.h"
#include "gaia.h"
#include "gaiaexception.h"
#include "debugging.h"
#include "gvarlengtharray.h"

//////////////////////////////////////////////////////////////////////////////
// ----[ Fixed-length integer types ]---------------------------------------//

typedef unsigned int uint;
typedef unsigned char uint8;
typedef signed int sint;
typedef signed char sint8;

#ifndef OS_WIN32

#include <inttypes.h>

typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;
typedef int16_t sint16;
typedef int32_t sint32;
typedef int64_t sint64;

#else // OS_WIN32

typedef unsigned __int16 uint16;
typedef unsigned __int32 uint32;
typedef unsigned __int64 uint64;
typedef __int16 sint16;
typedef __int32 sint32;
typedef __int64 sint64;

#endif // OS_WIN32


namespace gaia2 {

//////////////////////////////////////////////////////////////////////////////
// ----[ Base types used mostly everywhere ]--------------------------------//

typedef float Real;
typedef int Enum;

template <typename T>
class Array : public GVarLengthArray<T, 1> {
 public:
  Array() : GVarLengthArray<T, 1>() {}
  Array(int size) : GVarLengthArray<T, 1>(size) {}
  Array(int size, const T& value) : GVarLengthArray<T, 1>(size) {
    for (int i=0; i<size; i++) (*this)[i] = value;
  }
  //friend QDataStream& operator<< <>(QDataStream& out, const Array<T>& array);
  //friend QDataStream& operator>> <>(QDataStream& in, Array<T>& array);

  const T& at(int i) const { return (*this)[i]; }
  Array& operator<<(const T& x) { this->append(x); return *this; }
  T& front() { return (*this)[0]; }
  const T& front() const { return at(0); }
  Array& fill(const T& val, int size) {
    this->resize(size);
    for (int i=0; i<size; i++) {
      (*this)[i] = val;
    }
    return *this;
  }

  int size() const { return GVarLengthArray<T, 1>::size(); }

  const T* constBegin() const { return GVarLengthArray<T, 1>::constData(); }
  const T* begin() const { return GVarLengthArray<T, 1>::constData(); }
  T* begin() { return GVarLengthArray<T, 1>::data(); }

  const T* constEnd() const { return GVarLengthArray<T, 1>::constData() + size(); }
  const T* end() const { return GVarLengthArray<T, 1>::constData() + size(); }
  T* end() { return GVarLengthArray<T, 1>::data() + size(); }

  bool operator==(const Array<T>& rhs) const {
    if (this->size() != rhs.size()) return false;
    for (int i=0; i<size(); i++) {
      if (at(i) != rhs.at(i)) return false;
    }
    return true;
  }

  bool operator!=(const Array<T>& rhs) const { return !operator==(rhs); }
};


template <typename T>
QDataStream& operator<<(QDataStream& out, const Array<T>& array) {
  out << (qint32)array.size();
  for (int i=0; i<array.size(); i++) {
    out << array[i];
  }
  return out;
}

template <typename T>
QDataStream& operator>>(QDataStream& in, Array<T>& array) {
  qint32 size;
  in >> size;
  array.resize(size);
  for (int i=0; i<size; i++) {
    in >> array[i];
  }
  return in;
}


/**
 * Map class (derived from QMap) but that throws a custom Exception instead of
 * returning a default constructed value when the key isn't in there.
 */
template <typename Key, typename Value, typename ExceptionType = GaiaException>
class GaiaMap : public QMap<Key, Value> {
 public:
  const Value& value(const Key& key) const {
    typename QMap<Key, Value>::const_iterator it = this->find(key);
    if (it == this->end()) {
      throw ExceptionType("Key '", key, "' not found in map");
    }
    return it.value();
  }

  // NB: this function has no non-const counterpart, because it makes no sense
  const Value& value(const Key& key, const Value& defaultValue) const {
    try {
      return value(key);
    }
    catch (ExceptionType&) {
      return defaultValue;
    }
  }

  inline const Value& operator[](const Key& key) const {
    return value(key);
  }

  Value& value(const Key& key) {
    typename QMap<Key, Value>::iterator it = this->find(key);
    if (it == this->end()) {
      throw ExceptionType("Key '", key, "' not found in map");
    }
    return it.value();
  }

  inline Value& operator[](const Key& key) {
    return value(key);
  }
};

template <typename Key, typename Value, typename ExceptionType>
Stringifier& operator<<(Stringifier& out, const GaiaMap<Key, Value, ExceptionType>& gmap) {
  out << "{";
  if (gmap.isEmpty()) return out << "}";
  typename GaiaMap<Key, Value, ExceptionType>::const_iterator it = gmap.constBegin();
  out << " " << it.key() << ": " << it.value();
  for (++it; it != gmap.constEnd(); ++it) {
    out << ", " << it.key() << ": " << it.value();
  }
  return out << " }";
}



typedef Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> MatrixRXf;
typedef Eigen::RowVectorXf FrozenPoint;


} // namespace gaia2

#endif // GAIA_TYPES_H
