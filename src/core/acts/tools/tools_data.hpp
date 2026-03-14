#pragma once

namespace tool {
	enum errorcode : int32_t {
		OK = 0,
		BASIC_ERROR = -1,
		BAD_USAGE = -2
	};

	typedef std::function<int(int argc, const char* argv[])> toolfunctionnf;

	/*
	 * Test if the tool args contains enough arguments (argv[0] = acts argv[1] = tool name)
	 * @param argc argc
	 * @param count required count
	 * @return true if enough, false otherwise
	 */
	constexpr bool NotEnoughParam(int argc, size_t count) {
		return argc < count + 2;
	}
}