#include "parsing_line.h"

#include "error_handling.h"
#include "io.h"
#include <algorithm>
#include <fstream>
#include <iterator>

void ScenParsingLine::Load(const std::string& line) noexcept
{
    ResetPreserveIndex();

    std::vector<std::string> split = SplitString(line, ' ');
    if (split.size() == 0) {
        success_ = false;
    }
    {
        using enum HeadSpecialChars;
        switch (split[0][0]) {
        case static_cast<char>(qCommentary):
            head_spec_char_ = qCommentary;
            return;
        case static_cast<char>(qEndGroup):
            head_spec_char_ = qEndGroup;
            break;
        case static_cast<char>(qFileInfo):
            head_spec_char_ = qFileInfo;
            break;
        default:
            head_spec_char_ = qNotSet;
            break;
        }
    }
    std::size_t offset = head_spec_char_ == HeadSpecialChars::qNotSet ? 0 : 1;
    name_.resize(split[0].size() - offset);
    std::copy(split[0].cbegin() + offset, split[0].cend(), name_.begin());

    if (split.size() < 2) {
        success_ = false;
    }
    args_.reserve(split.size() - 1);
    std::copy(split.cbegin() + 1, split.cend(), std::back_inserter(args_));
    {
        using enum TailSpecialChars;
        switch (args_.back().back()) {
        case static_cast<char>(qBeginGroup):
            tail_spec_char_ = qBeginGroup;
            break;
        default:
            tail_spec_char_ = qNotSet;
            break;
        }
    }

    if (static_cast<char>(head_spec_char_)
        == static_cast<char>(tail_spec_char_)
        && head_spec_char_
        != HeadSpecialChars::qNotSet) {
        success_ = false;
    }
}

void ScenParsingLine::SetIndex()
{
    index_ = std::stoul(args_.at(0));
}
