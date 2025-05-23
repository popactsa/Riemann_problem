#ifndef SCEN_PARSING_LINE_H
#define SCEN_PARSING_LINE_H

#include "VariableType.h"
#include "auxiliary_functions.h"
#include "concepts.h"
#include "error_handling.h"
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <string>
#include <variant>
#include <vector>

class ScenParsingLine {
    // Parsed line from scenario file with info for future variable values
    // parsed. Also used when performing IO operations(e.g. showing solver types
    // for .scen files).
    // Parsing format : <HeadSpecialChar> name {args_} <TailSpecialChar>
    // Lines with incorrect <HeadSpecialChar> are set as commentaries
public:
    using container = std::vector<std::string>;

    enum class HeadSpecialChars : char {
        qVariable = 0,
        qCommentary = '#',
        qFileInfo = '!'
    };
    enum class TypeSpecialChars : char { qNotSet = 0, qArrayType = ':' };
    struct NamedArg {
        std::string name_;
        std::string value_;
    };

    ScenParsingLine() noexcept { Reset(); };
    // ScenParsingLine(const std::string& line) noexcept { Load(line); }
    ScenParsingLine(const ScenParsingLine&) = default;
    ScenParsingLine(ScenParsingLine&&) = delete;

    constexpr static void SetSep(char new_sep) noexcept { sep = new_sep; }
    void Load(const std::string& line) noexcept;
    // ScenParsingLine& Load(const std::string& line) noexcept;
    inline constexpr bool IsSolverType() noexcept
    {
        if (head_spec_char_
            == HeadSpecialChars::qFileInfo
            && name_
            == "SolverType")
            return true;
        return false;
    }

    inline constexpr HeadSpecialChars get_head_spec_char() const noexcept
    {
        return head_spec_char_;
    }
    inline constexpr dash::Flag<VariableType> get_type() const noexcept
    {
        return dash::Flag{type_};
    }
    inline constexpr std::size_t get_index() const
    {
        return index_ ? *index_ : throw std::exception();
    }
    inline constexpr const std::string& get_name() const noexcept
    {
        return name_;
    }
    constexpr const std::string& get_common_arg_at(std::size_t i) const
    {
        dash::Expect<dash::ErrorAction::qThrowing, std::bad_variant_access>(
            [this]() {
                return std::holds_alternative<std::vector<std::string>>(args_);
            },
            "Trying to access common argument");
        return std::get<std::vector<std::string>>(args_).at(i);
    }
    constexpr const std::vector<std::string>& get_common_args() const
    {
        dash::Expect<dash::ErrorAction::qThrowing, std::bad_variant_access>(
            [this]() {
                return std::holds_alternative<std::vector<std::string>>(args_);
            },
            "Trying to access common argument");
        return std::get<std::vector<std::string>>(args_);
    }
    constexpr const NamedArg& get_named_arg_at(std::size_t i) const
    {
        dash::Expect<dash::ErrorAction::qTerminating, std::bad_variant_access>(
            [this]() {
                return std::holds_alternative<std::vector<NamedArg>>(args_);
            },
            "Trying to access named argument");
        return std::get<std::vector<NamedArg>>(args_).at(i);
    }
    constexpr const std::vector<NamedArg>& get_named_args() const
    {
        dash::Expect<dash::ErrorAction::qThrowing, std::bad_variant_access>(
            [this]() {
                return std::holds_alternative<std::vector<NamedArg>>(args_);
            },
            "Trying to access named argument");
        return std::get<std::vector<NamedArg>>(args_);
    }

    inline void Reset() noexcept
    {
        head_spec_char_ = HeadSpecialChars(HeadSpecialChars::qVariable);
        type_.reset();
        name_.clear();
        index_ = -1;
        std::visit([](auto& arg) { arg.clear(); }, args_);
    }
    ~ScenParsingLine() = default;
private:
    static inline char sep = '\t';
    HeadSpecialChars head_spec_char_;
    dash::Flag<VariableType> type_;
    std::string name_;
    std::optional<std::size_t> index_;
    using args_variant =
        std::variant<std::vector<std::string>, std::vector<NamedArg>>;
    args_variant args_;
};

#endif // SCEN_PARSING_LINE_H
