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

#ifndef GAIA_ANALYZER_H
#define GAIA_ANALYZER_H

#include "../dataset.h"
#include "../factory.h"
#include "../transformation.h"

namespace gaia2 {

/**
 * @ingroup analyzers
 * the Analyzer abstract base class.
 *
 * If you want to implement a specific analyzer, you need to inherit from this
 * base class and implement either one of the 2 analyze methods.
 *
 * Analyzer::analyze(DataSet*) just gives you the DataSet and you're good to go,
 * whereas Analyzer::analyze(DataSet*, Region&) gives you an already parsed Region
 * of descriptors as well (got from the @c descriptorNames and @c exclude parameter.
 *
 * @NB: if you use the analyze(DataSet*) method, you should also call the checkDataSet()
 *      method to make sure you were not given a null pointer!
 */
class Analyzer {

 public:

  /**
   * Name for the algorithm, usually the key that was used to instantiate it
   * from the factory.
   */
  QString name;

  Analyzer(const ParameterMap& params);
  virtual ~Analyzer() {};

  virtual Transformation analyze(const DataSet* dataset) const;
  virtual Transformation analyze(const DataSet* dataset, const Region& region) const;

  /**
   * Checks that the given dataset is valid. At the moment, it does:
   *  - checks that it is not a null pointer
   *  - checks that all the points share the same layout memory object
   * @throw GaiaException if there was a problem with the dataset.
   */
  void checkDataSet(const DataSet* dataset) const;

  /**
   * Checks that the given dataset as at least the specified number of points.
   * @throw GaiaException if that is not the case.
   */
  void checkMinPoints(const DataSet* dataset, int n) const;

  /**
   * Checks that the given Region only contains fixed-length descriptors and throws
   * an exception if not.
   * @throw GaiaException if the region contained variable-length descriptors.
   */
  const Region& checkFixedLength(const Region& region, const PointLayout& layout) const;

  /**
   * List of valid parameters this analyzer accepts. It needs to be filled in
   * the constructor of the derived analyzer, and will be checked by the factory
   * just after instantiation.
   */
  QStringList validParams;

 protected:
  ParameterMap _params;
  QStringList _descriptorNames, _exclude;

};


typedef ParamCheckingFactory<QString, Analyzer, ParameterMap> AnalyzerFactory;

void registerAnalyzers();


} // namespace gaia2

#endif // GAIA_ANALYZER_H
