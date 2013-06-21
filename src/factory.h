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
#ifndef GAIA_FACTORY_H
#define GAIA_FACTORY_H

#include <QMap>
#include "gaia.h"

namespace gaia2 {

/**
 * A basic implementation of a generic factory class. We can choose the
 * identifier type (usually string) and the base type that we want to
 * implement. Registration is done via the creation of a static object of
 * type Factory<IdentifierType, AbstractProduct>::Registrar<ConcreteProduct>.
 * There is also a third template parameter that can be used, the type of
 * an optional argument. That means we will be able to call:
 * Factory::create("PCA", "dim=3");
 *
 */
template < typename IdentifierType,
           typename AbstractProduct,
           typename ArgumentType >
class Factory {

 protected:
  typedef AbstractProduct* (*ProductCreator)(const ArgumentType&);
  typedef QMap<IdentifierType, ProductCreator> CreatorMap;

  CreatorMap _map;

 public:

  static Factory& instance() {
    static Factory _instance;
    return _instance;
  }

  static AbstractProduct* create(const IdentifierType& id,
                                 const ArgumentType& arg = ArgumentType()) {
    return instance().create_i(id, arg);
  }

  AbstractProduct* create_i(const IdentifierType& id, const ArgumentType& arg) const {
    // slower than just find, but more flexible because case-insensitive (for the factory keys)
    IdentifierType lid = id.toLower();
    typename CreatorMap::const_iterator it = _map.constBegin();
    while (it != _map.constEnd() && it.key().toLower() != lid) {
      ++it;
    }
    if (it == _map.constEnd()) {
      QStringList msg;
      msg << "Identifier '" << id << "' not found in registry...\n";
      msg << "Available keys: " << keys().join(" ");
      throw GaiaException(msg);
    }

    return (*it)(arg);
  }

  static QStringList keys() { return instance()._map.keys(); }

  template <typename ConcreteProduct>
  class Registrar {
   public:
    Registrar(const IdentifierType& id) {
      Factory::instance()._map.insert(id, &create);
    }

    static AbstractProduct* create(const ArgumentType& arg) {
      return new ConcreteProduct(arg);
    }
  };

};


template < typename IdentifierType,
           typename AbstractProduct,
           typename ArgumentType >
class ParamCheckingFactory : public Factory<IdentifierType, AbstractProduct, ArgumentType> {

 public:
  typedef Factory<IdentifierType, AbstractProduct, ArgumentType> BaseFactory;

  static ParamCheckingFactory& instance() {
    return static_cast<ParamCheckingFactory&>(BaseFactory::instance());
  }

  static AbstractProduct* create(const IdentifierType& id,
                                 const ArgumentType& arg = ArgumentType()) {

    AbstractProduct* result;
    try {
      result = BaseFactory::instance().create_i(id, arg);
    }
    catch (FixedLengthException& e) {
      e.msg() = e.msg().arg(id);
      throw;
    }
    catch (GaiaException& e) {
      QStringList msg;
      msg << "Could not instantiate " << id << " analyzer because: " << e.msg();
      throw GaiaException(msg);
    }

    result->name = id;

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

};



} // namespace gaia2

#endif // GAIA_FACTORY_H
