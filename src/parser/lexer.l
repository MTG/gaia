%{
#include "grammar.h"
#include "token.h"
#include "gaiaexception.h"

Token yylval;

#ifndef YYSTYPE
#define YYSTYPE Token
#endif

#ifdef OS_WIN32
#include <io.h> // for _isatty function
#define isatty _isatty
#define YY_NO_UNISTD_H
#endif // OS_WIN32

// to avoid linking against libfl, as well as warnings turning to errors
#define YY_SKIP_YYWRAP
static void yyunput(int, char*);

int yywrap(void) {
  if (1) return 1;
  else yyunput(0, 0);
}

%}

REAL          {SCIENTIFIC}|{FLOAT}|{INTEGER}
SCIENTIFIC    {FLOAT}"e"{INTEGER}
FLOAT         {PUREFLOAT}|{INTEGER}
PUREFLOAT     "-"?[0-9]*\.[0-9]*
INTEGER       "-"?[0-9]+
STRING        \"[^\"]*\"
ID            [a-zA-Z0-9\[\]\.]*

%%


[ \t]+        {}

"("           { return PAREN_OPEN;  }
")"           { return PAREN_CLOSE; }
","           { return COMMA;       }

"NOT"         { return NOT;     }

"AND"         { return AND;     }
"OR"          { return OR;      }

"&&"          { return AND;     }
"||"          { return OR;      }

"IN"          { return IN;      }
"BETWEEN"     { return BETWEEN; }
"WHERE"       { return WHERE;   }

"=="          { return EQ;      }
"="           { return EQ;      }
"!="          { return NEQ;     }
"<"           { return LT;      }
">"           { return GT;      }
"<="          { return LTE;     }
">="          { return GTE;     }

"TRUE"        {
  yylval.boolv = true;
  return BOOL_CONSTANT;
}

"FALSE"       {
  yylval.boolv = false;
  return BOOL_CONSTANT;
}

{REAL}        {
  yylval.value = atof(yytext);
  /*std::cout << "SCANNER: value - " << std::flush;*/
  return VALUE_CONSTANT;
}

{STRING}      {
  // remove surrounding double-quotes
  yylval.name = new QString(yytext+1);
  yylval.name->chop(1);
  return STRING;
}

"VALUE"\.{ID} {
  yylval.name = new QString(yytext+6);
  return VALUE_VARIABLE;
}

"LABEL"\.{ID} {
  yylval.name = new QString(yytext+6);
  return LABEL_VARIABLE;
}

"POINT"\."ID" {
  return POINT_ID;
}


. {
  throw gaia2::GaiaException("Scanner error at character '", yytext, "'");
}
%%
