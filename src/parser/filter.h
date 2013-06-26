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

#ifndef GAIA_PARSER_FILTER_H
#define GAIA_PARSER_FILTER_H

#include <QMutex>
#include "parsertypes.h"

namespace gaia2 {
namespace parser {

class FilterParser;
struct yyParser;

#ifndef SWIG
#  ifdef INCLUDED_FROM_LEMON
// declare this here so we can tell Filter to make it one of its friends. Yay, friends!
static void yy_reduce(yyParser*, int);
#  else
void yy_reduce(yyParser*, int);
#  endif
#endif

} // namespace parser


/**
 * Returns true if the given predicate is always true (ie: it is a boolean
 * constant which value is True).
 */
inline bool isAlwaysTrue(parser::Predicate* pred) {
  parser::BooleanConstant* b = dynamic_cast<parser::BooleanConstant*>(pred);
  return (b && (b->value() == true));
}

/**
 * Returns true if the given predicate is always false (ie: it is a boolean
 * constant which value is False).
 */
inline bool isAlwaysFalse(parser::Predicate* pred) {
  parser::BooleanConstant* b = dynamic_cast<parser::BooleanConstant*>(pred);
  return (b && (b->value() == false));
}




/**
 * The Filter class allows to check whether a predicate is true for a given
 * Point. To use it, you first need to instantiate it (usually returned by the
 * FilterParser::parse() method), and then bind it to a specific layout (so that
 * variables used in the query are bound to the variables in the layout).
 * You can then call Filter::isTrue() method with a Point as argument to know
 * whether the predicate is true for the given Point or not.
 */
class Filter {
 protected:
  parser::Predicate* _pred;

  // we keep these to be able to bind all of them once we know the layout
  QList<parser::ValueVariable*> _valueVarList;
  QList<parser::LabelVariable*> _labelVarList;
  QList<parser::LabelPointID*> _pointVarList;

  // temporary lists for storing values in a IN (..., ...) clause
  // TODO: remove those, they don't belong here...
  QStringList _slist;
  QList<Real> _vlist;

  // WARNING: having mutable makes the class strongly non-reentrant
  //          eg.: we might think isTrue() is reentrant, but it's definitely not
  mutable const Point* _currentPoint;

  // these are friends because we don't want to expose internal fields, even
  // through public setters/getters, but they need to have access to them
  // because they are the ones in charge of constructing the object
  friend class parser::FilterParser;
  friend void parser::yy_reduce(parser::yyParser*, int);

  template <typename SearchPointType, typename DataSetType>
  friend class BaseQueryOptimizer;

  void clear();
  void clearVariables();

  void updateVariables(parser::Predicate* pred);
  void updateVariables(parser::Value* value);
  void updateVariables(parser::Label* label);

  static Filter* stealDataFrom(const Filter& other);


 public:

  Filter() : _pred(0) {}

  Filter(const Filter& filter);

  /**
   * Parses a given string and returns the newly created Filter object.
   */
  static Filter* parse(const QString& str);

  /**
   * Look in the predicate/value tree for all the variables it can find and
   * put them inside their respective *VarList member variables.
   * This needs to be called if you want your filter to be valid, otherwise it might
   * do weird things (such as crash your program, corrupt your data, ...).
   */
  void updateVariables();

  /**
   * Returns whether this filter needs to be bound to a specific layout.
   * A filter will need to be bound when it has at least one value or label
   * variable, otherwise it is free (and most probably useless!).
   */
  bool needsBinding() const;

  void bindToLayout(const PointLayout& layout);
  void bindToPointAttributes();

  ~Filter();

  /**
   * Returns whether the filter predicate is true for the given point.
   */
  bool isTrue(const Point* p) const;

  /**
   * Returns whether the filter predicate is always true, regardless of the given point.
   */
  bool isAlwaysTrue() const;

  const parser::Predicate* pred() const { return _pred; }

  static int opFromString(const QString& str);

  static int opFromString(const std::string& str) {
    return opFromString(QString::fromUtf8(str.c_str(), str.size()));
  }

  QString toString() const;
};



namespace parser {

/**
 * The FilterParser class is used to parse filter queries given as strings and
 * create the corresponding Filter instance that can be used on Points.
 */
class FilterParser {

 public:
  /**
   * This method parses a string representing an SQL-like query filter and
   * creates the associated Filter instance. Ownership of the Filter is yielded
   * to the calling function.
   */
  static Filter* parse(const QString& str);


 protected:
  // we need a global mutex for the parser because it uses global vars, and as
  // such is not re-entrant. By using this mutex, we even make it thread-safe!
  static QMutex _parserMutex;

  mutable void* _lemonParser; // C makes us do ugly things... (mutable void*?!?)

  FilterParser();
  ~FilterParser();
  Filter* parseFilter(const QString& str) const;
};



} // namespace parser
} // namespace gaia2

#endif // GAIA_PARSER_FILTER_H
