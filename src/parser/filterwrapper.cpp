#include "filterwrapper.h"
#include "convert.h"
using namespace std;
using namespace gaia2::parser;

namespace gaia2 {
namespace filter {


FilterWrapper::FilterWrapper(const QString& varname, const QString& op, Real value) {
  Value* lhs = new ValueVariable(varname);
  Value* rhs = new ValueConstant(value);
  _pred = new PredValueComparison(lhs, opFromString(op), rhs);
}

FilterWrapper::FilterWrapper(const string& varname, const string& op, Real value) {
  Value* lhs = new ValueVariable(QString::fromUtf8(varname.c_str(), varname.size()));
  Value* rhs = new ValueConstant(value);
  _pred = new PredValueComparison(lhs, opFromString(op), rhs);
}

void FilterWrapper::initValueList(const QString& varname, const QString& op, const QList<Real>& values) {
  int opv = opFromString(op);

  switch (opv) {

  case BETWEEN:
    if (values.size() != 2) throw GaiaException("Filter: the BETWEEN operator takes 2 values");

    _pred = new PredValueRange(new ValueVariable(varname), values[0], values[1]);
    return;

  case IN:
    _pred = new PredValueIsIn(new ValueVariable(varname), values);
    return;

  default:
    throw GaiaException("Filter: operator '", op, "' not compatible with a list of values");
  }
}

FilterWrapper::FilterWrapper(const QString& varname, const QString& op, const QList<Real>& values) {
  initValueList(varname, op, values);
}


FilterWrapper::FilterWrapper(const string& varname, const string& op, const vector<Real>& values) {
  initValueList(QString::fromUtf8(varname.c_str(), varname.size()),
                QString::fromUtf8(op.c_str(), op.size()),
                convert::Vector_to_QList(values));
}


FilterWrapper::FilterWrapper(const QString& varname, const QString& op, const QString& label) {
  Label* lhs = new LabelVariable(varname);
  Label* rhs = new LabelConstant(label);
  _pred = new PredLabelComparison(lhs, opFromString(op), rhs);
}

FilterWrapper::FilterWrapper(const string& varname, const string& op, const string& label) {
  Label* lhs = new LabelVariable(QString::fromUtf8(varname.c_str(), varname.size()));
  Label* rhs = new LabelConstant(QString::fromUtf8(label.c_str(), label.size()));
  _pred = new PredLabelComparison(lhs, opFromString(op), rhs);
}


void FilterWrapper::initLabelList(const QString& varname, const QString& op, const QList<QString>& labels) {
  int opv = opFromString(op);

  if (opv != IN) {
    throw GaiaException("Filter: operator '", op, "' not compatible with a list of strings");
  }

  _pred = new PredLabelIsIn(new LabelVariable(varname), labels);
}


FilterWrapper::FilterWrapper(const QString& varname, const QString& op, const QList<QString>& labels) {
  initLabelList(varname, op, labels);
}

FilterWrapper::FilterWrapper(const string& varname, const string& op, const vector<string>& labels) {
  initLabelList(QString::fromUtf8(varname.c_str(), varname.size()),
                QString::fromUtf8(op.c_str(), op.size()),
                convert::VectorString_to_QStringList(labels));
}





void AndFilter::init(const QList<Filter*>& preds) {
  if (preds.size() < 2) throw GaiaException("AndFilter: need at least 2 conditions to build an AndFilter");

  // note: we add the filters in reverse order so that the evaluation order is the same as a normal
  //       and/or condition, lazy from left to right.
  int i = preds.size() - 1;

  // WARNING: ugliest hack of the world
  //          as a derived class cannot access protected members of other instances
  //          treated as base class, let's just pretend we're all of the same class...
  //          this hack is repeated a few times in the next lines, but this comment won't be...
  Predicate* acc = preds[i]->pred()->copy();

  for (i--; i >= 0; i--) {
    acc = new PredLogicalOp(preds[i]->pred()->copy(), AND, acc);
  }

  _pred = acc;
}

void OrFilter::init(const QList<Filter*>& preds) {
  if (preds.size() < 2) throw GaiaException("OrFilter: need at least 2 conditions to build an OrFilter");

  // note: we add the filters in reverse order so that the evaluation order is the same as a normal
  //       and/or condition, lazy from left to right.
  int i = preds.size() - 1;
  Predicate* acc = preds[i]->pred()->copy();

  for (i--; i >= 0; i--) {
    acc = new PredLogicalOp(preds[i]->pred()->copy(), OR, acc);
  }

  _pred = acc;
}

NotFilter::NotFilter(const Filter* pred) {
  _pred = new PredNegate(pred->pred()->copy());
}


} // namespace filter
} // namespace gaia2
