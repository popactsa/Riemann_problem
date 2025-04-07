#ifndef SCEN_PARSING_LINE_H
#define SCEN_PARSING_LINE_H

#include <string>
#include <vector>

class ScenParsingLine {
    // Parsed line from scenario file with info for future variable values
    // parsed. Also used when performing IO operations(e.g. showing solver types
    // for .scen files).
    // <HeadSpecialChar> name {args_} <TailSpecialChar>
    // Lines with incorrect <HeadSpecialChar> will be set as commentaries
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

    ScenParsingLine() noexcept {};
    ScenParsingLine(const ScenParsingLine&) = default;
    explicit ScenParsingLine(const std::string& line) noexcept { *this = line; }
    ScenParsingLine& operator=(const std::string& rhs) noexcept;
    const std::string& operator[](const std::size_t index) const noexcept
    {
        return args_[index];
    }

    inline HeadSpecialChars head_spec_char() const noexcept
    {
        return head_spec_char_;
    }
    inline TailSpecialChars tail_spec_char() const noexcept
    {
        return tail_spec_char_;
    }
    inline const std::string& name() const noexcept { return name_; }
    inline operator bool() const noexcept { return success_; }
    inline const std::vector<std::string>& args() const noexcept
    {
        return args_;
    }
private:
    HeadSpecialChars head_spec_char_;
    TailSpecialChars tail_spec_char_;
    std::string name_;
    std::vector<std::string> args_;
    bool success_; // Parsing status
};

#endif // SCEN_PARSING_LINE_H
