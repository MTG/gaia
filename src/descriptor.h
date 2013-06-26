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

#ifndef GAIA_DESCRIPTOR_H
#define GAIA_DESCRIPTOR_H

#include <QTextStream>
#include "types.h"
#include "gaiamath.h"

namespace gaia2 {


// There are 3 solutions for the point structure:
//
// 1- Point = vector<(scope, UnscopedData)>, UnscopedData = vector<Descriptor>
// 2- Point = vector<SingleDescriptor>, SingleDescriptor = vector<(scope, Descriptor)>
// 3- Point = vector<vector<Descriptor> >, scope info stored apart, with maybe optional
//    variable indicating whether we actually correspond to type 1 or 2


template <typename T>
class SingleValueMorphable : public Array<T> {
 public:
  SingleValueMorphable() : Array<T>(1, T()) {}
  SingleValueMorphable(int size, const T& value) : Array<T>(size, value) {}
  SingleValueMorphable(const T& value) : Array<T>(1, value) {}
  SingleValueMorphable(const SingleValueMorphable& value) : Array<T>(value) {}
  SingleValueMorphable(const T* arr, int size) : Array<T>(size) {
    for (int i=0; i<size; i++) (*this)[i] = arr[i];
  }
  SingleValueMorphable(const std::vector<T>& v) : Array<T>((int)v.size()) {
    for (uint i=0; i<v.size(); i++) (*this)[i] = v[i];
  }

 protected:
  // this constructor is made protected because it should not be called, as it
  // creates confusion between a SVM of size n, or a SVM of size 1 & value n...
  SingleValueMorphable(int ambiguous) {
    throw GaiaException("Cannot call SingleValueMorphable::SVM(int) because it is ambiguous.");
  }

 public:
  SingleValueMorphable& operator=(const SingleValueMorphable& value) {
    Array<T>::operator=(value);
    return *this;
  }

  SingleValueMorphable& operator=(const T& value) {
    this->resize(1);
    this->front() = value;
    return *this;
  }

  const T& toSingleValue() const {
    if (this->size() != 1) {
      throw GaiaException("Could not convert descriptor to single value (size=", this->size(), ")");
    }
    return this->at(0);
  }

  // commented this automatic conversion because it actually causes a lot of
  // ambiguities, and it is probably better to state explicitly when we want to
  // convert from a SingleValueMorphable to a single value (catches errors earlier)
  /*
  operator const T&() const {
    return toSingleValue();
  }
  */

  void checkSize(const SingleValueMorphable& rhs) const {
    if (this->size() != rhs.size()) {
      throw GaiaException("left-operand and right-operand do not have same size");
    }
  }


#define transferAssignmentOperator(OP)                                 \
  SingleValueMorphable& operator OP(const SingleValueMorphable& rhs) { \
    checkSize(rhs);                                                    \
    for (int i=0; i<this->size(); i++) {                               \
      this->operator[](i) OP rhs.at(i);                                \
    }                                                                  \
    return *this;                                                      \
  }

  transferAssignmentOperator(+=);
  transferAssignmentOperator(-=);
  transferAssignmentOperator(*=);
  transferAssignmentOperator(/=);

#undef transferAssignmentOperator

#define transferBinaryOperator(OP)                                          \
  SingleValueMorphable operator OP(const SingleValueMorphable& rhs) const { \
    SingleValueMorphable result(*this);                                     \
    return result.operator OP##=(rhs);                                      \
  }

  transferBinaryOperator(+);
  transferBinaryOperator(-);
  transferBinaryOperator(*);
  transferBinaryOperator(/);

#undef transferBinaryOperator

#define transferUnaryOperator(OP)             \
  SingleValueMorphable operator OP() const {  \
    SingleValueMorphable result(*this);       \
    for (int i=0; i<this->size(); i++) {      \
      result.operator[](i) = OP result.at(i); \
    }                                         \
    return result;                           \
  }

  transferUnaryOperator(-);

#undef transferUnaryOperator

#define transferAssignmentOperatorFromSingleValue(OP) \
  SingleValueMorphable& operator OP(const T& x) {     \
    for (int i=0; i<this->size(); i++) {              \
      this->operator[](i) OP x;                       \
    }                                                 \
    return *this;                                     \
  }

  transferAssignmentOperatorFromSingleValue(+=);
  transferAssignmentOperatorFromSingleValue(-=);
  transferAssignmentOperatorFromSingleValue(*=);
  transferAssignmentOperatorFromSingleValue(/=);

#undef transferAssignmentOperatorFromSingleValue

#define transferBinaryOperatorFromSingleValue(OP)      \
  SingleValueMorphable operator OP(const T& x) const { \
    SingleValueMorphable result(*this);                \
    return result.operator OP##=(x);                   \
  }

  transferBinaryOperatorFromSingleValue(+);
  transferBinaryOperatorFromSingleValue(-);
  transferBinaryOperatorFromSingleValue(*);
  transferBinaryOperatorFromSingleValue(/);

#undef transferBinaryOperatorFromSingleValue


#define transferComparison(OP)               \
  bool operator OP(const T& x) const {       \
    for (int i=0; i<this->size(); i++) {     \
      if (!(this->at(i) OP x)) return false; \
    }                                        \
    return true;                             \
  }

  transferComparison(<);
  transferComparison(<=);
  transferComparison(>);
  transferComparison(>=);

#undef transferComparison

  bool operator==(const Array<T>& x) const {
    return Array<T>::operator==(x);
  }

  bool operator==(const T& x) const {
    return toSingleValue() == x;
  }

  SingleValueMorphable& minimum(const SingleValueMorphable& rhs) {
    checkSize(rhs);
    for (int i=0; i<this->size(); i++) {
      this->operator[](i) = std::min(this->at(i), rhs.at(i));
    }
    return *this;
  }

  SingleValueMorphable& maximum(const SingleValueMorphable& rhs) {
    checkSize(rhs);
    for (int i=0; i<this->size(); i++) {
      this->operator[](i) = std::max(this->at(i), rhs.at(i));
    }
    return *this;
  }

  SingleValueMorphable& minimum(const T& minv) {
    for (int i=0; i<this->size(); i++) {
      this->operator[](i) = std::min(this->at(i), minv);
    }
    return *this;
  }

  SingleValueMorphable& maximum(const T& maxv) {
    for (int i=0; i<this->size(); i++) {
      this->operator[](i) = std::max(this->at(i), maxv);
    }
    return *this;
  }


  bool isnan() const {
    for (int i=0; i<this->size(); i++) {
      if (gaia2::isnan(this->at(i))) return true;
    }
    return false;
  }

  bool isinf() const {
    for (int i=0; i<this->size(); i++) {
      if (gaia2::isinf(this->at(i))) return true;
    }
    return false;
  }

  SingleValueMorphable& inverse() {
    for (int i=0; i<this->size(); i++) {
      this->operator[](i) = (T)1.0 / this->at(i);
    }
    return *this;
  }

  SingleValueMorphable& opposite() {
    for (int i=0; i<this->size(); i++) {
      this->operator[](i) = -this->at(i);
    }
    return *this;
  }

  SingleValueMorphable& sqrt() {
    for (int i=0; i<this->size(); i++) {
      this->operator[](i) = std::sqrt(this->at(i));
    }
    return *this;
  }

  SingleValueMorphable& abs() {
    for (int i=0; i<this->size(); i++) {
      this->operator[](i) = std::abs(this->at(i));
    }
    return *this;
  }

  // only makes sense for RealDescriptor
  T norm() const {
    T result = 0;
    for (int i=0; i<this->size(); i++) {
      result += (*this)[i] * (*this)[i];
    }
    return std::sqrt(result);
  }

  QString toString() const {
    QString result;
    QTextStream s(&result);

    if (this->size() == 1) s << this->at(0);
    else if (this->size() == 0) s << "[]";
    else {
      s << "[ " << this->at(0);
      for (int i=1; i<this->size(); i++) {
        s << ", " << this->at(i);
      }
      s << " ]";
    }
    return result;
  }

};


template <typename T>
SingleValueMorphable<T> operator-(const T& x, const SingleValueMorphable<T>& y) {
  return (y - x).opposite();
}

template <typename T>
SingleValueMorphable<T> operator/(const T& x, const SingleValueMorphable<T>& y) {
  return (y / x).inverse();
}

template <typename T>
SingleValueMorphable<T> operator+(const T& x, const SingleValueMorphable<T>& y) {
  return y + x;
}

template <typename T>
SingleValueMorphable<T> operator*(const T& x, const SingleValueMorphable<T>& y) {
  return y * x;
}

template <typename T>
Stringifier& operator<<(Stringifier& out, const SingleValueMorphable<T>& array) {
  return out << array.toString();
}

typedef SingleValueMorphable<QString> StringDescriptor;
typedef SingleValueMorphable<Real> RealDescriptor;
typedef SingleValueMorphable<Enum> EnumDescriptor;


template <>
inline RealDescriptor sqrt(const RealDescriptor& x) {
  return RealDescriptor(x).sqrt();
}


} // namespace gaia2

#endif // GAIA_DESCRIPTOR_H
