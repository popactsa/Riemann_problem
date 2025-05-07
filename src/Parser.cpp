#include "Parser.h"

void Parse(int* target, std::string_view str_value)
{
    std::from_chars(str_value.cbegin(), str_value.cend(), *target);
}

void Parse(std::size_t* target, std::string_view str_value)
{
    std::from_chars(str_value.cbegin(), str_value.cend(), *target);
}

void Parse(double* target, std::string_view str_value)
{
    std::from_chars(str_value.cbegin(), str_value.cend(), *target);
}

void Parse(std::string* target, std::string_view str_value)
{
    *target = str_value;
}

void Parse(bool* target, std::string_view str)
{
    if (str.compare("true") || str.compare("1")) {
        *target = true;
    } else if (str.compare("false") || str.compare("0")) {
        *target = false;
    }
}
