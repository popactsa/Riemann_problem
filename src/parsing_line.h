#ifndef SCEN_PARSING_LINE_H
#define SCEN_PARSING_LINE_H

#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

class ScenParsingLine {
    // Parsed line from scenario file with info for future variable values
    // parsed. Also used when performing IO operations(e.g. showing solver types
    // for .scen files).
    // Parsing format : <HeadSpecialChar> name {args_} <TailSpecialChar>
    // Lines with incorrect <HeadSpecialChar> are set as commentaries
public:
    enum class HeadSpecialChars : char {
        qNotSet = 0,
        qCommentary = '#',
        qFileInfo = '!',
        // qBeginGroup = '{',
        // qEndGroup = '}'
    };
    enum class VariableType : short {
        qCommonType = 0,
        qCompoundType = 1 << 0,
        qArrayType = 1 << 1
    };
    enum class TypeSpecialChars : char { qNotSet = 0, qCompoundType = ':' };

    enum class Group { qWall };
    // Enumeration of groups of variables in solvers

    ScenParsingLine() noexcept { Reset(); };
    ScenParsingLine(const ScenParsingLine&) = default;
    ScenParsingLine(ScenParsingLine&&) = default;
    void Load(const std::string& line) noexcept;

    inline bool is_SolverType() noexcept
    {
        if (head_spec_char_
            == HeadSpecialChars::qFileInfo
            && name_
            == "SolverType")
            return true;
        return false;
    }

    inline HeadSpecialChars get_head_spec_char() const noexcept
    {
        return head_spec_char_;
    }
    inline VariableType get_type() const noexcept { return type_; }
    inline const std::string& get_name() const noexcept { return name_; }
    inline const std::vector<std::string>& get_args() const noexcept
    {
        return args_;
    }
    inline void Reset() noexcept
    {
        name_.clear();
        head_spec_char_ = HeadSpecialChars(HeadSpecialChars::qNotSet);
        type_ = VariableType::qCommonType;
        index_ = 0;
        args_.clear();
    }
private:
    HeadSpecialChars head_spec_char_;
    VariableType type_;
    std::string name_;
    std::size_t index_;
    std::vector<std::string> args_;
};

ScenParsingLine::VariableType operator|=(ScenParsingLine& lhs,
                                         const ScenParsingLine& rhs) noexcept;

#endif // SCEN_PARSING_LINE_H
