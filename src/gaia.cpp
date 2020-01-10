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

#include <QTextCodec>
#include "gaia.h"
#include "algorithms/analyzer.h"
#include "algorithms/applier.h"
#include "metrics/distancefunction.h"
#include "metrics/frozendistance.h"
#include "searchspacepool.h"


#ifdef HAVE_TBB
#include <tbb/task_scheduler_init.h>

static tbb::task_scheduler_init* tbbinit = 0;
#endif // HAVE_TBB

namespace gaia2 {

bool verbose;

const char* version = GAIA_VERSION;
const char* version_git_sha = GAIA_GIT_SHA;

// message handler that aborts execution, so that when run inside gdb we can
// still get a stack trace. Very useful!
// from https://doc.qt.io/qt-5/qtglobal.html#qInstallMessageHandler
void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    const char *file = context.file ? context.file : "";
    const char *function = context.function ? context.function : "";
    switch (type) {
    case QtDebugMsg:
        fprintf(stderr, "Debug: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
        break;
    case QtInfoMsg:
        fprintf(stderr, "Info: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
        break;
    case QtWarningMsg:
        fprintf(stderr, "Warning: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
        break;
    case QtCriticalMsg:
        fprintf(stderr, "Critical: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
        break;
    case QtFatalMsg:
        fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
        break;
    }
}


void init() {
  static bool initialized = false;
  if (initialized) return;

  QTextCodec::setCodecForLocale(QTextCodec::codecForName("utf-8"));

  qInstallMessageHandler(messageHandler);

  registerAnalyzers();
  registerAppliers();
  registerMetrics();
  registerFrozenMetrics();

  // by default, verbose = true (people know what's going on)
  verbose = true;

  // set debug level
  activatedDebugLevels = GNone
    //| GMemory
    //| GAlgorithms
    //| GMath
    //| GParser

    //| GDataSet
    //| GDescriptorTree
    //| GRegion
    //| GPoint
    //| GView

    //| GAll
    ;

#ifdef HAVE_TBB
  delete tbbinit;
  tbbinit = new tbb::task_scheduler_init;
#endif

  initialized = true;
}

void shutdown() {
  SearchSpacePool::clear();
  FrozenSearchSpacePool::clear();

#ifdef HAVE_TBB
  delete tbbinit;
  tbbinit = 0;
#endif
}

} // namespace gaia2
