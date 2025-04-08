#ifndef PARSER_H
#define PARSER_H

#include "concepts.h"
#include "error_handling.h"
#include "iSolver.h"
#include "parsing_line.h"
#include <charconv>

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
    bool
    SetGroupOwner_Name(const iSolver<Solver_Lagrange_1D>::ParameterInfo object,
                       const ScenParsingLine& line) noexcept;
    void AssignValue(const iSolver<Solver_Lagrange_1D>::ParameterInfo object,
                     const ScenParsingLine& line) noexcept;
    iSolver<S>::ParameterInfo GetParameterInfo(iSolver<S>::ParameterInfo object,
                                               std::string_view name) noexcept;
private:
    Parser() = default;
    S& solver_;
    iSolver<S>::ParameterInfo grown_;
};

template <typename S>
    requires IsCRTPBaseOfValue<iSolver, S>
void Parser<S>::ParseLine(const ScenParsingLine& line)
{
    if (line.head_spec_char()
        == ScenParsingLine::HeadSpecialChars::qCommentary
        || line.head_spec_char()
        == ScenParsingLine::HeadSpecialChars::qFileInfo) {
        return;
    }
    SetGroupOwner_Head(line);
    auto found = solver_.FindInParsingTable(line.name());
    typename S::ParameterInfo found_obj;
    if (found != solver_.FindInParsingTable("bebra")) {
        found_obj = found->second;
    }
    if (SetGroupOwner_Name(found_obj, line)) {
        return;
    }
    const auto parameter_info = GetParameterInfo(found_obj, line.name());
    AssignValue(parameter_info, line);
}

template <typename S>
    requires IsCRTPBaseOfValue<iSolver, S>
iSolver<S>::ParameterInfo
Parser<S>::GetParameterInfo(iSolver<S>::ParameterInfo object,
                            std::string_view name) noexcept
{
    if (!grown_.IsSet()) {
        return object;
    }
    using namespace std::literals::string_view_literals;
    decltype(object) result;
    if (grown_.type() == "Wall"sv) {
        result = static_cast<Wall<S>*>(grown_.ptr())
                     ->parsing_table.find(name)
                     ->second;
    }
    return result;
}

template <typename S>
    requires IsCRTPBaseOfValue<iSolver, S>
void Parser<S>::SetGroupOwner_Head(const ScenParsingLine& line) noexcept
{
    if (line.head_spec_char() == ScenParsingLine::HeadSpecialChars::qEndGroup) {
        grown_.Reset();
    }
}

template <typename S>
    requires IsCRTPBaseOfValue<iSolver, S>
bool Parser<S>::SetGroupOwner_Name(
    const iSolver<Solver_Lagrange_1D>::ParameterInfo object,
    const ScenParsingLine& line) noexcept
{
    const auto found = solver_.FindInGroupNames(line.name());
    if (found == solver_.FindInGroupNames("bebra")) {
        return false;
    }
    expect<ErrorAction::qThrowing, custom_exceptions::invalid_group_control>(
        [*this] { return !grown_.IsSet(); }, "Parser : Group is already set");
    expect<ErrorAction::qThrowing, custom_exceptions::invalid_group_control>(
        [&line] {
            return line.tail_spec_char()
                   == ScenParsingLine::TailSpecialChars::qBeginGroup;
        },
        "Parser : No group begin declared");
    grown_ = {line.name(), object.ptr()};
    return true;
}

template <typename S>
    requires IsCRTPBaseOfValue<iSolver, S>
void Parser<S>::AssignValue(
    const iSolver<Solver_Lagrange_1D>::ParameterInfo object,
    const ScenParsingLine& line) noexcept
{
    using namespace std::literals::string_view_literals;
    if (object.type() == "double"sv) {
        *static_cast<double*>(object.ptr()) = std::stod(line[0]);
    } else if (object.type() == "int"sv) {
        *static_cast<int*>(object.ptr()) = std::stoi(line[0]);
    } else if (object.type() == "uint"sv) {
        *static_cast<uint*>(object.ptr()) = std::stoul(line[0]);
    } else if (object.type() == "string"sv) {
        *static_cast<std::string*>(object.ptr()) = line[0];
    }
}
#endif // PARSER_H
