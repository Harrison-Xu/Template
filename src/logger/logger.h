/*
 * SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstring>
#include <sstream>
#include <string>
#include <utility>

// Compile-time log filtering. Larger values keep more verbose logs.
// Example: -DAPP_LOG_LEVEL=APP_LOG_LEVEL_DEBUG keeps DEBUG and above.
#define APP_LOG_LEVEL_OFF   0
#define APP_LOG_LEVEL_FATAL 1
#define APP_LOG_LEVEL_ERROR 2
#define APP_LOG_LEVEL_WARN  3
#define APP_LOG_LEVEL_INFO  4
#define APP_LOG_LEVEL_DEBUG 5
#define APP_LOG_LEVEL_TRACE 6

#ifndef APP_LOG_LEVEL
#define APP_LOG_LEVEL APP_LOG_LEVEL_TRACE
#endif

#if APP_LOG_LEVEL < APP_LOG_LEVEL_OFF || APP_LOG_LEVEL > APP_LOG_LEVEL_TRACE
#error "APP_LOG_LEVEL must be one of APP_LOG_LEVEL_OFF/FATAL/ERROR/WARN/INFO/DEBUG/TRACE"
#endif

namespace logger {

namespace detail {

template<typename T>
std::string arg_to_string(T&& value) {
    std::ostringstream stream;
    stream << std::forward<T>(value);
    return stream.str();
}

inline std::string arg_to_string(const char* value) {
    return value ? value : "";
}

inline std::string arg_to_string(char* value) {
    return value ? value : "";
}

inline void format_into(std::string& out, const char* fmt_str, const char* cursor) {
    out += cursor ? cursor : fmt_str;
}

template<typename T, typename... Args>
void format_into(std::string& out, const char* fmt_str, const char* cursor, T&& value, Args&&... args) {
    const char* match = std::strstr(cursor, "{}");
    if (!match) {
        out += cursor;
        return;
    }

    out.append(cursor, static_cast<std::size_t>(match - cursor));
    out += arg_to_string(std::forward<T>(value));
    format_into(out, fmt_str, match + 2, std::forward<Args>(args)...);
}

template<typename... Args>
std::string format_message(const char* fmt_str, Args&&... args) {
    if (!fmt_str) {
        return {};
    }

    std::string out;
    format_into(out, fmt_str, fmt_str, std::forward<Args>(args)...);
    return out;
}

} // namespace detail

enum class LogLevel {
    TRACE = 0,
    DEBUG,
    INFO,
    WARN,
    ERROR,
    FATAL,
    OFF
};

enum class ColorMode {
    AUTO,
    ENABLE,
    DISABLE
};

class Logger {
public:
    static void init();

    static void set_level(LogLevel level);
    static LogLevel level();

    static void set_tag(const char* tag);
    static const char* tag();

    static void set_color_mode(ColorMode mode);

    static void set_timestamp_enabled(bool enabled);
    static bool timestamp_enabled();

    static constexpr int compile_level() { return APP_LOG_LEVEL; }
    static constexpr bool should_compile_trace() { return APP_LOG_LEVEL >= APP_LOG_LEVEL_TRACE; }
    static constexpr bool should_compile_debug() { return APP_LOG_LEVEL >= APP_LOG_LEVEL_DEBUG; }
    static constexpr bool should_compile_info() { return APP_LOG_LEVEL >= APP_LOG_LEVEL_INFO; }
    static constexpr bool should_compile_warn() { return APP_LOG_LEVEL >= APP_LOG_LEVEL_WARN; }
    static constexpr bool should_compile_error() { return APP_LOG_LEVEL >= APP_LOG_LEVEL_ERROR; }
    static constexpr bool should_compile_fatal() { return APP_LOG_LEVEL >= APP_LOG_LEVEL_FATAL; }

    template<typename... Args>
    static void trace(const char* fmt_str, Args&&... args)
    {
        log(LogLevel::TRACE, detail::format_message(fmt_str, std::forward<Args>(args)...));
    }

    template<typename... Args>
    static void debug(const char* fmt_str, Args&&... args)
    {
        log(LogLevel::DEBUG, detail::format_message(fmt_str, std::forward<Args>(args)...));
    }

    template<typename... Args>
    static void info(const char* fmt_str, Args&&... args)
    {
        log(LogLevel::INFO, detail::format_message(fmt_str, std::forward<Args>(args)...));
    }

    template<typename... Args>
    static void warn(const char* fmt_str, Args&&... args)
    {
        log(LogLevel::WARN, detail::format_message(fmt_str, std::forward<Args>(args)...));
    }

    template<typename... Args>
    static void error(const char* fmt_str, Args&&... args)
    {
        log(LogLevel::ERROR, detail::format_message(fmt_str, std::forward<Args>(args)...));
    }

    template<typename... Args>
    static void fatal(const char* fmt_str, Args&&... args)
    {
        log(LogLevel::FATAL, detail::format_message(fmt_str, std::forward<Args>(args)...));
    }

    template<typename... Args>
    static void trace_at(const char* file,
                         int line,
                         const char* fmt_str,
                         Args&&... args)
    {
        log(LogLevel::TRACE, detail::format_message(fmt_str, std::forward<Args>(args)...), file, line);
    }

    template<typename... Args>
    static void debug_at(const char* file,
                         int line,
                         const char* fmt_str,
                         Args&&... args)
    {
        log(LogLevel::DEBUG, detail::format_message(fmt_str, std::forward<Args>(args)...), file, line);
    }

    template<typename... Args>
    static void info_at(const char* file,
                        int line,
                        const char* fmt_str,
                        Args&&... args)
    {
        log(LogLevel::INFO, detail::format_message(fmt_str, std::forward<Args>(args)...), file, line);
    }

    template<typename... Args>
    static void warn_at(const char* file,
                        int line,
                        const char* fmt_str,
                        Args&&... args)
    {
        log(LogLevel::WARN, detail::format_message(fmt_str, std::forward<Args>(args)...), file, line);
    }

    template<typename... Args>
    static void error_at(const char* file,
                         int line,
                         const char* fmt_str,
                         Args&&... args)
    {
        log(LogLevel::ERROR, detail::format_message(fmt_str, std::forward<Args>(args)...), file, line);
    }

    template<typename... Args>
    static void fatal_at(const char* file,
                         int line,
                         const char* fmt_str,
                         Args&&... args)
    {
        log(LogLevel::FATAL, detail::format_message(fmt_str, std::forward<Args>(args)...), file, line);
    }

private:
    static void log(LogLevel level,
                    const std::string& msg,
                    const char* file = nullptr,
                    int line = 0);

    static bool should_log(LogLevel level);

    static bool should_use_color();

private:
    inline static LogLevel level_ = LogLevel::INFO;
    inline static std::string tag_ = "";
    inline static ColorMode color_mode_ = ColorMode::AUTO;
    inline static bool timestamp_enabled_ = false;
};

} // namespace logger

#if APP_LOG_LEVEL >= APP_LOG_LEVEL_TRACE
#define LOG_TRACE(...) ::logger::Logger::trace_at(__FILE__, __LINE__, __VA_ARGS__)
#else
#define LOG_TRACE(...) do {} while (0)
#endif

#if APP_LOG_LEVEL >= APP_LOG_LEVEL_DEBUG
#define LOG_DEBUG(...) ::logger::Logger::debug_at(__FILE__, __LINE__, __VA_ARGS__)
#else
#define LOG_DEBUG(...) do {} while (0)
#endif

#if APP_LOG_LEVEL >= APP_LOG_LEVEL_INFO
#define LOG_INFO(...) ::logger::Logger::info_at(__FILE__, __LINE__, __VA_ARGS__)
#else
#define LOG_INFO(...) do {} while (0)
#endif

#if APP_LOG_LEVEL >= APP_LOG_LEVEL_WARN
#define LOG_WARN(...) ::logger::Logger::warn_at(__FILE__, __LINE__, __VA_ARGS__)
#else
#define LOG_WARN(...) do {} while (0)
#endif

#if APP_LOG_LEVEL >= APP_LOG_LEVEL_ERROR
#define LOG_ERROR(...) ::logger::Logger::error_at(__FILE__, __LINE__, __VA_ARGS__)
#else
#define LOG_ERROR(...) do {} while (0)
#endif

#if APP_LOG_LEVEL >= APP_LOG_LEVEL_FATAL
#define LOG_FATAL(...) ::logger::Logger::fatal_at(__FILE__, __LINE__, __VA_ARGS__)
#else
#define LOG_FATAL(...) do {} while (0)
#endif
