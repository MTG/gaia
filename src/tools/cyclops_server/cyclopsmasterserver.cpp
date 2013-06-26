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

#include <QCoreApplication>
#include <QNetworkAccessManager>
#include "yamlrpcserver.h"
#include "cyclopsproxy.h"
#include "cyclopsmaster.h"

FILE* logFile;

void logToFile(QtMsgType type, const char *msg) {
  fprintf(logFile, "%s\n", msg);
  fflush(logFile);
}

int main(int argc, char* argv[]) {
  QCoreApplication app(argc, argv);
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
