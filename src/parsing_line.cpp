#include "error_handling.h"
#include "parsing_line.h"

#include "io.h"
#include <algorithm>
#include <iterator>

ScenParsingLine::VariableType
operator|=(ScenParsingLine::VariableType& lhs,
           const ScenParsingLine::VariableType& rhs) noexcept
{
    using ut = std::underlying_type_t<ScenParsingLine::VariableType>;
    lhs = static_cast<ScenParsingLine::VariableType>(static_cast<ut>(lhs)
                                                     | static_cast<ut>(rhs));
    return lhs;
}

void ScenParsingLine::Load(const std::string& line) noexcept
{
    Reset();

    std::size_t pop_words = 0;
    std::vector<std::string> split = SplitString(line, ' ');
    switch (static_cast<HeadSpecialChars>(split[0][0])) {
        using enum HeadSpecialChars;
    case qCommentary: {
        head_spec_char_ = qCommentary;
        return;
    }
    case qFileInfo: {
        head_spec_char_ = qFileInfo;
        break;
    }
    default: {
        head_spec_char_ = qNotSet;
        break;
    }
    }
    std::size_t offset = head_spec_char_ == HeadSpecialChars::qNotSet ? 0 : 1;
    name_.resize(split[0].size() - offset);
    std::copy(split[0].cbegin() + offset, split[0].cend(), name_.begin());
    ++pop_words;

    try {
        index_ = std::stoul(split[1]);
        type_ |= VariableType::qArrayType;
        ++pop_words;
    } catch (const std::invalid_argument& err) {
    } catch (const std::out_of_range& err) {
        std::terminate();
    }

    switch (static_cast<TypeSpecialChars>(split[pop_words][0])) {
        using enum TypeSpecialChars;
    case qCompoundType: {
        type_ |= VariableType::qCompoundType;
        ++pop_words;
        break;
    }
    default: {
        break;
    }
    }

    dash::Expect<dash::ErrorAction::qTerminating, dash::InvalidLineFormat>(
        [&split, &pop_words]() { return split.size() > pop_words; },
        "No arguments to variable passed");

    if (split.size() <= pop_words)
        args_.reserve(split.size() - pop_words);
    std::copy(split.cbegin() + pop_words, split.cend(),
              std::back_inserter(args_));
}
