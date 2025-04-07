#ifndef PARSER_H
#define PARSER_H

#include "concepts.h"
#include "error_handling.h"
#include "iSolver.h"
#include "parsing_line.h"

template <typename S>
    requires IsCRTPBaseOfValue<iSolver, S>
class Parser {
public:
    Parser(S& solver) noexcept : solver_(solver), grown_() {}
    void ParseLine(const ScenParsingLine& line);
    // @does : Parses a given line into a `solver_` `parameters`
    // Throws an exception if can't parse a line appropriately
    void SetGroupOwner_Head(const ScenParsingLine& line) noexcept;
    // @does : Checks what group parsing variable belongs to. Checks for
    // opening/closing curly braces i.e. group declarations/endings
    bool SetGroupOwner_Name(
        std::unordered_map<std::string_view,
                           const typename S::GroupOwner>::const_iterator it,
        const ScenParsingLine& line) noexcept;
    void
    AssignValue(const ScenParsingLine& line,
                std::unordered_map<std::string_view,
                                   const typename S::GroupOwner>::const_iterator
                    it) noexcept;
private:
    Parser() = default;
    S& solver_;
    iSolver<S>::GroupOwner grown_;
};

template <typename S>
    requires IsCRTPBaseOfValue<iSolver, S>
void Parser<S>::ParseLine(const ScenParsingLine& line)
{
    SetGroupOwner_Head(line);
    const auto it = solver_.FindInParsingTable(line.name());
    if (!SetGroupOwner_Name(it, line)) {
        return;
    }
    AssignValue(line, it);
}

template <typename S>
    requires IsCRTPBaseOfValue<iSolver, S>
void Parser<S>::SetGroupOwner_Head(const ScenParsingLine& line) noexcept
{
    if (line.head_spec_char()
        == ScenParsingLine::HeadSpecialChars::qCommentary) {
        return;
    }
    if (line.head_spec_char() == ScenParsingLine::HeadSpecialChars::qEndGroup) {
        grown_.Reset();
    }
}

template <typename S>
    requires IsCRTPBaseOfValue<iSolver, S>
bool Parser<S>::SetGroupOwner_Name(
    std::unordered_map<std::string_view,
                       const typename S::GroupOwner>::const_iterator it,
    const ScenParsingLine& line) noexcept
{
    const auto found = solver_.FindInGroupNames(line.name());
    if (!found) {
        return false;
    }
    expect<ErrorAction::qThrowing, custom_exceptions::invalid_group_control>(
        [*this] { return !grown_.IsSet(); }, "Parser : Group is already set");
    expect<ErrorAction::qThrowing, custom_exceptions::invalid_group_control>(
        [&line] {
            return line.tail_spec_char()
                   == ScenParsingLine::TailSpecialChars::qEndGroup;
        },
        "Parser : No group begin declared");
    grown_ = {line.name(), it->second.ptr()};
    return true;
}

template <typename S>
    requires IsCRTPBaseOfValue<iSolver, S>
void Parser<S>::AssignValue(
    const ScenParsingLine& line,
    std::unordered_map<std::string_view,
                       const typename S::GroupOwner>::const_iterator
        it) noexcept
{
}
#endif // PARSER_H
