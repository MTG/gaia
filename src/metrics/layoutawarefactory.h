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
#ifndef GAIA_LAYOUTAWAREFACTORY_H
#define GAIA_LAYOUTAWAREFACTORY_H

#include <QMap>
#include "gaia.h"
#include "pointlayout.h"

namespace gaia2 {

/**
 * A tuned implementation of a factory. It is based on the generic Factory
 * class, except that it requires for all objects to have a constructor with
 * a PointLayout* (or FrozenDataSet) as first argument, and then another one
 * specified by the <code>ArgumentType</code> template argument.
 */
template < typename IdentifierType,
           typename AbstractProduct,
           typename LayoutType,
           typename ArgumentType >
class LayoutAwareFactory {

 protected:
  typedef AbstractProduct* (*ProductCreator)(const LayoutType&, const ArgumentType&);
  typedef QMap<IdentifierType, ProductCreator> CreatorMap;

  CreatorMap _map;

 public:

  static LayoutAwareFactory& instance() {
    static LayoutAwareFactory _instance;
    return _instance;
  }

  static AbstractProduct* create(const IdentifierType& id,
                                 const LayoutType& layout,
                                 const ArgumentType& arg = ArgumentType()) {

    AbstractProduct* result;
    try {
      result = instance().create_i(id, layout, arg);
    }
    catch (FixedLengthException& e) {
      // add metric name info and re-throw
      e.msg() = e.msg().arg(id);
      throw;
    }

    result->name = id;

    // in case user explicitly stated params shouldn't be checked, return the instance now
    if (result->validParams.size() == 1 && result->validParams[0] == "NO_PARAMS_CHECK") {
      return result;
    }

    // check whether the given parameters are valid before returning the new instance
    foreach (const QString& p, arg.keys()) {
      if (!result->validParams.contains(p)) {
        QStringList msg;
        msg << "Could not create algorithm " << id << " because parameter " << p
            << " is not a valid parameter.\n"
            << "Valid parameters are: " << result->validParams.join(", ");
        delete result; // clean up the object, as this is a failed initialization
        throw GaiaException(msg);
      }
    }

    return result;
  }

  AbstractProduct* create_i(const IdentifierType& id,
                            const LayoutType& layout,
                            const ArgumentType& arg) const {
    // slower than just find, but more flexible because case-insensitive (for the factory keys)
    IdentifierType lid = id.toLower();
    typename CreatorMap::const_iterator it = _map.constBegin();
    while (it != _map.constEnd() && it.key().toLower() != lid) {
      ++it;
    }
    if (it == _map.end()) {
      QStringList msg;
      msg << "Identifier '" << id << "' not found in registry...\n";
      msg << "Available keys: " << keys().join(" ");
      throw GaiaException(msg);
    }

    return (*it)(layout, arg);
  }

  static QStringList keys() { return instance()._map.keys(); }

  template <typename ConcreteProduct>
  class Registrar {
   public:
    Registrar(const IdentifierType& id) {
      LayoutAwareFactory::instance()._map.insert(id, &create);
    }

    static AbstractProduct* create(const LayoutType& layout, const ArgumentType& arg) {
      return new ConcreteProduct(layout, arg);
    }
  };

};

} // namespace gaia2

#endif // GAIA_LAYOUTAWAREFACTORY_H
