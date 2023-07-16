#pragma once

#include <android/log.h>
#include <cstdio>

#include <utility>
#include <string_view>

#ifndef NDEBUG
#include <fmt/std.h>
#endif

namespace saglobal {
    extern const char* g_logTag;
}

namespace salog {
    int printFormat(int prio, const char* format, ...);
    int print(int prio, const char* msgStr);

    #ifndef NDEBUG
    template<typename... Args>
    auto coutFmt(int prio, fmt::format_string<Args...> fmt, Args&&... args)
    {
        uint64_t fioAllocated{};
        char* fioPtr{};

        // this FILE IO is dynamic allocated and grows up as required
        auto fio{open_memstream(&fioPtr, reinterpret_cast<size_t*>(&fioAllocated))};
        fmt::print(fio, fmt, std::forward<Args>(args)...);
        fclose(fio);
        // at this point, a null byte has wrote inside of fio string

        const auto out{print(prio, fioPtr)};
        free(fioPtr);
        return out;
    }
    #else
    template<typename... Args>
    auto coutFmt([[maybe_unused]] int prio, [[maybe_unused]] const std::string_view formatUnused,
                 [[maybe_unused]] Args&&... args)
    {
        return 0;
    }
    #endif

    void assertAbort(const char* cond, const char* fileName,
        int line, const char* format, ...);
}

#define SALOG_ASSERT(cond, fmt, ...)\
    if (!(cond))\
        salog::assertAbort(#cond, __FILE__, __LINE__, fmt, #__VA_ARGS__)

