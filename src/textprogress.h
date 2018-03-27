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


#ifndef GAIA_TEXTPROGRESS_H
#define GAIA_TEXTPROGRESS_H

#include <iostream>

namespace gaia2 {

enum UpdateType {
  UpdateOnUnit,
  UpdateOnPercent
};

/**
 * This class is a very simple class which allows you to print a
 * progress percentage on the standard output. It is initialized
 * with the value to be reached (which represents 100%) and then
 * your only task is to feed it with the progress value and it
 * will display itself correctly.
 */
class TextProgress {

 protected:
  QString _display;
  int _total, _current, _previous;
  int _fieldWidth;
  UpdateType _updateType;
  bool _silent;

 public:
  TextProgress(int total, QString format = "[%1/%2] (%3% done)...", UpdateType t = UpdateOnPercent) :
    _total(total), _current(0), _previous(-total), _updateType(t), _silent(false) {
    _fieldWidth = QString::number(total).size();
    _display = QChar('\r') + format;
  }

  int total() const { return _total; }
  void setTotal(int total) {
    _total = total;
    _fieldWidth = QString::number(total).size();
  }


  /**
   * Updates the counter, ie: displays its state on the standard output.
   */
  inline void update() {
    if (_silent) return;
    switch (_updateType) {
    case UpdateOnPercent:
      if (_current*100/_total == _previous*100/_total) return;
      break;
    case UpdateOnUnit:
      if (_current == _previous) return;
      break;
    }

    _previous = _current;
    int percent = _current*100/_total;
    std::cout << _display.arg(_current, _fieldWidth).arg(_total).arg(percent) << std::flush;
    if (percent == 100) std::cout << std::endl;
  }

  /**
   * Updates the counter to given value, then display its state on
   * the standard output.
   * @param value the value of the counter.
   */
  inline void update(int value) {
    _current = value;
    update();
  }

  inline void setSilent(bool silent) {
    _silent = silent;
  }

  inline bool isSilent() {
    return _silent;
  }
};

} // namespace gaia2

#endif // GAIA_TEXTPROGRESS_H
