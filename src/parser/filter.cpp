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


// only include grammar.c, it is the first part of our grammar, it also does
// all the includes by itself
#include <cassert>
#include "grammar.c"

// trick to have lemon generate code inside our namespace:
// the parser namespace is open inside grammar.c and is closed here
CLOSE_GAIA_PARSER_NS

// -------- Additional declarations needed for lemon parser --------
// -------- (to include the flex scanner)                   --------
typedef struct yy_buffer_state *YY_BUFFER_STATE;

int             yylex( void );
YY_BUFFER_STATE yy_scan_string( const char * );
void            yy_delete_buffer( YY_BUFFER_STATE );

extern Token yylval;

using namespace gaia2::parser;

namespace gaia2 {


Filter::Filter(const Filter& other)
  : _pred(other._pred->copy()), _valueVarList(other._valueVarList),
    _labelVarList(other._labelVarList), _pointVarList(other._pointVarList) {}


Filter* Filter::stealDataFrom(const Filter& other) {
  Filter* result = new Filter();

  // don't make copies of the pointers, this is the desired behavior
  // (although dangerous, of course)
  result->_pred = other._pred;
  result->_valueVarList = other._valueVarList;
  result->_labelVarList = other._labelVarList;
  result->_pointVarList = other._pointVarList;

  return result;
}

void Filter::updateVariables() {
  clearVariables();

  updateVariables(_pred);
}

void Filter::updateVariables(parser::Value* pred) {
  ValueVariable* vv = dynamic_cast<ValueVariable*>(pred);
  if (vv) { _valueVarList << vv; return; }
}

void Filter::updateVariables(parser::Label* pred) {
  LabelVariable* lv = dynamic_cast<LabelVariable*>(pred);
  if (lv) { _labelVarList << lv; return; }

  LabelPointID* lp = dynamic_cast<LabelPointID*>(pred);
  if (lp)  { _pointVarList << lp; return; }
}

void Filter::updateVariables(parser::Predicate* pred) {

  PredValueComparison* pvc = dynamic_cast<PredValueComparison*>(pred);
  if (pvc) {
    updateVariables(pvc->_lhs);
    updateVariables(pvc->_rhs);
    return;
  }

  PredValueRange* pvr = dynamic_cast<PredValueRange*>(pred);
  if (pvr) {
    updateVariables(pvr->_var);
    return;
  }

  PredLabelComparison* plc = dynamic_cast<PredLabelComparison*>(pred);
  if (plc) {
    updateVariables(plc->_lhs);
    updateVariables(plc->_rhs);
    return;
  }

  PredLogicalOp* plo = dynamic_cast<PredLogicalOp*>(pred);
  if (plo) {
    updateVariables(plo->_lhs);
    updateVariables(plo->_rhs);
    return;
  }

  PredNegate* pn = dynamic_cast<PredNegate*>(pred);
  if (pn) {
    updateVariables(pn->_pred);
    return;
  }

  PredValueIsIn* pvi = dynamic_cast<PredValueIsIn*>(pred);
  if (pvi) {
    updateVariables(pvi->_var);
    return;
  }

  PredLabelIsIn* pli = dynamic_cast<PredLabelIsIn*>(pred);
  if (pli) {
    updateVariables(pli->_var);
    return;
  }

  if (dynamic_cast<BooleanConstant*>(pred)) return;

  // we should not arrive here, that would mean we missed a type...
  throw GaiaException("Filter::updateVariables: wrong predicate type");
}

int Filter::opFromString(const QString& str) {
  if (str == "==") return EQ;
  if (str == "!=") return NEQ;
  if (str == ">")  return GT;
  if (str == ">=") return GTE;
  if (str == "<")  return LT;
  if (str == "<=") return LTE;
  if (str.toLower() == "in") return IN;
  if (str.toLower() == "between") return BETWEEN;

  throw GaiaException("Unknown filter operator: ", str);
}


bool Filter::needsBinding() const {
  return (_valueVarList.size() +
          _labelVarList.size()) > 0;
}

void Filter::bindToLayout(const PointLayout& layout) {
  foreach (parser::ValueVariable* var, _valueVarList) {
    var->bind(layout, &_currentPoint);
  }
  foreach (parser::LabelVariable* var, _labelVarList) {
    var->bind(layout, &_currentPoint);
  }
}

void Filter::bindToPointAttributes() {
  foreach (parser::LabelPointID* var, _pointVarList) {
    var->bind(&_currentPoint);
  }
}

bool Filter::isTrue(const Point* p) const {
  _currentPoint = p;
  return _pred->isTrue();
}

bool Filter::isAlwaysTrue() const {
  return gaia2::isAlwaysTrue(_pred);
}

void Filter::clearVariables() {
  _valueVarList.clear();
  _labelVarList.clear();
  _pointVarList.clear();
}

void Filter::clear() {
  _pred = 0;
  _slist.clear();
  _vlist.clear();
  clearVariables();
}

Filter::~Filter() {
  delete _pred;
  clear();
}

QString Filter::toString() const {
  return _pred->toString();
}


Filter* Filter::parse(const QString& str) {
  return parser::FilterParser::parse(str);
}


namespace parser {

QMutex FilterParser::_parserMutex;

FilterParser::FilterParser() {
  QMutexLocker locker(&_parserMutex);
  _lemonParser = ParseAlloc(malloc);
}

FilterParser::~FilterParser() {
  QMutexLocker locker(&_parserMutex);
  ParseFree(_lemonParser, free);
}

Filter* FilterParser::parse(const QString& str) {
  static FilterParser parser;
  return parser.parseFilter(str);
}

Filter* FilterParser::parseFilter(const QString& strin) const {
  QMutexLocker locker(&_parserMutex);

  QString str = strin;
  str.replace('\n', ' ');
  str.replace('\r', ' ');

  int yv;
  YY_BUFFER_STATE buf = yy_scan_string(str.toLatin1().data());

  Filter* result;

  try {
    while ((yv = yylex()) != 0) {
      G_DEBUG(GParser, "parsed type: " << yv
              << " - v: " << yylval.value
              << " - b: " << (yylval.boolv ? "true" : "false")
              << " - n: " << (yylval.name ? yylval.name->toUtf8().data() : "UNKNOWN"));
      Parse(_lemonParser, yv, yylval);

      // reset this pointer to 0 because it is now invalid (delete has occurred)
      yylval.name = 0;
    }

    Parse(_lemonParser, 0, yylval);

    result = Filter::stealDataFrom(parseResult);
  }
  catch (GaiaException& e) {
    // deallocate the parser and allocate a new one, so that it isn't in an
    // invalid state when we start parsing the next query
    ParseFree(_lemonParser, free);
    _lemonParser = ParseAlloc(malloc);

    // clear all the other structures
    parseResult.clear();
    yy_delete_buffer(buf);

    throw GaiaException(e.msg() + ": \"", str, '"');
  }

  result->updateVariables();

  G_DEBUG(GParser, "correctly parsed filter: " << result->toString());
  parseResult.clear();
  yy_delete_buffer(buf);

  return result;
}



} // namespace parser
} // namespace gaia2
