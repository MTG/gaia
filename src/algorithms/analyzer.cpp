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
#include "analyzer.h"
#include "utils.h"

// analyzers
#include "cleaner.h"
#include "normalize.h"
#include "pca.h"
#include "rca.h"
#include "remove.h"
#include "removevl.h"
#include "select.h"
#include "svmtrain.h"
#include "mergeregion.h"
#include "addfield.h"
#include "fixlength.h"
#include "enumerate.h"
#include "gaussianize.h"
#include "extract.h"
#include "center.h"
#include "inverseprojection.h"
#include "rename.h"

namespace gaia2 {

#define REGISTER(classname, name) AnalyzerFactory::Registrar<classname> register##classname(name)

/**
 * Registers all the analyzers available inside the AnalyzerFactory.
 * <b>NB:</b> For the python docstrings to work correctly, the name with which
 * the analyzer is registered needs to be the exact same name as the class name
 * (case-sensitive).
 */
void registerAnalyzers() {
  REGISTER(Cleaner, "Cleaner");
  REGISTER(Normalize, "Normalize");
  REGISTER(Remove, "Remove");
  REGISTER(RemoveVL, "RemoveVL");
  REGISTER(Select, "Select");
  REGISTER(MergeRegion, "MergeRegion");
  REGISTER(PCA, "PCA");
  REGISTER(RCA, "RCA");
  REGISTER(SVMTrain, "SVMTrain");
  REGISTER(AddField, "AddField");
  REGISTER(FixLength, "FixLength");
  REGISTER(Enumerate, "Enumerate");
  REGISTER(Gaussianize, "Gaussianize");
  REGISTER(Extract, "Extract");
  REGISTER(Center, "Center");
  REGISTER(InverseProjection, "InverseProjection");
  REGISTER(Rename, "Rename");
}


Analyzer::Analyzer(const ParameterMap& params) : _params(params) {
  // these should be made available to all analyzers (nearly all the time)
  validParams << "descriptorNames" << "except";

  // they also have default values, so a call to Analyzer::ctor should never fail
  _descriptorNames = _params.value("descriptorNames", "*").toStringList();
  _exclude = _params.value("except", QStringList()).toStringList();
}


void Analyzer::checkDataSet(const DataSet* dataset) const {
  if (!dataset) {
    throw GaiaException("Analyzer: the given dataset was a null pointer");
  }
  dataset->checkAllPointsShareSameLayout();
}

void Analyzer::checkMinPoints(const DataSet* dataset, int n) const {
  if (dataset->size() < n) {
    QStringList msg;
    msg << "Analyzer: cannot analyze dataset for " << name
        << " because it has less than " << QString::number(n) << " points";
    throw GaiaException(msg);
  }
}

  const Region& Analyzer::checkFixedLength(const Region& region, const PointLayout& layout) const {
  if (!region.isLengthTypeOnlyFrom(FixedLength)) {
    QStringList msg;
    msg << "The " << name << " transformation can only operate on fixed-length descriptors, "
        << "however the following ones are variable-length: "
        << layout.descriptorNames(region.select(UndefinedType, VariableLength)).join(", ");
    throw GaiaException(msg);
  }

  return region;
}


Transformation Analyzer::analyze(const DataSet* dataset) const {
  checkDataSet(dataset);

  const PointLayout& layout = dataset->layout();
  QStringList descs = selectDescriptors(layout, UndefinedType, _descriptorNames, _exclude);
  Region region = layout.descriptorLocation(descs);

  return analyze(dataset, region);
}


Transformation Analyzer::analyze(const DataSet* dataset, const Region& region) const {
  throw GaiaException("Analyzer: you need to reimplement either the analyze(const DataSet*) or the analyze(const DataSet*, const Region&) in your derived class");
}


} // namespace gaia2
