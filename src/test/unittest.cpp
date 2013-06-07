#include <QDataStream>
#include <QBuffer>
#include <QByteArray>
#include "pointlayout.h"
#include "descriptor.h"
#include "dataset.h"
#include "utils.h"
using namespace gaia2;

const char* TEST_DB = "test/unittest/data/dataset_small.db";

void testLayout() {

  //SharedPointer<PointLayout> p1(new PointLayout);
  PointLayout p1;

  p1.add("a", RealType);
  p1.add("b", RealType);
  p1.add("c", RealType);
  p1.add("d", StringType);

  PointLayout p2 = p1;
  PointLayout p3 = p2.copy();

  p3.remove("b");

  Q_ASSERT(p1.regionSize(RealType, VariableLength) == 3);
  Q_ASSERT(p2.regionSize(RealType, VariableLength) == 3);
  Q_ASSERT(p3.regionSize(RealType, VariableLength) == 2);

  QBuffer* buffer = new QBuffer;
  buffer->open(QBuffer::ReadWrite);

  QDataStream out(buffer);
  setDataStreamVersion(out);
  out << p1;

  PointLayout p4;
  buffer->seek(0);
  checkValidDataStream(out);
  out >> p4;

  Q_ASSERT(p1 == p4);

  //delete p1;
}

void testDescriptor() {
  RealDescriptor x(0, 0.0);         x << 1.0 << 2.0 << 3.0;
  RealDescriptor y(0, 0.0);         y <<   3 <<   2 <<   1;
  RealDescriptor diff(0, 0.0);   diff <<  -2 <<   0 <<   2;
  RealDescriptor sum_2(0, 0.0); sum_2 <<   3 <<   4 <<   5;

  Q_ASSERT(x-y == diff);
  Q_ASSERT(x + 2 == sum_2);
}

void testRef() {
  DataSet ds;
  Q_ASSERT(ds.layout().ref() == 1);

  Point p;
  p.setName("p1");

  PointLayout lext(p.layout());
  Q_ASSERT(lext.ref() == 2);

  lext = p.layout().copy();
  Q_ASSERT(lext.ref() == 1);

  ds.addPoint(&p);

  Q_ASSERT(lext.ref() == 1);
  Q_ASSERT(ds.layout().ref() == 3); // +2 (dataset + pointcopy), ref = 3

  Point p2(p);
  p2.setName("p2");
  Q_ASSERT(ds.layout().ref() == 4);

  ds.addPoint(&p2);
  Q_ASSERT(ds.layout().ref() == 5);
}

void testMatrix() {
  // FIXME: implement autoconversion from RealDescriptor to Eigen::Matrix
  /*
  RealDescriptor x(0, 0.0); x << 2 << 3;
  x << 1 << 2 << 3 << 4 << 5 << 6;
  Eigen::MatrixXf m = x;

  Q_ASSERT(m.rows() == 2);
  Q_ASSERT(m.cols() == 3);
  Q_ASSERT(m[1][1] == 5);
  Q_ASSERT(m[0][2] == 3);
  */
}

void testDataSetCopy() {
  DataSet* ds = new DataSet();
  ds->load(TEST_DB);

  DataSet* ds2 = ds->copy();

  delete ds;
  delete ds2;
}

int main() {
  testLayout();
  testDescriptor();
  testRef();
  testMatrix();
  testDataSetCopy();

  //qDebug() << "ended without problems";

  return 0;
}
