#include <QCoreApplication>
#include <QFile>
#include "yamlrpcserver.h"
#include "cyclopsproxy.h"
#include "cyclops.h"
#include "yamlcpp.h"


int main(int argc, char* argv[]) {
  QCoreApplication app(argc, argv);
  gaia2::init();

  if (argc != 1 && argc != 3) {
    qDebug() << "ERROR: Wrong number of arguments...";
    qDebug() << "USAGE: " << argv[0] << " -c config_file (for standalone mode)";
    qDebug() << "   or: " << argv[0] << " [-p port] (for slave mode)";
    qDebug() << "Exiting...";
    exit(1);
  }

  int port = 5151;

  Cyclops cyclops;

  if (argc == 3) {
    QString mode(argv[1]);
    if (mode == "-c") {
      QFile file(argv[2]);
      if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "ERROR: couldn't open file:" << argv[2];
        qDebug() << "Exiting...";
        exit(1);
      }
      gaia2::yaml::Mapping config = gaia2::yaml::load(file.readAll()).mapping();
      
      if (config.contains("port")) {
        bool ok;
        QString portValue = config.value("port").toString();
        port = portValue.toUInt(&ok);
        if (!ok || port >= 65535) {
          qDebug() << "ERROR: Invalid port for serving requests:" << portValue;
          qDebug() << "Exiting...";
          exit(1);
        }
      }
      
      cyclops.setupFromYaml(config);
    }
    else if (mode == "-p") {
      bool ok;
      QString portValue(argv[2]);
      port = portValue.toUInt(&ok);
      if (!ok || port >= 65535) {
        qDebug() << "ERROR: Invalid port for serving requests:" << portValue;
        qDebug() << "Exiting...";
        exit(1);
      }
    }
    else {
      qDebug() << "ERROR: Invalid option:" << mode;
      qDebug() << "Exiting...";
      exit(1);
    }
  }
  
  qDebug() << "Using port" << port;

  CyclopsProxy proxy(&cyclops);
  YamlRPCServer s(port, &proxy);

  QObject::connect(&s, SIGNAL(quit()),
                   &app, SLOT(quit()),
                   // we need to make it a queued connection to leave the time to the server
                   // to reply correctly to the client before exiting
                   Qt::QueuedConnection);

  return app.exec();
}
