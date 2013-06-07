#include "gaia.h"
#include "dataset.h"
#include "parameter.h"
#include "view.h"
#include "parser/filter.h"
using namespace std;
using namespace gaia2;
using namespace gaia2::parser;


ParameterMap parseOptions(int argc, char* argv[]) {
  if (argc < 3) {
    qFatal("ERROR: not enough arguments...\n\n\
Usage: %s dataset id_of_query [ where_clause ]", argv[0]);
  }

  ParameterMap options;
  options.insert("dataset", argv[1]);
  options.insert("id", argv[2]);

  if (argc == 4) options.insert("where", argv[3]);

  return options;
}

int main(int argc, char* argv[]) {
  gaia2::init();
  ParameterMap options = parseOptions(argc, argv);

  DataSet dataset;
  dataset.load(options.value("dataset"));
  const PointLayout& layout = dataset.layout();

  // test predicate on first 10 songs
  /*
  FilterParser parser;
  Filter* filter = parser.parse(options.value("where"));
  filter->bindToLayout(layout);

  Collection& c = *dataset.collection(dataset.collectionNames().at(0));
  qDebug() << "Where clause is:" << options.value("where");
  for (int i=0; i<10; i++) {
    qDebug() << "Point" << c[i]->name << "is" << filter->isTrue(c[i])
             << "- actual BPM is" << c[i]->realValue("tempotap_bpm.mean")
             << " and Key is" << c[i]->label("key_key");
  }
  */

  // create default region that includes all the real type descriptors
  Region region = layout.descriptorLocation(layout.descriptorNames(RealType, "*"));

  ParameterMap euclidParams, manhattanParams, mixedParams;

  euclidParams.insert("name", "euclidean");
  euclidParams.insert("params", ParameterMap());
  euclidParams.insert("weight", 1.0);

  manhattanParams.insert("name", "manhattan");
  manhattanParams.insert("params", ParameterMap());
  manhattanParams.insert("weight", 1.0);

  mixedParams.insert("euclid", euclidParams);
  mixedParams.insert("manhattan", manhattanParams);

  /*
  LinearCombinationDistance* mixed = new LinearCombinationDistance;
  mixed->addDistance(new EuclideanDistance(region), 1.0);
  mixed->addDistance(new ManhattanDistance(region), 1.0);
  */
  DistanceFunction* mixed = MetricFactory::create("LinearCombination", layout, mixedParams);

  // create euclidean distance
  View view(&dataset);

  // compute the nearest-neighbors
  SearchResults neighbors;
  if (options.contains("where")) {
    neighbors = view.nnSearch(options.value("id"), mixed, options.value("where")).get(5);
  }
  else {
    neighbors = view.nnSearch(options.value("id"), mixed).get(5);
  }

  qDebug() << neighbors;

  return 0;
}
