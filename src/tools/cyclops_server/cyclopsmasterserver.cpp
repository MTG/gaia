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
