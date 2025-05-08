#ifndef PARSER_TEST_H
#define PARSER_TEST_H

#include "error_handling.h"
#include "parsing_line.h"

bool IsCommonType(std::string str);
bool IsArrayType(std::string str);
bool IsNamedType(std::string str);

#endif
