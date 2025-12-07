export module scint.core.option;

import std;

namespace scint {

[[noreturn]] inline void option_unwrap_failed() noexcept {
    // TODO: log
    std::abort();
}

struct none_t {
    explicit constexpr none_t() = default;
};

inline constexpr none_t None{};

export template <typename T>
class Option {
    static_assert(!std::is_reference_v<T>, "Option<T&> is not supported; use std::reference_wrapper instead.");
    static_assert(!std::is_void_v<T>,      "Option<void> is not supported; use a sentinel type instead.");
public:

    // Empty by default.
    constexpr Option() noexcept
        : has_value_(false), storage_{} {}

    // Explicit empty case.
    explicit constexpr Option(none_t) noexcept
        : Option() {}

    // Construct from a value (copy).
    explicit constexpr Option(const T& value)
        : has_value_(true) {
        std::construct_at(std::addressof(storage_.value), value);
    }

    // Construct from a value (move).
    explicit constexpr Option(T&& value) noexcept(std::is_nothrow_move_constructible_v<T>)
        : has_value_(true) {
        std::construct_at(std::addressof(storage_.value), std::move(value));
    }

    ~Option() {
        reset();
    }

    [[nodiscard]] static constexpr Option some(T value)
        noexcept(std::is_nothrow_move_constructible_v<T>) {
        return Option(std::move(value));
    }

    [[nodiscard]] static constexpr Option none() noexcept {
        return Option{};
    }

    [[nodiscard]] constexpr bool is_some() const noexcept { return has_value_; }
    [[nodiscard]] constexpr bool is_none() const noexcept { return !has_value_; }

    // Pointer-like access.
    [[nodiscard]] constexpr T* operator->() noexcept {
        if (!has_value_) [[unlikely]] {
            option_unwrap_failed();
        }
        return std::addressof(storage_.value);
    }

    [[nodiscard]] constexpr const T* operator->() const noexcept {
        if (!has_value_) [[unlikely]] {
            option_unwrap_failed();
        }
        return std::addressof(storage_.value);
    }

    [[nodiscard]] constexpr T& operator*() & noexcept {
        if (!has_value_) [[unlikely]] {
            option_unwrap_failed();
        }
        return storage_.value;
    }

    [[nodiscard]] constexpr const T& operator*() const & noexcept {
        if (!has_value_) [[unlikely]] {
            option_unwrap_failed();
        }
        return storage_.value;
    }

    [[nodiscard]] constexpr T&& operator*() && noexcept {
        if (!has_value_) [[unlikely]] {
            option_unwrap_failed();
        }
        return std::move(storage_.value);
    }

    [[nodiscard]] constexpr const T&& operator*() const && noexcept {
        if (!has_value_) [[unlikely]] {
            option_unwrap_failed();
        }
        return std::move(storage_.value);
    }

    // Direct value access (debug-checked).
    [[nodiscard]] constexpr T& value() & {
        if (!has_value_) [[unlikely]] {
            option_unwrap_failed();
        }
        return storage_.value;
    }

    [[nodiscard]] constexpr const T& value() const & {
        if (!has_value_) [[unlikely]] {
            option_unwrap_failed();
        }
        return storage_.value;
    }

    [[nodiscard]] constexpr T&& value() && {
        if (!has_value_) [[unlikely]] {
            option_unwrap_failed();
        }
        return std::move(storage_.value);
    }

    [[nodiscard]] constexpr const T&& value() const && {
        if (!has_value_) [[unlikely]] {
            option_unwrap_failed();
        }
        return std::move(storage_.value);
    }

    // Return contained value or a fallback.
    [[nodiscard]] constexpr T value_or(T default_value) const {
        if (has_value_) {
            return storage_.value;
        }
        return std::move(default_value);
    }

    [[nodiscard]] constexpr T unwrap_or_default() const &
        noexcept(std::is_nothrow_default_constructible_v<T> &&
                 std::is_nothrow_copy_constructible_v<T>) {
        if (has_value_) {
            return storage_.value;
        }
        return T{};
    }

    template <typename... Args>
    constexpr T& emplace(Args&&... args) {
        reset();
        std::construct_at(std::addressof(storage_.value),
                          std::forward<Args>(args)...);
        has_value_ = true;
        return storage_.value;
    }

    constexpr void reset() noexcept {
        if (has_value_) {
            std::destroy_at(std::addressof(storage_.value));
            has_value_ = false;
        }
    }

private:
    bool has_value_;

    union Storage {
        char dummy; // keeps union trivially constructible
        T    value;

        constexpr Storage() noexcept : dummy{} {}
        ~Storage() {}
    } storage_;
};

// Helper for type-deducing construction: some(42) -> Option<int>
template <typename T>
[[nodiscard]] constexpr auto some(T&& value) {
    using U = std::decay_t<T>;
    return Option<U>::some(std::forward<T>(value));
}

} // namespace scint
