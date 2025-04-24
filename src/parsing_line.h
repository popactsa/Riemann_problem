#ifndef SCEN_PARSING_LINE_H
#define SCEN_PARSING_LINE_H

#include "iSolver.h"
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
        qEndGroup = '}'
    };
    enum class TailSpecialChars : char {
        qNotSet = 0,
        qBeginGroup = '{'
        // qEndGroup = '}'
    };

    enum class Group { qWall };
    // Enumeration of groups of variables in solvers

    ScenParsingLine() noexcept { Reset(); };
    ScenParsingLine(const ScenParsingLine&) = default;
    void Load(const std::string& line) noexcept;

    inline bool is_SolverType() noexcept
    {
        if (head_spec_char_
            == HeadSpecialChars::qFileInfo
            && success_
            && name_
            == "SolverType")
            return true;
        return false;
    }

    inline HeadSpecialChars get_head_spec_char() const noexcept
    {
        return head_spec_char_;
    }
    inline TailSpecialChars get_tail_spec_char() const noexcept
    {
        return tail_spec_char_;
    }
    inline const std::string& get_name() const noexcept { return name_; }
    inline operator bool() const noexcept { return success_; }
    inline const std::vector<std::string>& get_args() const noexcept
    {
        return args_;
    }
    inline std::size_t get_index() const noexcept { return index_; }
    inline void ResetPreserveIndex() noexcept
    {
        name_.clear();
        head_spec_char_ = HeadSpecialChars(HeadSpecialChars::qNotSet);
        tail_spec_char_ = TailSpecialChars(TailSpecialChars::qNotSet);
        args_.clear();
        success_ = true;
    }
    inline void Reset() noexcept
    {
        ResetPreserveIndex();
        index_ = 0;
    }
    void SetIndex();
private:
    HeadSpecialChars head_spec_char_;
    TailSpecialChars tail_spec_char_;
    std::string name_;
    std::vector<std::string> args_;
    std::size_t index_;
    bool success_; // Parsing status
};

void ReadParameters(PolySolver& solver, std::filesystem::path path) noexcept;

#endif // SCEN_PARSING_LINE_H
