#include "parser_test.h"

bool IsCommonType(std::string str)
{
    ScenParsingLine line;
    line.Load(str);
    return line.get_type() & dash::Flag{VariableType::qCommonType};
}

bool IsArrayType(std::string str)
{
    ScenParsingLine line;
    line.Load(str);
    return line.get_type() & dash::Flag{VariableType::qArrayType};
}

bool IsNamedType(std::string str)
{
    ScenParsingLine line;
    line.Load(str);
    return line.get_type() & dash::Flag{VariableType::qNamedType};
}
