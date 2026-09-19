#pragma once
#include <core/logs.hpp>

namespace acts::unit_test {
    void AssertImpl(const std::string& msg, const char* file, size_t line, bool value);

    class UnitTest {
      public:
        const char* id;
        void (*func)();
        UnitTest(uint64_t uid, const char* id, void (*func)());

        bool HandleTest();
    };
} // namespace acts::unit_test

#define ASSERT_VAL(msg, val)                                                                                           \
    acts::unit_test::AssertImpl(std::format("{} - {}", #val, msg), LOG_GET_LOG_REF_STR, __LINE__, val)
#define ASSERT_TRUE(msg, val) ASSERT_VAL(msg, val)
#define ASSERT_FALSE(msg, val) ASSERT_VAL(msg, !(val))
#define ASSERT_EQ(msg, expected, actual) ASSERT_VAL(msg, (expected) == (actual))
#define ASSERT_EQ_STR(msg, expected, actual) ASSERT_VAL(msg, std::string_view(expected) == std::string_view(actual))
#define ASSERT_EQ_WSTR(msg, expected, actual) ASSERT_VAL(msg, std::wstring_view(expected) == std::wstring_view(actual))
#define ASSERT_NEQ(msg, expected, actual) ASSERT_VAL(msg, (expected) != (actual))
#define ASSERT_NEQ_STR(msg, expected, actual) ASSERT_VAL(msg, std::string_view(expected) != std::string_view(actual))
#define ASSERT_NEQ_WSTR(msg, expected, actual) ASSERT_VAL(msg, std::wstring_view(expected) != std::wstring_view(actual))
#define ADD_TEST(id, func) static acts::unit_test::UnitTest __unittest_##id(::hash::Hash64(#id), #id, func)