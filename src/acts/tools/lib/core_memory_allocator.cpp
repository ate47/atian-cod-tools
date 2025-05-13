#include <includes.hpp>
#include <unit_test.hpp>
#include <core/memory_allocator.hpp>

namespace {
	class TestDelete {
	public:
		int* step;
		TestDelete(int* step) : step(step) {
			*step = 1;
		}
		~TestDelete() {
			*step = 3;
		}

		void TestFunc() {
			*step = 2;
		}
	};

	core::memory_allocator::MemoryAllocator CreateAlloc(const char** out, int* step) {
		core::memory_allocator::MemoryAllocator memalloc2{};

		
		*out = memalloc2.CloneStr("test");
		ASSERT_VAL("check test", !std::strcmp("test", *out));
		TestDelete* test{ memalloc2.New<TestDelete>(step) };
		ASSERT_VAL("check step", *step == 1);
		test->TestFunc();
		ASSERT_VAL("check step", *step == 2);

		return memalloc2;
	}

	void memory_alloc_test() {
		const char* test{};
		int step{};
		core::memory_allocator::MemoryAllocator memalloc1{ CreateAlloc(&test, &step) };
		
		ASSERT_VAL("check test", !std::strcmp("test", test));

		ASSERT_VAL("check step out", step == 2);
		
		core::memory_allocator::MemoryAllocator memalloc2{ std::move(memalloc1) };
		
		ASSERT_VAL("check test out", !std::strcmp("test", test));
		ASSERT_VAL("check step out", step == 2);

		ASSERT_VAL(std::format("empty memall2 {}", memalloc2.Count()), memalloc2.Count() == 2);

		memalloc2.FreeAll();

		ASSERT_VAL("check step out", step == 3);
	}

	ADD_TEST(memory_alloc_test, memory_alloc_test);

}