#include "io_auxiliary.h"
#include <exception>

std::string
time_to_string(const std::filesystem::file_time_type &ftime) noexcept
{
    std::time_t cftime = std::chrono::system_clock::to_time_t(
        std::chrono::file_clock::to_sys(ftime));
    std::string str = std::asctime(std::localtime(&cftime));
    str.pop_back(); // rm the trailing '\n' put by `asctime`
    return str;
}

std::vector<std::string_view> split_string_view_to_v(std::string_view init,
                                                     const char sep)
{
    expect<Error_action::logging, std::length_error>(
        [&]() { return init.size() > 0; }, std::to_string(init.size()).c_str());
    std::vector<std::string_view> result;
    result.reserve(2);
    std::size_t prev{0};
    for (std::size_t current(init.find(sep, prev)); current != init.npos;
         prev = current + 1, current = init.find(sep, prev)) {
        result.push_back(
            static_cast<std::string_view>(init.substr(prev, current - prev)));
    }
    result.push_back(static_cast<std::string_view>(init.substr(prev)));
    result.shrink_to_fit();
    return result;
}

std::vector<std::string_view> split_string_view_to_v(
    std::string_view init) // if get rid of expect-assertions, function can be
                           // declared constexpr
{
    return split_string_view_to_v(init, ' ');
}

bool check_rmod(const std::filesystem::path &p) noexcept
{
    namespace fs = std::filesystem;
    if (!fs::exists(p)) {
        std::cout << p << " doesn't exist" << std::endl;
        return false;
    } else if ((fs::status(p).permissions() & fs::perms::owner_read) !=
               fs::perms::owner_read) {
        std::cout << p << " isn't readable" << std::endl;
        return false;
    } else if ((fs::is_empty(p))) {
        std::cout << p << " is empty" << std::endl;
        return false;
    }
    return true;
}

std::tuple<solver_types, std::filesystem::path>
get_path_to_file_in_dir(const std::filesystem::path &dir, int pos,
                        std::string_view postfix)
{
    int cnt{0};
    for (auto const &dir_entry : std::filesystem::directory_iterator{
             dir, std::filesystem::directory_options::skip_permission_denied}) {
        using enum std::filesystem::perms;
        auto file_perms = std::filesystem::status(dir_entry).permissions();
        if ((file_perms & owner_read) == owner_read &&
            dir_entry.path().string().ends_with(postfix)) {
            if (cnt < pos)
                ++cnt;
            if (cnt == pos) {
                using enum solver_types;
                solver_types solver_type = unknown;
                std::string solver_name;
                std::ifstream fin(dir_entry.path());
                if (fin.is_open()) {
                    while (std::getline(fin, solver_name)) {
                        if (solver_name[0] == '!') {
                            auto found =
                                solver_types_table.find(solver_name.substr(1));
                            if (found != solver_types_table.end())
                                solver_type = found->second;
                            break;
                        }
                    }
                    fin.close();
                }
                return {solver_type, dir_entry.path()};
            }
        }
    }
    throw std::range_error("end of directory reached");
}

std::string get_format_by_left_side_impl(
    std::initializer_list<std::string_view> args) noexcept
{
    std::string fmt;
    for (auto const &it : args) {
        fmt += it;
    }
    fmt +=
        "{:.>" + std::to_string(w.ws_col - static_cast<int>(fmt.size())) + "}";
    return fmt;
}

int print_filenames(const std::filesystem::path &dir,
                    std::string_view postfix) noexcept
{
    int cnt{0};

#ifdef __cpp_lib_format
    std::string fmt =
        "      {:>2} : ";     // supposing you have less than 100 solvers..
    const int init_size = 11; // depending on fmt
    using namespace std::literals::string_literals;
    using namespace io_constants;
    fmt_add_align(fmt, {{".<"s, max_file_name_size},
                        {".<"s, max_solver_name_size},
                        {".>"s, w.ws_col - init_size - max_file_name_size -
                                    max_solver_name_size}});

#endif
    for (auto const &dir_entry : std::filesystem::directory_iterator{
             dir, std::filesystem::directory_options::skip_permission_denied}) {
        auto file_perms = std::filesystem::status(dir_entry).permissions();
        using enum std::filesystem::perms;
        if ((file_perms & owner_read) == owner_read &&
            dir_entry.path().string().ends_with(postfix)) {
            ++cnt;
            std::ifstream fin(dir_entry.path());
#ifdef __cpp_lib_format
            std::string solver_type_read;
            if (fin.is_open()) {
                while (std::getline(fin, solver_type_read)) {
                    if (solver_type_read[0] == '#')
                        continue;
                    else if (solver_type_read[0] == '!') {
                        solver_type_read.erase(0, 1);
                        auto found = solver_types_table.find(solver_type_read);
                        if (found != solver_types_table.end())
                            solver_type_read = found->first;
                        else
                            solver_type_read = "unknown";
                        break;
                    } else {
                        solver_type_read = "unknown";
                        break;
                    }
                }
                fin.close();
            }
            std::string rp_str{static_cast<std::string>(
                std::filesystem::relative(dir_entry.path(), dir))};
            std::string time_str{time_to_string(dir_entry.last_write_time())};
            std::cout << std::vformat(
                             fmt, std::make_format_args(
                                      cnt, rp_str, solver_type_read, time_str))
                      << std::endl;
#else
            std::cout << cnt << " : " << dir_entry.path().string() << std::endl;
#endif
        }
    }
    return cnt;
}

int choose_in_range(const int min, const int max)
{
    if (min == max)
        return min;
    std::string read;
    while (std::getline(std::cin, read)) {
        try {
            int choose = std::stoi(read);
            if (choose >= min && choose <= max)
                return choose;
            else
                std::cout << "please, choose value in range [" << min << ", "
                          << max << "]" << std::endl;
        } catch (std::invalid_argument &err) {
            if (!read.empty())
                std::cout << "enter only integers" << std::endl;
            else
                std::cout << "~empty input~" << std::endl;
            continue;
        }
    }
}
