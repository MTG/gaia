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
