#ifndef AUXILIARY_FUNCTIONS_H
#define AUXILIARY_FUNCTIONS_H
#include <algorithm>
#include <atomic>
#include <bitset>
#include <cassert>
#include <chrono>
#include <climits>
#include <format>
#include <iostream>
#include <mutex>

namespace dash {
inline void rc_free_print(std::string_view msg) {
    static std::mutex           mtx;
    std::lock_guard<std::mutex> lock(mtx);
    std::cout << msg << std::flush;
}

enum class ErrorAction {
    qIgnore,
    qThrowing,
    qTerminating,
    qLogging
};
inline static constexpr ErrorAction qDefaultErrorAction{ErrorAction::qLogging};

////////Tour of C++ 2022 B.Stroustrup p.49/////////
template<
    ErrorAction action,
    typename exc,
    typename C>
constexpr void Expect(
    const C&           cond,
    const std::string& msg = "<no message provided>") {
    if constexpr (action == ErrorAction::qThrowing) {
        if (!cond()) {
            if constexpr (std::is_constructible_v<exc, const std::string&>) {
                throw exc(msg);
            } else {
                throw exc();
            }
        }
    }
    if constexpr (action == ErrorAction::qTerminating) {
        if (!cond()) {
            std::cerr << msg << std::endl;
            std::terminate();
        }
    }
    if constexpr (action == ErrorAction::qLogging) {
        if (!cond()) {
            std::cerr << "EXPECT logging: " << msg << std::endl;
        }
    }
    // Error_action::qIgnore --> nothing happens
}

template<typename F>
struct FinalAction {
    explicit FinalAction(F f) noexcept: act(f) {}

    ~FinalAction() noexcept { act(); }

    F act;
};

template<typename F>
[[nodiscard]]
inline auto Finally(F f) noexcept {
    return FinalAction{f};
}

// Type tag dispatching
template<typename T>
struct TheirOwnAddress {
    static constexpr char address = 0;
};

template<typename T>
inline constexpr const void* qTypeTag = &TheirOwnAddress<T>::address;

template<typename T>
inline static std::size_t type_tag() noexcept {
    return reinterpret_cast<std::size_t>(qTypeTag<T>);
}

// Prints time program spent in a scope
// Thread safe

template<typename TimerT = std::chrono::microseconds>
    requires std::is_convertible_v<
        TimerT,
        std::chrono::microseconds>
[[nodiscard]]
auto SetScopedTimer(std::string_view timer_name = "unnamed") noexcept {
    constinit static std::atomic<std::uint32_t> timer_id{0};
    timer_id++;
    const auto start_time = std::chrono::high_resolution_clock::now();
    return FinalAction{[timer_name, start_time]() {
        std::cout << std::format(
            "=============== {}; id : {} ===============\n",
            timer_name,
            timer_id.load());
        const auto final_time =
            std::chrono::high_resolution_clock::now() - start_time;
        using namespace std::chrono_literals;
        std::cout << std::format(
            " time : {}\n", std::chrono::duration_cast<TimerT>(final_time));
    }};
}

// Wrapper of enum bit flags
// If enum doesn't represent single-bit(or their intentional combinations) flags
// you may expect curious behavior in some operations
// Notabene : in c++23 std::bitset ops are constexpr
template<typename E>
    requires std::is_enum_v<E>
class Flag {
public:
    Flag(E rhs) noexcept: bits_(static_cast<std::underlying_type_t<E>>(rhs)) {}

    Flag()                       = default;
    Flag(const Flag&)            = default;
    Flag& operator=(const Flag&) = default;

    void reset() noexcept { bits_.reset(); }

    Flag& operator|=(const Flag& rhs) noexcept {
        bits_ |= rhs.bits_;
        return *this;
    }

    [[nodiscard]]
    Flag operator|(const Flag& rhs) const noexcept {
        Flag result  = *this;
        result      |= rhs;
        return result;
    }

    Flag& operator&=(const Flag& rhs) noexcept {
        bits_ &= rhs.bits_;
        return *this;
    }

    [[nodiscard]]
    Flag operator&(const Flag& rhs) const noexcept {
        Flag result  = *this;
        result      &= rhs;
        return result;
    }

    // Disable a certain flag(*= used since ~= is not a thing)
    Flag& operator*=(const Flag& rhs) noexcept {
        bits_ &= ~rhs.bits_;
        return *this;
    }

    void enable(const Flag& rhs) noexcept { *this |= rhs; }

    void disable(const Flag& rhs) noexcept { *this *= rhs; }

    Flag& operator~() noexcept {
        ~bits_;
        return *this;
    }

    [[nodiscard]]
    bool operator==(Flag rhs) const noexcept {
        return bits_ == rhs.bits_;
    }

    [[nodiscard]]
    bool operator!=(Flag rhs) const noexcept {
        return bits_ != rhs.bits_;
    }

    [[nodiscard]]
    bool any() const noexcept {
        return bits_.any();
    }

    [[nodiscard]]
    explicit operator bool() const noexcept {
        return any();
    }

    operator std::bitset<sizeof(E) * CHAR_BIT>() const noexcept {
        return bits_;
    }

private:
    std::bitset<sizeof(E) * CHAR_BIT> bits_;
};
}    // namespace dash
#endif    // AUXILIARY_FUNCTIONS_H
