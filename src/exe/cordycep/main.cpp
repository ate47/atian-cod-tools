#include <acts_api/internal.hpp>
#include <vector>

int main(int argc, const char* argv[]) {
	std::vector<const char*> args{};
	args.reserve((size_t)argc + 2);
	args.push_back(argv[0]);
	args.push_back("fastfilecordycep");
	for (size_t i = 1; i < argc; i++) {
		args.push_back(argv[i]);
	}
	args.push_back(nullptr);

	return MainActs(argc + 1, args.data());
}
