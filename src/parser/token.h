#ifndef GAIA_PARSER_TOKEN_H
#define GAIA_PARSER_TOKEN_H

#include <QString>
#include "../types.h"
#include <iostream>

class Token {
 public:
  double value;
  bool boolv;
  QString* name;
};


#endif // GAIA_PARSER_TOKEN_H
