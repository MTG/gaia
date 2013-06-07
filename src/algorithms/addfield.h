#ifndef GAIA_ADDFIELD_H
#define GAIA_ADDFIELD_H

#include "analyzer.h"

namespace gaia2 {

/**
 * @ingroup analyzers datasetmanagementalgos
 *
 * The AddField analyzer allows you to add new fields in a layout of an already
 * existing DataSet. By default, the real fields will have a value of 0 and the
 * string fields will have a value of "".
 * @param real the names of the fields of type real to be added.
 * @param string the names of the fields of type string to be added.
 * @param default a map containing the (optional) default values for the new
 *                fields to be added.
 * @param size a map containing the (optional) default size for the new fields
 *             to be added. If a field is specified here, it will be added as a
 *             fixed-length field in the layout, otherwise it will be a
 *             variable-length field.
 */
class AddField : public Analyzer {

 public:

  AddField(const ParameterMap& params);
  virtual ~AddField() {};

  Transformation analyze(const DataSet* data) const;

 protected:
  QStringList _real;
  QStringList _string;
  ParameterMap _default;
  ParameterMap _size;
};

} // namespace gaia2

#endif // GAIA_ADDFIELD_H
