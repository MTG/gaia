#include "../dataset.h"
#include "../metrics/distancefunction.h"
#include "../algorithms/analyzer.h"
#include "../view.h"
using namespace gaia2;

const char* TEST_DB = "test/unittest/data/dataset_small.db";

void testNNSearch() {
  DataSet* ds = new DataSet();
  ds->load(TEST_DB);

  DataSet* ds_novl = transform(ds, "RemoveVL");
  DataSet* ds_fl = transform(ds_novl, "FixLength");

  QString pid = ds->samplePoint()->name();

  DistanceFunction* df = MetricFactory::create("euclidean", ds_fl->layout());
  View v(ds_fl);

  for (int i=0; i<10000; i++) {
    v.nnSearch(pid, df).get(10);
  }

  delete ds;
  delete ds_novl;
  delete ds_fl;
}

void testTransfoChain() {
  DataSet* ds = new DataSet();
  ds->load(TEST_DB);


  DataSet* ds_novl = transform(ds, "RemoveVL");
  DataSet* ds_fl = transform(ds_novl, "FixLength");

  DataSet* ds_norm = transform(ds_fl, "Normalize");

  delete ds;
  delete ds_novl;
  delete ds_norm;
}


int main() {
  gaia2::init();
  gaia2::verbose = false;

#ifdef NDEBUG
  qDebug() << "NDEBUG defined";
#else
  qDebug() << "NDEBUG not defined";
#endif

#ifdef QT_NO_DEBUG
  qDebug() << "QT_NO_DEBUG defined";
#else
  qDebug() << "QT_NO_DEBUG not defined";
#endif

  testNNSearch();
  testTransfoChain();
  return 0;
}
