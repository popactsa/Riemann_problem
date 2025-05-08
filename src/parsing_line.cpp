#include "error_handling.h"
#include "parsing_line.h"

#include "io.h"
#include <algorithm>
#include <iterator>

void ScenParsingLine::Load(const std::string& line) noexcept
{
    Reset();
    std::size_t pop_words = 0;
    std::vector<std::string> split = SplitString(line, sep);
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
        [&split]() { return split.size() >= 2; },
        "Provide more arguments(>=2)");

    const auto found_arraytype_char = std::find_if(
        split.cbegin() + pop_words,
        pop_words + 2 < split.size() ? split.cbegin() + pop_words + 2
                                     : split.cend(),
        [](auto& str) {
            return str.front()
                   == static_cast<char>(
                       ScenParsingLine::TypeSpecialChars::qArrayType);
        });
    if (found_arraytype_char != split.cend()) {
        std::size_t where =
            std::distance(split.cbegin() + pop_words, found_arraytype_char);
        if (where == 0) {
            index_ = {};
        } else {
            bool is_uint = dash::IsUnsignedInt(split[1]);
            if (is_uint) {
                index_ = std::stoul(split[1]);
            }
        }
        pop_words += where + 1;
        type_ |= dash::Flag{VariableType::qArrayType};
    }

    const auto found_namedtype_evidence =
        std::find_if(split.cbegin() + pop_words, split.cend(),
                     [](auto& str) { return std::isalpha(str.front()); });
    type_ |= found_namedtype_evidence
                     != split.cend()
                     && split.size()
                     - pop_words
                     != 1
                 ? dash::Flag{VariableType::qNamedType}
                 : dash::Flag{VariableType::qCommonType};

    {
        auto mask = dash::Flag{VariableType::qNamedType}
                    & dash::Flag{VariableType::qArrayType};
        if ((type_ & mask) == mask && !index_) {
            type_ &= ~dash::Flag{VariableType::qArrayType};
        }
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
