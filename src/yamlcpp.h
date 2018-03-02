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

#ifndef YAMLCPP_H
#define YAMLCPP_H

#include <algorithm> // for lexicographical_compare
#include <QList>
#include <QMap>
#include <QString>
#include <QTextStream>
#include "gaia.h"

namespace gaia2 {
namespace yaml {
class Node;
}
}

//std::ostream& operator<<(std::ostream& out, const gaia2::yaml::Node& node);


namespace gaia2 {
namespace yaml {

/**
 * YamlException that offers the the same functionality as GaiaException but
 * is a different class to allow specific filtering on it.
 */
GAIA_DEFINE_EXCEPTION(YamlException);


/**
 * Possible types in the Yaml specification.
 */
typedef enum {
  ScalarType,
  SequenceType,
  MappingType
} NodeType;


// Mappings used from the Yaml types to our C++ types; use Qt's classes.
typedef QString Scalar;
typedef QList<Node> Sequence;
typedef GaiaMap<Node, Node, YamlException> Mapping;


// should probably be implemented like this
/*
class YamlNode;
class ScalarTypeNode : public YamlNode;
class SequenceTypeNode : public YamlNode;
class MappingTypeNode : public YamlNode;
*/
// or even better, just as a QVariant, and that would also remove the hack for
// knowing whether a scalar is quoted or not

#define yamlCheckType(input_type) if (type() != input_type##Type) throw YamlException("Type is not a "#input_type)

class Node {
 protected:
  NodeType _type;

  Scalar _scalar;
  Sequence _sequence;
  Mapping _mapping;

 public:

  Node() : _type(ScalarType), _scalar("") {}
  Node(const QString& str) : _type(ScalarType), _scalar(str) {}
  Node(const char* str) : _type(ScalarType), _scalar(QString::fromUtf8(str)) {}
  Node(const std::string& str) : _type(ScalarType), _scalar(QString::fromUtf8(str.c_str(), (int)str.size())) {}
  Node(const Sequence& seq) : _type(SequenceType), _sequence(seq) {}
  Node(const Mapping& mapping) : _type(MappingType), _mapping(mapping) {}

  Node(const Node& n) : _type(n._type),
                        _scalar(n._scalar),
                        _sequence(n._sequence),
                        _mapping(n._mapping) {}


  QString toString() const {
    return scalar();
  }

  /*
  template<typename T> const T as() const {
    //return boost::lexical_cast<T>(asString().toLatin1());
    return T();
  }
  */

  operator QString() const {
    return toString();
  }

  bool operator==(const Node& rhs) const {
    if (_type != rhs._type) return false;

    switch (_type) {
    case ScalarType: return _scalar == rhs._scalar;
    case SequenceType: return _sequence == rhs._sequence;
    case MappingType: return _mapping == rhs._mapping;
    }

    // weird, g++ complains if there's no return value here...
    return false;
  }

  /**
   * @todo revise me
   */
  bool operator<(const Node& rhs) const {
    if (_type != rhs._type) return _type < rhs._type;

    switch (_type) {
    case ScalarType: return _scalar < rhs._scalar;
    case SequenceType:
      return std::lexicographical_compare(_sequence.begin(), _sequence.end(),
                                          rhs._sequence.begin(), rhs._sequence.end());
    case MappingType:
      return std::lexicographical_compare(_mapping.begin(), _mapping.end(),
                                          rhs._mapping.begin(), rhs._mapping.end());
    }

    // weird, g++ complains if there's no return value here...
    return false;
  }

  NodeType type() const { return _type; }

  // ---------------- Conversion functions ---------------- //

  const Scalar& scalar() const {
    yamlCheckType(Scalar);
    return _scalar;
  }

  Scalar& scalar() {
    yamlCheckType(Scalar);
    return _scalar;
  }

  const Sequence& sequence() const {
    yamlCheckType(Sequence);
    return _sequence;
  }

  Sequence& sequence() {
    yamlCheckType(Sequence);
    return _sequence;
  }

  const Mapping& mapping() const {
    yamlCheckType(Mapping);
    return _mapping;
  }

  Mapping& mapping() {
    yamlCheckType(Mapping);
    return _mapping;
  }

  // ---------------- Shortcut convenience functions ---------------- //

  const Node& operator[](int i) const {
    return sequence()[i];
  }

  Node& operator[](int i) {
    return sequence()[i];
  }

  const Node& operator[](const Node& node) const {
    return mapping()[node];
  }

  Node& operator[](const Node& node) {
    return mapping()[node];
  }


  friend std::ostream& operator<<(std::ostream& out, const gaia2::yaml::Node& node);

};


/**
 * Global function for the namespace that reads a string and returns a Node
 * instance that represents the data.
 * You can pass it a parameter, markQuotedScalars, which if it is true, will
 * add a char '\0' in front of all the scalars that have been quoted.
 */
Node loadFromString(const std::string& str, bool markQuotedScalars = false);

Node load(const char* str, uint size, bool markQuotedScalars = false);

Node load(const QString& str);

QByteArray dump(const Node& node);

/**
 * This function loads a yaml file and returns the corresponding yaml node.
 * You can pass it a parameter, markQuotedScalars, which if it is true, will
 * add a char '\0' in front of all the scalars that have been quoted.
 */
Node loadFromFile(const QString& filename, bool markQuotedScalars = false);


inline Stringifier& operator<<(Stringifier& out, const Node& node) {
  switch (node.type()) {
  case yaml::ScalarType: return out << node.scalar();
  case yaml::MappingType: return out << node.mapping();
  case yaml::SequenceType: return out << node.sequence();
  }

  return out << "Unknown Type";
}

} // namespace yaml
} // namespace gaia2


#endif // YAMLCPP_H
