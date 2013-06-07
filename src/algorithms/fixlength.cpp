#include "fixlength.h"
#include "removevl.h"
#include "algoutils.h"

namespace gaia2 {

FixLength::FixLength(const ParameterMap& params) : Analyzer(params) {
  _default = false;
  try {
    _descriptorNames = params.value("descriptorNames").toStringList();
  }
  catch (GaiaException&) {
    _default = true;
  }
  _exclude = params.value("except", QStringList()).toStringList();
}


Transformation FixLength::analyze(const DataSet* dataset) const {
  G_INFO("Doing fixlength analysis...");
  checkDataSet(dataset);

  QStringList selected, excluded;
  if (_default) {
    // if we didn't specify descriptorNames, we want the default behavior which
    // is to fix the length of all the descriptors which have the same size in all
    // points in this dataset
    selected = dataset->layout().descriptorNames();
    excluded = findVariableLengthDescriptors(dataset);
  }
  else { // !_default
    // only fix the length of the specified descriptors
    selected = dataset->layout().descriptorNames(UndefinedType, _descriptorNames);
    excluded = dataset->layout().descriptorNames(UndefinedType, _exclude);
  }

  // substract one from the other to get final list of desired descriptors
  foreach (QString name, excluded) {
    selected.removeAll(name);
  }
  selected.sort();


  Transformation result(dataset->layout());
  result.analyzerName = "fixlength";
  result.analyzerParams = _params;
  result.applierName = "fixlengthapplier";

  ParameterMap params;
  params.insert("descriptorNames", selected);

  result.params = params;

  return result;
}


} // namespace gaia2
