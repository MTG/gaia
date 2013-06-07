#define PARAMETERMAP_DEFINED
#include "../parameter.h"
#include "analyzer.h"
#include "parameter.h"
#include "../algorithms/analyzer.h"
#include "../transformation.h"
using namespace std;

namespace gaia2std {

Analyzer::Analyzer(const string& name, const ParameterMap& params) :
  _d(::gaia2::AnalyzerFactory::create(QString::fromUtf8(name.c_str(), name.size()),
                                     *params.impl())) {}

PROXY_DESTRUCTOR(Analyzer);


proxy_ptr<Transformation> Analyzer::analyze(const DataSet* data) const {
  ::gaia2::Transformation* t = new ::gaia2::Transformation(_d->analyze(data->impl()));
  return proxy_ptr<Transformation>(new Transformation(t));
}


} // namespace gaia2std
