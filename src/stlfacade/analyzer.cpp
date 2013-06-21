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
