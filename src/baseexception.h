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
#ifndef GAIA_BASEEXCEPTION_H
#define GAIA_BASEEXCEPTION_H

#include <exception>
#include <string>

namespace gaia2std {

class GException : public std::exception {
 protected:
  std::string _message;

 public:
  GException() : exception() {}
  GException(const char* msg) : exception(), _message(msg) {}
  GException(const std::string& msg) : exception(), _message(msg) {}

  virtual ~GException() throw() {}
  virtual const char* what() const throw() { return _message.c_str(); }

  const std::string& message() const throw() { return _message; }
};

} // namespace gaia2std

#endif // GAIA_BASEEXCEPTION_H
