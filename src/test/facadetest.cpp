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

#include <iostream>
#include <cstdlib>
#include "../stlfacade/gaia2std.h"
#include "../stlfacade/dataset.h"
#include "../stlfacade/parameter.h"
#include "../stlfacade/analyzer.h"
#include "../stlfacade/streamutil.h" // for cout << vector<string> & friends
#include "../stlfacade/distancefunction.h"
#include "../stlfacade/view.h"
using namespace std;
using namespace gaia2std;


const char* TEST_DATABASE = "dataset_small.db";


int main() {
  init();

  proxy_ptr<DataSet> ds(new DataSet);
  ds->load(TEST_DATABASE);

  assert(ds->point("06 Booo!.mp3")->label("chords_mode")[0] == "major");

  cout << "boo:chords_mode = " << ds->point("06 Booo!.mp3")->label("chords_mode")[0] << endl;

  ParameterMap noParams;

  Analyzer remove("removevl", noParams);
  proxy_ptr<DataSet> dsr = remove.analyze(ds)->applyToDataSet(ds);

  Analyzer fixlength("fixlength", noParams);
  proxy_ptr<DataSet> dsf = fixlength.analyze(dsr)->applyToDataSet(dsr);

  // apply cleaner transformation
  Analyzer cleaner("cleaner", noParams);
  proxy_ptr<DataSet> dsc = cleaner.analyze(dsf)->applyToDataSet(dsf);

  // let's see what happens when we free the original dataset:
  ds = proxy_ptr<DataSet>(); // set it to null pointer, should remove last ref to dataset

  // try some normalization
  proxy_ptr<DataSet> dsn;
  try {
    Analyzer norm("normalize", noParams);
    dsn = norm.analyze(dsc)->applyToDataSet(dsc);
  }
  catch (GException& e) {
    cout << "Caught gaia2std exception:\n" << e.what() << endl;
    exit(1);
  }

  cout << dsn->point("06 Booo!.mp3")->value("tempotap_bpm.value") << endl;
  cout << dsn->point("06 Booo!.mp3")->value("tempotap_bpm") << endl;
  cout << dsn->point("06 Booo!.mp3")->label("chords_mode") << endl;

  // PCA
  ParameterMap pcaParams;
  pcaParams.insert("resultName", "pca");
  pcaParams.insert("dimension", 30);
  vector<string> dnames;
  dnames.push_back("*.mean");
  dnames.push_back("*.var");
  pcaParams.insert("descriptorNames", dnames);

  Analyzer pca("pca", pcaParams);
  proxy_ptr<DataSet> pca30ds = pca.analyze(dsn)->applyToDataSet(dsn);

  //------------------------------------------------------------------------

  // create a DF on dataset
  DistanceFunction euclidean("euclidean", pca30ds, noParams);

  // create the associated view
  View v(pca30ds);

  string id = "06 Booo!.mp3";
  cout << "looking for point with id: " << id << endl;

  SearchResults results = v.nnSearch(id, euclidean, 10);

  cout << results << endl;

  return 0;
}
