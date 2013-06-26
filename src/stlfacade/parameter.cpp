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
