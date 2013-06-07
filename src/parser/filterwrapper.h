#ifndef GAIA_PARSER_FILTERWRAPPER_H
#define GAIA_PARSER_FILTERWRAPPER_H

#include "filter.h"
#include "convert.h"

namespace gaia2 {
namespace filter {

/**
 * The FilterWrapper class serves as an easy wrapper for a subset of all
 * possible filters, but with a much more intuitive API that allows filters
 * to be constructed programatically on the fly instead of being parsed from
 * a string representation.
 */
class FilterWrapper : public Filter {
  void initValueList(const QString& varname, const QString& op, const QList<Real>& values);
  void initLabelList(const QString& varname, const QString& op, const QList<QString>& labels);

 public:
  FilterWrapper(const QString& varname, const QString& op, Real value);
  FilterWrapper(const QString& varname, const QString& op, const QList<Real>& values);

  FilterWrapper(const QString& varname, const QString& op, const QString& label);
  FilterWrapper(const QString& varname, const QString& op, const QList<QString>& labels);

  FilterWrapper(const std::string& varname, const std::string& op, Real value);
  FilterWrapper(const std::string& varname, const std::string& op, const std::vector<Real>& values);

  FilterWrapper(const std::string& varname, const std::string& op, const std::string& label);
  FilterWrapper(const std::string& varname, const std::string& op, const std::vector<std::string>& labels);

};


class AndFilter : public Filter {
  void init(const QList<Filter*>& preds);

 public:
  AndFilter(const QList<Filter*>& preds)  { init(preds); }
  AndFilter(const std::vector<Filter*>& preds) { init(convert::Vector_to_QList(preds)); }

};

class OrFilter : public Filter {
  void init(const QList<Filter*>& preds);

 public:
  OrFilter(const QList<Filter*>& preds)  { init(preds); }
  OrFilter(const std::vector<Filter*>& preds) { init(convert::Vector_to_QList(preds)); }
};

class NotFilter : public Filter {
 public:
  NotFilter(const Filter* pred);
};


} // namespace filter
} // namespace gaia2


#endif // GAIA_PARSER_FILTERWRAPPER_H
