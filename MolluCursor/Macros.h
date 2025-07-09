#pragma once


#ifdef _DEBUG
#    define DEBUG_SCOPED_STOP_WATCH(...) ScopedStopWatch(std::format(__VA_ARGS__))
#else
#    define DEBUG_SCOPED_STOP_WATCH(...) (void)(0)
#endif

#ifdef _DEBUG
#    define ASSERT(x, ...)                                                                                                         \
        do                                                                                                                         \
        {                                                                                                                          \
            if (!(x))                                                                                                              \
            {                                                                                                                      \
                fprintf(stderr, "Assertion failed: %s, file %s, line %d\n", std::format(__VA_ARGS__).c_str(), __FILE__, __LINE__); \
                __debugbreak();                                                                                                    \
            }                                                                                                                      \
        } while (0)
#else
#    define ASSERT(x, m_msg)                                                                                      \
        do                                                                                                        \
        {                                                                                                         \
            if (!(x)) { fprintf(stderr, "Assertion failed: %s, file %s, line %d\n", m_msg, __FILE__, __LINE__); } \
        } while (0)
#endif

namespace detail
{

void CrashImpl(std::string_view _msg);

}

#ifdef _DEBUG

// 디버그 모드에서는 __debugbreak()를 호출. 매크로 정의 -> 콜 스택 때문
#    define CRASH(...)                                   \
        do                                               \
        {                                                \
            detail::CrashImpl(std::format(__VA_ARGS__)); \
            __debugbreak();                              \
            std::terminate();                            \
        } while (0)

#else
#    define CRASH(...)                                   \
        do                                               \
        {                                                \
            detail::CrashImpl(std::format(__VA_ARGS__)); \
            std::terminate();                            \
        } while (0)

#endif

#define NODISCARD [[nodiscard]]