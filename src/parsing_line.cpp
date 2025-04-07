#include "parsing_line.h"

#include "io.h"
#include <iterator>

ScenParsingLine& ScenParsingLine::operator=(const std::string& line) noexcept
{
    success_ = true;
    std::vector<std::string> split;
    SplitString(line, ' ', split);
    if (split.size() == 0) {
        success_ = false;
    }
    using enum HeadSpecialChars;
    switch (split[0][0]) {
    case static_cast<char>(qCommentary):
        head_spec_char_ = qCommentary;
    case static_cast<char>(qEndGroup):
        head_spec_char_ = qEndGroup;
    case static_cast<char>(qFileInfo):
        head_spec_char_ = qFileInfo;
    default:
        head_spec_char_ = qNotSet;
    }

    if (split.size() < 1) {
        success_ = false;
    }
    name_ = split[0];

    if (split.size() < 2) {
        success_ = false;
    }
    args_.reserve(split.size() - 1);
    std::copy(split.cbegin() + 1, split.cend(), std::back_inserter(args_));
    if (args_.back().size() == 1) {
        using enum TailSpecialChars;
        switch (args_.back()[0]) {
        case static_cast<char>(qBeginGroup):
            tail_spec_char_ = qBeginGroup;
        case static_cast<char>(qEndGroup):
            tail_spec_char_ = qEndGroup;
        default:
            tail_spec_char_ = qNotSet;
        }
    }

    if (static_cast<char>(head_spec_char_)
        == static_cast<char>(tail_spec_char_)
        && head_spec_char_
        != HeadSpecialChars::qNotSet) {
        success_ = false;
    }
    return *this;
}
