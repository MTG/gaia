
%include {

/*

Grammar

 Predicate ::= PredBoolean | PredComparison | PredBinaryOp | PredUnaryOp
 PredBoolean ::= TRUE | FALSE
 ValueComparisonType ::= '==' | '!=' | '<' | '<=' | '>' | '>='
 StringComparisonType ::= '==' | '!='
 PredComparison ::= Value ValueComparisonType Value | String StringComparisonType String
 BinaryOp ::= '&&' | '||' | AND | OR
 UnaryOp ::= NOT
 PredBinaryOp ::= Predicate BinaryOp Predicate
 PredUnaryOp ::= UnaryOp Predicate

 Value ::= VALUE_CONSTANT | VALUE_VARIABLE
 String ::= STRING_CONSTANT | STRING_VARIABLE

*/

#define INCLUDED_FROM_LEMON
#include <iostream>
#include "grammar.h"
#include "token.h"
#include "parsertypes.h"
#include "filter.h"

// this is a hack to get stuff inside the correct namespace:
// we open the namespace here, so that it is after the #include's and we close
// it in the including file, in our case the "filter.cpp" file
OPEN_GAIA_PARSER_NS

Filter parseResult;


}

%syntax_error {
  throw GaiaException("Syntax error: could not parse expression");
}

%token_type {Token}
%type predicate {Predicate*}
%type value {Value*}
%type filter {Predicate*}
%type label {Label*}

%left AND.
%left OR.
%nonassoc EQ NEQ GT GTE LT LTE.

program ::= filter(A). {
  parseResult._pred = A;
}

// only filter using point attributes
filter(A) ::= WHERE predicate(B). {
  A = B;
}

// these calls (A->value()) will fail if trying to use a LabelVariable
// it corresponds more or less to the desired behaviour at the moment
string_list ::= string_list COMMA label(A). { parseResult._slist.append(A->value()); delete A; }
string_list ::= label(A).                   { parseResult._slist.append(A->value()); delete A; }

// these calls (A->value()) will fail if trying to use a ValueVariable
// it corresponds more or less to the desired behaviour at the moment
value_list ::= value_list COMMA value(A).   { parseResult._vlist.append(A->value()); delete A; }
value_list ::= value(A).                    { parseResult._vlist.append(A->value()); delete A; }


predicate(A) ::= NOT PAREN_OPEN predicate(B) PAREN_CLOSE. { A = new PredNegate(B); }
predicate(A) ::= PAREN_OPEN predicate(B) PAREN_CLOSE. { A = B; }

predicate(A) ::= label(B) EQ  label(C). { A = new PredLabelComparison(B, EQ, C);  }
predicate(A) ::= label(B) NEQ label(C). { A = new PredLabelComparison(B, NEQ, C); }

predicate(A) ::= predicate(B) OR  predicate(C). { A = new PredLogicalOp(B, OR,  C); }
predicate(A) ::= predicate(B) AND predicate(C). { A = new PredLogicalOp(B, AND, C); }

predicate(A) ::= value(B) EQ  value(C). { A = new PredValueComparison(B, EQ, C);  }
predicate(A) ::= value(B) NEQ value(C). { A = new PredValueComparison(B, NEQ, C); }
predicate(A) ::= value(B) LT  value(C). { A = new PredValueComparison(B, LT, C);  }
predicate(A) ::= value(B) GT  value(C). { A = new PredValueComparison(B, GT, C);  }
predicate(A) ::= value(B) LTE value(C). { A = new PredValueComparison(B, LTE, C); }
predicate(A) ::= value(B) GTE value(C). { A = new PredValueComparison(B, GTE, C); }

predicate(A) ::= value(B) BETWEEN value(C) AND value(D). {
  if (!dynamic_cast<ValueConstant*>(C) || !dynamic_cast<ValueConstant*>(D)) {
    throw GaiaException("BETWEEN ... AND ... can only be used with constant values");
  }
  A = new PredValueRange(B, C->value(), D->value());
  delete C;
  delete D;
}

predicate(A) ::= BOOL_CONSTANT(B). {
  A = new BooleanConstant(B.boolv);
}

predicate(A) ::= label(B) IN PAREN_OPEN string_list PAREN_CLOSE. {
  A = new PredLabelIsIn(B, parseResult._slist);
  parseResult._slist.clear();
}

predicate(A) ::= label(B) NOT IN PAREN_OPEN string_list PAREN_CLOSE. {
  A = new PredNegate(new PredLabelIsIn(B, parseResult._slist));
  parseResult._slist.clear();
}

predicate(A) ::= value(B) IN PAREN_OPEN value_list PAREN_CLOSE. {
  A = new PredValueIsIn(B, parseResult._vlist);
  parseResult._vlist.clear();
}

predicate(A) ::= value(B) NOT IN PAREN_OPEN value_list PAREN_CLOSE. {
  A = new PredNegate(new PredValueIsIn(B, parseResult._vlist));
  parseResult._vlist.clear();
}

label(A) ::= STRING(B). {
  A = new LabelConstant(*B.name);
  // got it, now we can safely remove the ref we got in the lexer
  delete B.name;
}

label(A) ::= LABEL_VARIABLE(B).    {
  A = new LabelVariable(*B.name);
  // got it, now we can safely remove the ref we got in the lexer
  delete B.name;
}

label(A) ::= POINT_ID. {
  A = new LabelPointID();
}


value(A) ::= VALUE_CONSTANT(B).    {
  A = new ValueConstant(B.value);
}

value(A) ::= VALUE_VARIABLE(B). {
  A = new ValueVariable(*B.name);
  // got it, now we can safely remove the ref we got in the lexer
  delete B.name;
}
