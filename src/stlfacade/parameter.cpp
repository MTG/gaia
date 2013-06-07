#include "parameter.h"
#include "../parameter.h"
#include "../convert.h"
using namespace std;

namespace gaia2std {

ParameterMap::ParameterMap() : _ownsData(true), _d(new ::gaia2::ParameterMap) {}

PROXY_DESTRUCTOR(ParameterMap);

void ParameterMap::insert(const string& key, const string& value) {
  _d->insert(QString::fromUtf8(key.c_str(), key.size()),
             QString::fromUtf8(value.c_str(), value.size()));
}

void ParameterMap::insert(const string& key, float value) {
  _d->insert(QString::fromUtf8(key.c_str(), key.size()), value);
}

void ParameterMap::insert(const string& key, vector<string> value) {
  _d->insert(QString::fromUtf8(key.c_str(), key.size()),
             ::gaia2::convert::VectorString_to_QStringList(value));
}

void ParameterMap::insert(const string& key, vector<float> value) {
  _d->insert(QString::fromUtf8(key.c_str(), key.size()),
             ::gaia2::convert::VectorFloat_to_RealDescriptor(value));
}


} // namespace gaia2std
