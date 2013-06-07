#include <QCoreApplication>
#include <QFile>
#include <QTextStream>
#include <QThreadPool>
#include <iostream>
#include "dataset.h"
#include "yamlcpp.h"
#include "textprogress.h"
#include "utils.h"
using namespace std;
using namespace gaia2;


// ugly global variables...
QStringList descs_select("*"), descs_exclude;
PointArray _validPoints;
PointLayout* layout = 0;


void usage() {
  qFatal("ERROR: not enough arguments...\n\n\
Usage: gaiamerge list_of_sigfiles output_dataset [ start end ] [--reflayout=...] [--select=...] [--exclude=...]");
}

ParameterMap parseOptions(int argc, char* argv[]) {
  if (argc < 3) {
    usage();
  }

  QStringList args;
  for (int i=0; i<argc; i++) args << argv[i];

  ParameterMap options;
  options.insert("inputFiles", args[1]);
  options.insert("dataset", args[2]);


  // this is ugly, but works...
  for (int i=0; i<args.size(); i++) {
    if (args[i].startsWith("--select=")) {
      descs_select = args[i].mid(9).split(",");
      args.removeAt(i);
      i--;
    }

    if (args[i].startsWith("--exclude=")) {
      descs_exclude = args[i].mid(10).split(",");
      args.removeAt(i);
      i--;
    }

    if (args[i].startsWith("--reflayout=")) {
      layout = new PointLayout();
      Point refp; refp.load(args[i].mid(12));
      *layout = refp.layout();

      args.removeAt(i);
      i--;
    }

    if (args[i].startsWith("--nthreads=")) {
      QThreadPool::globalInstance()->setMaxThreadCount(args[i].mid(11).toInt());
      args.removeAt(i);
      i--;
    }
  }


  int start = 0, end = 10000000;

  if (args.size() > 3) start = args[3].toInt();
  if (args.size() > 4) end = args[4].toInt();
  if (args.size() > 5) usage();

  options.insert("start", start);
  options.insert("end", end);

  return options;
}


int main(int argc, char* argv[]) {
  QCoreApplication app(argc, argv);
  gaia2::verbose = true;

  // get command-line options
  ParameterMap options = parseOptions(argc, argv);

#ifdef USE_BZ2_ENCRYPTION
  yaml::Node filelist = yaml::loadFromPlainFile(options.value("inputFiles"));
#else
  yaml::Node filelist = yaml::loadFromFile(options.value("inputFiles"));
#endif

  const yaml::Mapping& tracks = filelist.mapping();

  // convert our yaml::Map to a QMap we can give to mergeFiles
  QMap<QString, QString> sigfiles;
  yaml::Mapping::const_iterator it = tracks.constBegin();
  for (; it != tracks.constEnd(); ++it) {
    sigfiles.insert(it.key().toString(), it.value().toString());
  }

  //qDebug() << "Found" << tracks.size() << "in filelist";

  // do the actual merging
  DataSet* dataset = DataSet::mergeFiles(sigfiles,
                                         descs_select, descs_exclude,
                                         options.value("start").toInt(),
                                         options.value("end").toInt(),
                                         layout);

  // save dataset & clean up stuff
  qDebug() << "Saving dataset...";

  dataset->save(options.value("dataset"));

  qDebug() << "Dataset successfully saved!";

  // let the OS reclaim the memory itself, much faster than deleting the dataset here...
  //delete dataset;

  return 0;
}
