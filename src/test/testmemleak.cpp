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
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see http://www.gnu.org/licenses/
 */
#include "dataset.h"
#include "view.h"
#include "distancefunction.h"
#include "analyzer.h"
#include <iostream>
using namespace gaia2;
using namespace std;

const char* TEST_DB = "test/unittest/data/dataset_small.db";

void testSelectTransform() {
  DataSet* ds = new DataSet(), *ds_new;
  ds->load(TEST_DB);

  ParameterMap params;
  params.insert("descriptorNames", "*");
  Analyzer* select = AnalyzerFactory::create("select", params);

  for (int i=0; i<10; i++) {
    ds_new = select->analyze(ds).applyToDataSet(ds);
    delete ds;
    ds = ds_new;
  }

  delete select;
  delete ds;
}


PointLayout createLayout() {
  PointLayout l;
  l.add("c", RealType);
  l.add("a", RealType);
  l.add("b", RealType);
  l.add("d", StringType);
  l.add("a", "1", RealType);
  l.add("a", "2", RealType);
  return l;
}


DataSet* createDataSet() {
  PointLayout l = createLayout();

  DataSet* d = new DataSet();
  Point p;
  p.setName("p");
  p.setLayout(l);

  d->addPoint(&p);

  return d;
}

// taken from testdataset.py
void testLinkedViews() {
  DataSet* dsorig = createDataSet();

  dsorig->point("p")->setValue("a.1", RealDescriptor(1, 23.0));

  for (int i=0; i<5; i++) {
    Point p;
    p.setName(QString("p%1").arg(i));
    p.setLayout(dsorig->layout());
    p.setValue("a.1", float(i));
    dsorig->addPoint(&p);
  }

  DataSet* ds = transform(dsorig, "fixlength");
  View v(ds);
  View v2(ds);

  Point query = Point(*dsorig->point("p"));
  ds->removePoint("p");
  ds->removePoint("p3");
  ds->addPoint(&query);

  delete ds;
  delete dsorig;
}

// taken from testdataset.py
void testHistory() {
  DataSet* d = createDataSet();

  ParameterMap params;
  params.insert("descriptorNames", QStringList() << "a*" << "b*");
  Analyzer* sel = AnalyzerFactory::create("select", params);
  DataSet* ds = sel->analyze(d).applyToDataSet(d);
  delete sel;

  Point p;
  p.setName("ptest");
  p.setLayout(createLayout());

  ds->addPoint(&p);

  delete ds;
  delete d;
}

void testIndexing() {
  DataSet* orig = new DataSet;
  orig->load(TEST_DB);

  DataSet* ds = transform(orig, "fixlength");
  delete orig;

  ds->setReferenceDataSet(ds);

  ParameterMap params;
  params.insert("descriptorNames", "*.mean");
  DistanceFunction* dist = MetricFactory::create("euclidean", ds->layout(), params);
  View v(ds);

  v.indexOn("tempotap_bpm");

  const Point& p = *ds->samplePoint();
  ResultSet rs = v.nnSearch(p, dist, "where true");
  rs = v.nnSearch(p, dist, "where value.tempotap_bpm > 140");
  v.nnSearch(p, dist).get(10);

  ResultSet rs2 = v.nnSearch(p, dist, "where value.tempotap_bpm between 120 and 130");
  ResultSet rs3 = v.nnSearch(p, v.nnSearch(p, dist, "where value.tempotap_bpm <= 130"),
                             dist, "where value.tempotap_bpm >= 120");


  // some more tests so that valgrind can catch memory errors
  v.nnSearch(p, dist, "WHERE value.tempotap_bpm > -10");
  v.nnSearch(p, dist, "WHERE value.tempotap_bpm < -10");
  v.nnSearch(p, dist, "WHERE value.tempotap_bpm > 23000");
  v.nnSearch(p, dist, "WHERE value.tempotap_bpm < 23000");
  v.nnSearch(p, dist, "WHERE value.tempotap_bpm BETWEEN 120 AND 120");
  v.nnSearch(p, dist, "WHERE value.tempotap_bpm BETWEEN -2.3 AND 4096");
  v.nnSearch(p, dist, "WHERE value.tempotap_bpm BETWEEN -2.3 AND -1.4");
  v.nnSearch(p, dist, "WHERE value.tempotap_bpm BETWEEN 2048 AND 4096");

  if (rs2.size() != rs3.size()) {
    qDebug() << "ERROR: rs2.size() =" << rs2.size() << "and rs3.size() =" << rs3.size();
  }

  delete dist;
  delete ds;
}

void testQueries() {
  DataSet* orig = new DataSet;
  orig->load(TEST_DB);

  DataSet* ds = transform(orig, "fixlength");
  delete orig;

  ParameterMap params;
  params.insert("descriptorNames", "*.mean");
  DistanceFunction* dist = MetricFactory::create("euclidean", ds->layout(), params);
  View v(ds);
  Point q = *ds->samplePoint();

  for (int i=0; i<1000; i++) {
    v.nnSearch(q, dist).get(5);
  }

  delete ds;
  delete dist;
}

DataSet* loadDB(const QString& filename) {
  DataSet* ds = new DataSet();
  ds->load(filename);

  return ds;
}

DataSet* loadTransformDB(const QString& filename) {
  DataSet* ds = loadDB(filename);
  /*
  DataSet* ds2 = transform(ds1, "removevl"); delete ds1;
  ds1 = transform(ds2, "fixlength"); delete ds2;
  ds2 = transform(ds1, "cleaner"); delete ds1;
  ds1 = transform(ds2, "normalize"); delete ds2;
  ParameterMap params;
  params.insert("dimension", 15);
  params.insert("resultName", "pca15");
  ds2 = transform(ds1, "pca", params); delete ds1;
  return ds2;
  */
  DataSet* result = applyTransfoChain(ds,
                                      QString("- transfo: removevl\n"
                                              "- transfo: fixlength\n"
                                              "- transfo: cleaner\n"
                                              "- transfo: normalize\n"
                                              "-\n"
                                              "  transfo: pca\n"
                                              "  params: { dimension: 15, resultName: pca15 }"));

  delete ds;
  return result;
}

void testTransfo() {
  for (int i=0; i<5; i++) {
    DataSet* ds = loadTransformDB(TEST_DB);
    delete ds;
  }
}

void testViewSearch() {
  DataSet* ds = loadTransformDB(TEST_DB);

  for (int i=0; i<1000; i++) {
    DistanceFunction* euclidean = MetricFactory::create("euclidean", ds->layout());
    View v(ds);
    SearchResults results = v.nnSearch(*(ds->samplePoint()), euclidean).get(10);
    delete euclidean;
  }

  delete ds;
}

void testEnumerate() {
  DataSet* ds = loadDB(TEST_DB);

  ParameterMap params;
  params.insert("descriptorNames", ds->layout().descriptorNames(StringType));
  DataSet* dse = transform(ds, "enumerate", params);

  delete dse;

  delete ds;
}

int main() {
  gaia2::init();
  gaia2::verbose = false;

  testSelectTransform();
  testLinkedViews(); // leaks a few bytes by the lemon parser
  testHistory();
  testIndexing();
  testQueries();
  testTransfo();
  testViewSearch();

  gaia2::shutdown();

  return 0;
}
