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
#include <iostream>
#include "debugging.h"
#include "gaia.h"
using namespace std;

namespace gaia2 {

int activatedDebugLevels = 0;

Logger loggerInstance;

const char* debugModuleDescription(DebuggingModule module) {
  switch (module) {
  case GMemory:         return "[  Memory  ] ";
  case GAlgorithms:     return "[Algorithms] ";
  case GMath:           return "[   Math   ] ";
  case GParser:         return "[  Parser  ] ";
  case GIO:             return "[    IO    ] ";
  case GPerf:           return "[   Perf   ] ";
  case GTools:          return "[  Tools   ] ";

  // class specific modules
  case GDataSet:        return "[ DataSet  ] ";
  case GDescriptorTree: return "[ DescTree ] ";
  case GRegion:         return "[  Region  ] ";
  case GPoint:          return "[  Point   ] ";
  case GView:           return "[   View   ] ";
  case GSearchSpace:    return "[  SSpace  ] ";

  case GNone:           return "[          ] ";
  case GAll:            return "[   ALL    ] ";
  default:              return "[  Mixed   ] ";
  }
}

void setDebugLevel(int levels) {
  activatedDebugLevels |= levels;
}

void unsetDebugLevel(int levels) {
  activatedDebugLevels &= ~levels;
}


// NOTE: in a thread-safe implementation, the msg queue would be thread-safe and
//       the flushing would need to happen in a separate thread
//       This can be achieved using tbb::concurrent_queue

void Logger::flush() {
  while (!_msgQueue.isEmpty()) {
    //qDebug(_msgQueue.takeFirst().toUtf8().constData()); // TODO: should we flush here somehow?
    std::cerr << _msgQueue.takeFirst().toStdString() << std::flush;
  }
}

void Logger::debug(DebuggingModule module, const QString& msg, bool resetHeader) {
  static const char* BLUE_FONT = "\x1B[0;34m";
  static const char* RESET_FONT = "\x1B[0m";

  if (module & activatedDebugLevels) {
    if (_addHeader) {
      foreach (const QString& s, msg.split('\n')) {
        _msgQueue << G_STRINGIFY(BLUE_FONT << debugModuleDescription(module) << RESET_FONT << s << '\n');
      }
    }
    else {
      _msgQueue << msg;
    }

    _addHeader = resetHeader;
    flush();
  }
}

void Logger::info(const QString& msg) {
  if (!::gaia2::verbose) return;
  static const char* GREEN_FONT = "\x1B[0;32m";
  static const char* RESET_FONT = "\x1B[0m";
  foreach (const QString& s, msg.split('\n')) {
    _msgQueue << G_STRINGIFY(GREEN_FONT << "[   INFO   ] " << RESET_FONT << s << '\n');
  }
  flush();
}

void Logger::warning(const QString& msg) {
  // TODO: should use qWarning here?
  static const char* YELLOW_FONT = "\x1B[0;33m";
  static const char* RESET_FONT = "\x1B[0m";
  foreach (const QString& s, msg.split('\n')) {
    _msgQueue << G_STRINGIFY(YELLOW_FONT << "[ WARNING  ] " << RESET_FONT << s << '\n');
  }
  flush();
}

void Logger::error(const QString& msg) {
  // TODO: should use qCritical here?
  static const char* RED_FONT = "\x1B[0;31m";
  static const char* RESET_FONT = "\x1B[0m";
  foreach (const QString& s, msg.split('\n')) {
    _msgQueue << G_STRINGIFY(RED_FONT << "[  ERROR   ] " << RESET_FONT << s << '\n');
  }
  flush();
}



} // namespace gaia2
