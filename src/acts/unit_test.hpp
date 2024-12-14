#pragma once
#include <core/logs.hpp>

namespace acts::unit_test {
	void AssertImpl(const std::string& msg, const char* file, size_t line, bool value);

	class UnitTest {
	public:
		const char* id;
		void(*func)();
		UnitTest(uint64_t uid, const char* id, void(*func)());

		bool HandleTest();
	};
}

#define ASSERT_VAL(msg, val) acts::unit_test::AssertImpl(std::format("{} - {}", #val, msg), LOG_GET_LOG_REF_STR.data(), __LINE__, val)
#define ADD_TEST(id, func) static acts::unit_test::UnitTest __unittest_##id(hash::Hash64(#id), #id, func)