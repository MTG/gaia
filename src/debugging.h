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


#ifndef GAIA_DEBUGGING_H
#define GAIA_DEBUGGING_H

#include <QDebug>
#include <QTextStream>
#include <QStringList>
#include <QVariant>

namespace gaia2 {

enum DebuggingModule {
  // general modules
  GMemory         = 1 << 0,
  GAlgorithms     = 1 << 1,
  GMath           = 1 << 2,
  GParser         = 1 << 3,
  GIO             = 1 << 4,
  GPerf           = 1 << 5,
  GTools          = 1 << 6,

  // class specific modules
  GDataSet        = 1 << 10,
  GDescriptorTree = 1 << 11,
  GRegion         = 1 << 12,
  GPoint          = 1 << 13,
  GView           = 1 << 14,
  GSearchSpace    = 1 << 15,

  GNone           = 0,
  GAll            = (1 << 30) -1
};


const char* debugModuleDescription(DebuggingModule module);

extern int activatedDebugLevels;

void setDebugLevel(int levels);
void unsetDebugLevel(int levels);

/**
 * Asynchronous thread-safe logger object. (TODO: implementation is still not thread-safe)
 */
class Logger {
 protected:
  QStringList _msgQueue;
  bool _addHeader;

  void flush();

 public:
  Logger() : _addHeader(true) {}

  void debug(DebuggingModule module, const QString& msg, bool resetHeader = false);
  void info(const QString& msg);
  void warning(const QString& msg);
  void error(const QString& msg);

};

extern Logger loggerInstance;

class Stringifier {
 protected:
  QString _str;
  QTextStream _stream;

 public:
  Stringifier() : _str(), _stream(&_str) {}

  Stringifier& operator<<(const QVariant& var) {
    _stream << '"' << var.toString() << '"';
    return *this;
  };

  Stringifier& operator<<(const QStringList& slist) {
    if (slist.empty()) {
      _stream << "[]";
      return *this;
    }

    _stream << "[ " << slist.join(", ") << " ]";
    return *this;
  }

  template <typename T>
  Stringifier& operator<<(const QList<T>& list) {
    if (list.isEmpty()) {
      _stream << "[]";
      return *this;
    }

    typename QList<T>::const_iterator it = list.constBegin();
    *this << "[ " << *it;
    for (++it; it != list.constEnd(); ++it) {
      *this << ", " << *it;
    }
    _stream << " ]";
    return *this;
  }

  template <typename T>
  Stringifier& operator<<(const T& obj) {
    _stream << obj;
    return *this;
  }

  const QString& str() const { return _str; }
};


} // namespace gaia2


#define G_ACTIVE(module) ((module) & activatedDebugLevels)
#define G_STRINGIFY(msg) (gaia2::Stringifier() << msg).str()

#define G_HLINE "--------------------------------------------------------------------------------"
#define G_TITLE(str) G_STRINGIFY(QString((80-QString(str).size()-2)/2, '-') \
                                 << ' ' << str << ' ' <<            \
                                 QString((80-QString(str).size()-1)/2, '-'))

#define G_DEBUG(module, msg) if (G_ACTIVE(module)) loggerInstance.debug(module, G_STRINGIFY(msg), true)
#define G_INFO(msg) if (::gaia2::verbose) loggerInstance.info(G_STRINGIFY(msg))
#define G_WARNING(msg) loggerInstance.warning(G_STRINGIFY(msg))
#define G_ERROR(msg) loggerInstance.error(G_STRINGIFY(msg))

#endif // GAIA_DEBUGGING_H
