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

#ifndef CYCLOPS_NORMAL
#include <QCoreApplication>
#include <QNetworkAccessManager>
#include "yamlrpcserver.h"
#include "cyclopsproxy.h"
#include "cyclopsmaster.h"
#include <QTextStream>
#ifdef GAIA_QT5
#include <QLoggingCategory>
#include <qapplication.h>
#include <stdio.h>
#include <stdlib.h>
#endif

FILE* logFile;

void logToFile(QtMsgType type, const char *msg) {
  fprintf(logFile, "%s\n", msg);
  fflush(logFile);
}

#ifdef GAIA_QT5
void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
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

int main(int argc, char* argv[]) {
  QTextStream standardOutput(stdout);
#ifdef GAIA_QT5
  qInstallMessageHandler(myMessageOutput);
#endif
  QCoreApplication app(argc, argv);
#ifdef GAIA_QT5
  QLoggingCategory::setFilterRules("*.debug=true");
    qDebug() << "Debugging";
    app.exit();
  const int argumentCount = QCoreApplication::arguments().size();
  const QStringList argumentList = QCoreApplication::arguments();
  if (argumentCount == 0) {
          standardOutput << QObject::tr("Usage: %1 <serialportname> [baudrate]")
                            .arg(argumentList.first()) << endl;
          return 1;
      }
#endif
  //QCoreApplication app(argc, argv);
  gaia2::init();

  //logFile = fopen("/var/log/cyclops/master.log", "w");
  //qInstallMsgHandler(logToFile);

  if (argc != 2) {
    qDebug() << "ERROR: You need to specify the config file for the master Cyclops server...";
    qDebug() << "Exiting...";
    exit(1);
  }


  CyclopsMaster cm;
  cm.setupClients(argv[1]);

  QObject::connect(&cm, SIGNAL(quit()),
                   &app, SLOT(quit()),
                   // we need to make it a queued connection to leave the time to the server
                   // to reply correctly to the client before exiting
                   Qt::QueuedConnection);

  return app.exec();
}
#endif
