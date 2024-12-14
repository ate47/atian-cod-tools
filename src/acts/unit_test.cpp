#include <includes.hpp>
#include <unit_test.hpp>
#include <cli/clicolor.hpp>

namespace acts::unit_test {
	std::unordered_map<uint64_t, UnitTest*>& GetTests() {
		static std::unordered_map<uint64_t, UnitTest*> tests{};
		return tests;
	}

	void AssertImpl(const std::string& msg, const char* file, size_t line, bool value) {
		if (value) {
			LOG_TRACE("{}@{}: {} -> {}OK", file, line, msg, cli::clicolor::Color(1, 5, 1));
			return;
		}
		LOG_ERROR("--- ASSERTION ERROR ---");
		LOG_ERROR("file .... {}@{}", file, line);
		LOG_ERROR("message . {}", msg);
		LOG_ERROR("-----------------------");

		throw std::runtime_error("assertion fail");
	}


	UnitTest::UnitTest(uint64_t uid, const char* id, void(*func)()) : id(id), func(func) {
		GetTests()[uid] = this;
	}

	bool UnitTest::HandleTest() {
		if (IsDebuggerPresent()) {
			func();
			return true; // use error handling
		}
		try {
			func();
			return true;
		}
		catch (std::runtime_error& re) {
			LOG_ERROR("{}", re.what());
			return false;
		}
	}

	namespace {
		int utest(int argc, const char* argv[]) {
			uint64_t tuid{ tool::NotEnoughParam(argc, 2) ? 0 : hash::Hash64(argv[2]) };

			size_t errors{};
			size_t tests{};
			for (auto& [uid, test] : GetTests()) {
				if (!tuid || uid == tuid) {
					tests++;
					LOG_INFO("Test - {}", test->id);
					
					if (test->HandleTest()) {
						LOG_INFO("{} -> {}OK", test->id, cli::clicolor::Color(1, 5, 1));
					}
					else {
						LOG_INFO("{} -> {}ERROR", test->id, cli::clicolor::Color(5, 1, 1));
						errors++;
					}
				}
			}

			if (!tests) {
				LOG_ERROR("No test found");
				return tool::BASIC_ERROR;
			}

			if (errors) {
				LOG_ERROR("Test(s) completed with {} error(s)", errors);
			}
			else {
				LOG_INFO("Test(s) completed with 0 error");
			}

			return errors ? tool::BASIC_ERROR : tool::OK;
		}

		void utesttest() {
			ASSERT_VAL("test", true);
			ASSERT_VAL("test", true);
			ASSERT_VAL("test", 1 < 2);
			ASSERT_VAL("test", 3 == 3);

		}

		ADD_TOOL(utest, "dev", " (test)", "start unit test", utest);
		ADD_TEST(utest, utesttest);
	}
}