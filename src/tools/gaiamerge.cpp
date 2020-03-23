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
bool failOnUnmatched = true;


void usage() {
  qFatal("ERROR: not enough arguments...\n\n\
Usage: gaiamerge list_of_sigfiles output_dataset [ start end ] [--reflayout=...] [--select=...] [--exclude=...] [--dontFailOnUnmatched]");
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

    if (args[i] == ("--dontFailOnUnmatched")) {
      failOnUnmatched = false;
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

  yaml::Node filelist = yaml::loadFromFile(options.value("inputFiles"));

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
                                         layout, failOnUnmatched);

  // save dataset & clean up stuff
  qDebug() << "Saving dataset...";

  dataset->save(options.value("dataset"));

  qDebug() << "Dataset successfully saved!";

  // let the OS reclaim the memory itself, much faster than deleting the dataset here...
  //delete dataset;

  return 0;
}
