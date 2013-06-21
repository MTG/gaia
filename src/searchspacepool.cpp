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
#include "searchspacepool.h"

namespace gaia2 {

// static variable for normal SearchSpaces
template<>
QMutex* SearchSpacePool::_sspoolMutex = 0;

template<>
QList<SearchSpace*> SearchSpacePool::_sspool = QList<SearchSpace*>();


// static variable for frozen SearchSpaces
template<>
QMutex* FrozenSearchSpacePool::_sspoolMutex = 0;

template<>
QList<FrozenSearchSpace*> FrozenSearchSpacePool::_sspool = QList<FrozenSearchSpace*>();



} // namespace gaia2
