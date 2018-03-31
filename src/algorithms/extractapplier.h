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

/* <copyright entity="UPF">
# UPF. All Right Reserved, http://www.upf.edu/
#
# This source is subject to the Contributor License Agreement of the Essentia project.
# Please see the CLA.txt file available at http://essentia.upf.edu/contribute/
# for more
# information.
#
# THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
# KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
# PARTICULAR PURPOSE.
#
# </copyright>
*/


#ifndef GAIA_EXTRACTAPPLIER_H
#define GAIA_EXTRACTAPPLIER_H

#include "applier.h"
#include "../3rdparty/libsvm/svm.h"
#include "../utils.h"

namespace gaia2 {

class InnerDim {
public:
  InnerDim(DescriptorLengthType ltype_, int desc_, int dim_) :
    ltype(ltype_), desc(desc_), dim(dim_) {}
  DescriptorLengthType ltype;
  int desc;
  int dim;
};


/**
 * @ingroup appliers
 * Extract applier class. Extracts specified dimensions from descriptors and
 * promotes them to individual descriptors.
 */
class ExtractApplier : public Applier {

 public:
  ExtractApplier(const Transformation& transfo);
  virtual ~ExtractApplier();

  /**
   * Ownership of resulting point is handed to caller of this function.
   */
  virtual Point* mapPoint(const Point* p) const;

 protected:
  QMap<QString, QList<int> > _dims;
  PointLayout _newLayout;
  IndexMap _realMap, _stringMap, _enumMap;

  typedef QList<QPair<InnerDim, InnerDim> > Mapping;
  Mapping _mapping;

  Mapping getMappings(const PointLayout& layout) const;
  void transferData(Point* p, const Mapping& m) const;

};

} // namespace gaia2

#endif // GAIA_EXTRACTAPPLIER_H
