#include "dataset.h"
#include "../dataset.h"
#include "../convert.h"
using namespace std;

namespace gaia2std {


DataSet::DataSet() : _ownsData(true), _d(new ::gaia2::DataSet)  {}

PROXY_DESTRUCTOR(DataSet);

string DataSet::name() const {
  return _d->name().toUtf8().data();
}

void DataSet::setName(const string& name) {
  _d->setName(QString::fromUtf8(name.c_str(), name.size()));
}

void DataSet::setReferenceDataSet(DataSet* dataset) {
  if (!dataset) {
    throw gaia2std::GException("Reference data set is null");
  }
  _d->setReferenceDataSet(dataset->_d);
}

vector<string> DataSet::pointNames() const {
  return ::gaia2::convert::QStringList_to_VectorString(_d->pointNames());
}

proxy_ptr<const Point> DataSet::point(const string& id) const {
  const ::gaia2::Point* p = _d->point(QString::fromUtf8(id.c_str(), id.size()));
  return proxy_ptr<const Point>(new Point(p));
}

proxy_ptr<Point> DataSet::point(const string& id) {
  ::gaia2::Point* p = _d->point(QString::fromUtf8(id.c_str(), id.size()));
  return proxy_ptr<Point>(new Point(p));
}


void DataSet::addPoint(Point* point) {
  assert(point->_ownsData == true);
  _d->addPoint(point->impl());
  point->_ownsData = false;
}

void DataSet::removePoint(const string& id) {
  _d->removePoint(QString::fromUtf8(id.c_str(), id.size()));
}

bool DataSet::empty() const {
  return _d->empty();
}

int DataSet::size() const {
  return _d->size();
}

void DataSet::load(const string& filename) {
  _d->load(QString::fromUtf8(filename.c_str(), filename.size()));
}

void DataSet::save(const string& filename) {
  _d->save(QString::fromUtf8(filename.c_str(), filename.size()));
}


} // namespace gaia2std
