#include "error_handling.h"
#include "parsing_line.h"

#include "io.h"
#include <algorithm>
#include <iterator>

void ScenParsingLine::Load(const std::string& line) noexcept
{
    Reset();
    std::size_t pop_words = 0;
    std::vector<std::string> split = SplitString(line, '\t');
    switch (static_cast<HeadSpecialChars>(split[0].front())) {
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
        head_spec_char_ = qVariable;
        break;
    }
    }
    std::size_t offset = head_spec_char_ == HeadSpecialChars::qVariable ? 0 : 1;
    name_.resize(split[0].size() - offset);
    std::copy(split[0].cbegin() + offset, split[0].cend(), name_.begin());
    ++pop_words;
    dash::Expect<dash::ErrorAction::qTerminating, dash::InvalidLineFormat>(
        [&split, &pop_words]() { return split.size() >= 2; },
        "Provide more arguments(>=2)");

    if (split.size()
        - pop_words
        > 1
        && head_spec_char_
        == HeadSpecialChars::qVariable) {
        bool is_uint = dash::IsUnsignedInt(split[1]);
        if (is_uint) {
            index_ = std::stoul(split[1]);
            type_ |= dash::Flag{VariableType::qArrayType};
            ++pop_words;
        }
    }

    if (head_spec_char_ == HeadSpecialChars::qVariable) {
        switch (static_cast<TypeSpecialChars>(split[pop_words].front())) {
            using enum TypeSpecialChars;
        case qNamedType: {
            type_ |= dash::Flag{VariableType::qNamedType};
            ++pop_words;
            break;
        }
        default: {
            type_ |= dash::Flag{VariableType::qCommonType};
            break;
        }
        }
    } else {
        type_ |= dash::Flag{VariableType::qCommonType};
    }

    dash::Expect<dash::ErrorAction::qTerminating, dash::InvalidLineFormat>(
        [&split, &pop_words]() { return split.size() > pop_words; },
        "No arguments to variable passed");

    if (type_ & dash::Flag{VariableType::qNamedType}) {
        dash::Expect<dash::ErrorAction::qTerminating, dash::InvalidLineFormat>(
            [&split, &pop_words]() {
                return (split.size() - pop_words) % 2 == 0;
            },
            "Not all names are paired with values");
        args_ = std::vector<NamedArg>();
        for (std::size_t i = pop_words; i < split.size(); i += 2) {
            std::get<std::vector<NamedArg>>(args_).push_back(
                {split[i], split[i + 1]});
        }
    } else if (type_ & dash::Flag{VariableType::qCommonType}) {
        args_ = std::vector<std::string>();
        std::size_t args_size = split.size() - pop_words;
        if (args_size > 1) {
            type_ |= dash::Flag{VariableType::qArrayType};
        }
        std::get<std::vector<std::string>>(args_).reserve(args_size);
        auto it = std::back_inserter(std::get<std::vector<std::string>>(args_));
        std::copy(split.cbegin() + pop_words, split.cend(), it);
    }
}
