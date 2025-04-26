#include "io.h"

#include <chrono>
#include <format>
#include <fstream>
#include <iostream>
#include <sys/ioctl.h>
#include <unordered_map>

#include "Solver_Lagrange_1D.h"
#include "error_handling.h"
#include "iSolver.h"
#include "parsing_line.h"

extern struct winsize w;

namespace {
inline constexpr std::size_t qAscTimeStrSize = 25;
inline constexpr std::size_t qMaxFilenameSize = 35;
inline constexpr std::size_t qMaxSolvernameSize = 15;
std::unordered_map<std::string, Solvers> SolversNameTypeTable{
    {"Lagrange1D", Solvers::qLagrange1D},
    {"WENO3_1D", Solvers::qWENO3_1D},
    {"Godunov1D", Solvers::qGodunov1D}};

std::size_t ChooseFileNumberInRange(const std::size_t min,
                                    const std::size_t max) noexcept
// @returns : a number of a entered via std::cin file
// Checks if file is readable, chosen number fits in [min, max]
{
    if (min == max)
        return min;
    std::string read;
    while (std::getline(std::cin, read)) {
        try {
            std::size_t choose = std::stoul(read);
            if (choose >= min && choose <= max)
                return choose;
            std::cout
                << "please, choose value in range ["
                << min
                << ", "
                << max
                << "]"
                << std::endl;
        } catch (std::invalid_argument& err) {
            if (!read.empty())
                std::cout << "enter only integers" << std::endl;
            else
                std::cout << "~empty input~" << std::endl;
            continue;
        }
    }
    return -1;
}

std::string GetScenSolverName(const fs::path& file) noexcept
// @does : Opens `file`, searches for a solver name
// @returns : Solver's name
// If read name is found in table prints it, else prints "unknown"
{
    std::ifstream fin(file);
    if (fin.is_open()) {
        std::string line;
        ScenParsingLine parsed;
        while (std::getline(fin, line)) {
            parsed.Load(line);
            if (!parsed.is_SolverType()) {
                continue;
            }
            auto found = SolversNameTypeTable.find(parsed.get_args()[0]);
            if (found != SolversNameTypeTable.end()) {
                return found->first;
            }
        }
    }
    return std::string("unknown");
}

std::string ConvertFSTimeToString(const fs::file_time_type& time) noexcept
{
    std::time_t chrono_time = std::chrono::system_clock::to_time_t(
        std::chrono::file_clock::to_sys(time));
    std::string str = std::asctime(std::localtime(&chrono_time));
    str.pop_back(); // rm the trailing '\n' put by `asctime`
    return str;
}

#ifdef __cpp_lib_format
inline void FmtAddAlign(std::string& fmt,
                        std::string_view align,
                        const std::vector<std::size_t>& sizes) noexcept
// @does : Appends to `fmt` sizes with a specified align for ALL
// @returns : void
{
    for (auto it : sizes) {
        fmt += "{:";
        fmt += align;
        fmt += std::to_string(it);
        fmt += "}";
    }
}

inline void FmtAddAlign(std::string& fmt,
                        const std::vector<std::pair<std::string, std::size_t>>&
                            align_n_sizes) noexcept
// @does : Appends to `fmt` sizes with a specified align for EACH
{
    for (auto [align, size] : align_n_sizes)
        FmtAddAlign(fmt, align, {size});
}
#endif // __cpp_lib_format

} // namespace

bool IsReadable(const fs::path& p) noexcept
{
    if (!fs::exists(p)) {
        return false;
    } else if ((fs::status(p).permissions() & fs::perms::owner_read)
               != fs::perms::owner_read) {
        return false;
    } else if ((fs::is_empty(p))) {
        return false;
    }
    return true;
}

std::pair<Solvers, fs::path> ChooseFileInDir(const fs::path& dir,
                                             std::size_t min,
                                             std::size_t max,
                                             std::string_view postfix) noexcept
{
    std::size_t chosen = ChooseFileNumberInRange(min, max);
    std::size_t cnt = 0;
    for (const auto& dir_entry : std::filesystem::directory_iterator{
             dir, std::filesystem::directory_options::skip_permission_denied}) {
        using enum std::filesystem::perms;
        auto file_perms = std::filesystem::status(dir_entry).permissions();
        if (!((file_perms & owner_read)
              == owner_read
              && dir_entry.path().string().ends_with(postfix))) {
            continue;
        }
        if (++cnt != chosen) {
            continue;
        }

        ScenParsingLine parsed_line;
        std::string line;
        std::ifstream fin(dir_entry.path());
        if (fin.is_open()) {
            while (std::getline(fin, line)) {
                parsed_line.Load(line);
                if (parsed_line.is_SolverType()) {
                    break;
                }
            }
            if (fin.eof()) {
                return {Solvers::qUnknown, dir_entry.path()};
            }
            Solvers solver_type = Solvers::qUnknown;
            auto found = SolversNameTypeTable.find(parsed_line.get_args()[0]);
            if (found != SolversNameTypeTable.end())
                solver_type = found->second;
            fin.close();
            return {solver_type, dir_entry.path()};
        }
    }
    return {};
}

std::size_t PrintFilenames(const fs::path& dir,
                           std::string_view postfix) noexcept
{
    int cnt{0};
#ifdef __cpp_lib_format
    using namespace std::literals::string_literals;
    std::string fmt = "      {:>2} : ";
    // supposing you have less than 100 scenarios..
    const std::size_t init_size = 11; // depending on fmt
    FmtAddAlign(
        fmt, {{".<"s, qMaxFilenameSize},
              {".<"s, qMaxSolvernameSize},
              {".>"s,
               w.ws_col - init_size - qMaxFilenameSize - qMaxSolvernameSize}});
#endif // __cpp_lib_format
    for (const auto& dir_entry : fs::directory_iterator{
             dir, fs::directory_options::skip_permission_denied}) {
        using enum std::filesystem::perms;
        std::string rp_str = std::filesystem::relative(dir_entry.path(), dir);
        if (!rp_str.ends_with(postfix)) {
            continue;
        }
        ++cnt;
        if constexpr (__cpp_lib_format) {
            if (w.ws_col
                < fmt.size()
                + init_size
                + qMaxFilenameSize
                + qMaxSolvernameSize) {
                std::cout << cnt << " : " << rp_str << std::endl;
                continue;
            }
            std::string solver_name_read = GetScenSolverName(dir_entry.path());
            std::string time_str{
                ConvertFSTimeToString(dir_entry.last_write_time())};
            std::cout
                << std::vformat(fmt, std::make_format_args(cnt, rp_str,
                                                           solver_name_read,
                                                           time_str))
                << std::endl;
        } else {
            std::cout << cnt << " : " << rp_str << std::endl;
        }
    }
    return cnt;
}

void ReadParameters(PolySolver& solver, fs::path path) noexcept
{
    dash::Expect<dash::ErrorAction::qTerminating, std::exception>(
        [&solver]() { return !std::holds_alternative<std::monostate>(solver); },
        "Incorrect solver type provided");
    std::ifstream fin(path);
    dash::Expect<dash::ErrorAction::qTerminating, std::exception>(
        [&fin]() { return fin.is_open(); }, "Can't open a file");
    ScenParsingLine line;
    ScenParsingLine parent_line;
    std::string read;
    while (std::getline(fin, read)) {
        line.Load(read);
        // std::visit(dash::overloaded{[]([[maybe_unused]] std::monostate& arg)
        // {},
        //                             [&line]([[maybe_unused]] auto& arg) {
        //                                 arg.Parse(line);
        //                             }},
        //            solver);
    }
}
