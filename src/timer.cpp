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


#include "timer.h"
using namespace gaia2;

#ifndef OS_WIN32

Timer::Timer() {
  _begin.tv_sec = _begin.tv_usec = 0;
  _end.tv_sec = _end.tv_usec = 0;
  _total = 0.0;
  _isRunning = false;
}

void Timer::start() {
  _isRunning = true;
  gettimeofday(&_begin, 0);
}

void Timer::reset() {
  _total = 0.0;
  _isRunning = false;
}

void Timer::stop() {
  if (!_isRunning) return;

  gettimeofday(&_end, 0);
  _isRunning = false;
  _total += diff(_end, _begin);

}

double Timer::elapsed() const {
  // can only measure time when timer is stopped
  if (!_isRunning) return _total;

  // if timer was running, measure time online
  struct timeval end;
  gettimeofday(&end, 0);

  return _total + diff(end, _begin);
}

#endif

