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

#ifndef GAIA_GAIAEXCEPTION_H
#define GAIA_GAIAEXCEPTION_H

#include <QString>
#include <QStringList>
#include <QTextStream>
#include <QPair>
#include "baseexception.h"


namespace gaia2 {

/**
 * Exception class that can take up to 3 arguments of any type, which will
 * be serialized into a QString for the error message.
 */
class GaiaException : public gaia2std::GException {
 protected:
  QString _msg;

 public:
  GaiaException() {}
  GaiaException(const QString& msg) : _msg(msg) {}
  GaiaException(const char* msg) : gaia2std::GException(), _msg(msg) {}
  GaiaException(const std::string& msg) : gaia2std::GException(), _msg(QString::fromUtf8(msg.c_str(),
                                                                                         (int)msg.size())) {}
  GaiaException(const QStringList& msg) : gaia2std::GException(), _msg(msg.join("")) {}

  template <typename T, typename U>
  GaiaException(const T& a, const U& b) : gaia2std::GException() {
    QTextStream(&_msg) << a << b;
  }

  template <typename T, typename U, typename V>
  GaiaException(const T& a, const U& b, const V& c) : gaia2std::GException() {
    QTextStream(&_msg) << a << b << c;
  }

  template <typename T, typename U, typename V, typename W>
  GaiaException(const T& a, const U& b, const V& c, const W& d) : gaia2std::GException() {
    QTextStream(&_msg) << a << b << c << d;
  }

  virtual ~GaiaException() throw() {}
  virtual const char* what() const throw() {
    // NB: we need to store the ascii message permanently inside the exception
    // instance, otherwise it gets destroyed when we come out and the pointer is
    // invalid

    // if message was set from a gaia2::GaiaException, copy it into the
    // std::string error message variable
    if (_msg.size() > 0) {
      const_cast<GaiaException*>(this)->_message = _msg.toUtf8().data();
    }
    return _message.c_str();
  }

  const QString& msg() const throw () { return _msg; }
  QString& msg() throw () { return _msg; }
};


inline QTextStream& operator<<(QTextStream& out, const QPair<int, int>& p) {
  return out << '<' << p.first << ", " << p.second << '>';
}


inline QTextStream& operator<<(QTextStream& out, const QStringList& slist) {
  if (slist.empty()) return out << "[]";
  return out << "[ " << slist.join(", ") << " ]";
}


/**
 * Use this define to creates derived instances of GaiaException easily.
 */
#define GAIA_DEFINE_EXCEPTION(UserException)                                           \
class UserException : public gaia2::GaiaException {                                    \
 public:                                                                               \
  UserException() {}                                                                   \
  UserException(const QString& msg) : gaia2::GaiaException(msg) {}                     \
  UserException(const char* msg) : gaia2::GaiaException(msg) {}                        \
  UserException(const std::string& msg) : gaia2::GaiaException(msg) {}                 \
  UserException(const QStringList& msg) : gaia2::GaiaException(msg) {}                 \
  template <typename T, typename U>                                                    \
  UserException(const T& a, const U& b) : gaia2::GaiaException(a, b) {}                \
  template <typename T, typename U, typename V>                                        \
  UserException(const T& a, const U& b, const V& c) : gaia2::GaiaException(a, b, c) {} \
  template <typename T, typename U, typename V, typename W>                            \
  UserException(const T& a, const U& b, const V& c, const W& d) :                      \
    gaia2::GaiaException(a, b, c, d) {}                                                \
};


// Exception hierarchy
GAIA_DEFINE_EXCEPTION(FixedLengthException);

} // namespace gaia2


#endif // GAIA_GAIAEXCEPTION_H
