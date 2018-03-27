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

#include <QTextCodec>
#include <QtGlobal>
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

#ifndef GAIA_QT5
// message handler that aborts execution, so that when run inside gdb we can
// still get a stack trace. Very useful!
// got the idea from http://der-dakon.net/blog/KDE/tracing-qassert.html

static void qtMsgHandler(QtMsgType type, const char *msg) {
  fprintf(stderr, "%s\n", msg);
  if (type == QtFatalMsg) {
    abort();
  }
}
#else
void qtMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    switch (type) {
    case QtDebugMsg:
        fprintf(stderr, "Debug: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtInfoMsg:
        fprintf(stderr, "Info: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtWarningMsg:
        fprintf(stderr, "Warning: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtCriticalMsg:
        fprintf(stderr, "Critical: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtFatalMsg:
        fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        abort();
    }
}
#endif

void init() {
  static bool initialized = false;
  if (initialized) return;

  QTextCodec::setCodecForLocale(QTextCodec::codecForName("utf-8"));

#ifndef GAIA_QT5
  qInstallMsgHandler(qtMsgHandler);
#else
  qInstallMessageHandler(qtMessageHandler);
#endif
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
