#pragma once
#include <includes_shared.hpp>

/*
 * Utilities
 */
namespace utils {
	typedef long long Timestamp;
	/*
	 * Get current timestamp
	 * @return timestamp
	 */
	Timestamp GetTimestamp();

	template<typename S>
	constexpr size_t GetMaxSize() {
		return ~(~0ull << (sizeof(S) << 3)) >> (std::is_unsigned<S>() ? 0 : 1);
	}

	/*
	 * Get null stream
	 * @return stream
	 */
	std::ostream& NullStream();

	/*
	 * Return va string
	 * @param fmt format
	 * @return string
	 */
	char* va(const char* fmt, ...);

	/*
	 * Return va string
	 * @param fmt format
	 * @param ap va list
	 * @return string
	 */
	char* vap(const char* fmt, va_list ap);

	/*
	 * Read a file inside a string buffer
	 * @param path file path
	 * @param buffer buffer to write the size
	 * @param append append to end of the buffer
	 * @return if the file was read
	 */
	bool ReadFile(const std::filesystem::path& path, std::string& buffer, bool append = false);
	/*
	 * Read a file inside a string buffer
	 * @param path file path
	 * @param buffer buffer to write the size
	 * @param append append to end of the buffer
	 * @return if the file was read
	 */
	bool ReadFile(const std::filesystem::path& path, std::vector<byte>& buffer, bool append = false);
	/*
	 * Read a file inside a buffer
	 * @param path file path
	 * @param buffer buffer to write the size, NULL to allocate it (with std::malloc)
	 * @param size size of the buffer
	 * @param nullTerminate add a 0 byte at the end of the file
	 * @return if the file was read
	 */
	bool ReadFileNotAlign(const std::filesystem::path& path, void*& buffer, size_t& size, bool nullTerminate = false);
	/*
	 * Read a file inside a buffer and align the result at 128 bits
	 * @param path file path
	 * @param buffer buffer to write the size, NULL to allocate it (with std::malloc)
	 * @param bufferAligned buffer location aligned
	 * @param size size of the buffer
	 * @param sizeAligned size of the aligned buffer
	 * @return if the file was read
	 */
	bool ReadFileAlign(const std::filesystem::path& path, void*& buffer, void*& bufferAligned, size_t& size, size_t& sizeAligned);
	/*
	 * Read a file inside a buffer and align the result at 128 bits
	 * @param path file path
	 * @param buffer buffer to write the size
	 * @param size size of the buffer
	 * @param sizeAligned size of the aligned buffer
	 * @return if the file was read
	 */
	bool ReadFileAlign(const std::filesystem::path& path, std::string& buffer, void*& bufferAligned, size_t& sizeAligned);
	/*
	 * Read a file inside a buffer and align the result at 128 bits
	 * @param path file path
	 * @param buffer buffer to write the size
	 * @param size size of the buffer
	 * @param sizeAligned size of the aligned buffer
	 * @return if the file was read
	 */
	bool ReadFileAlign(const std::filesystem::path& path, std::vector<byte>& buffer, void*& bufferAligned, size_t& sizeAligned);
	/*
	 * write a buffer into a file
	 * @param path file path
	 * @param ptr buffer location
	 * @param size buffer size
	 * @return if the file was written
	 */
	bool WriteFile(const std::filesystem::path& path, const void* ptr, size_t size);
	/*
	 * write a buffer into a file
	 * @param path file path
	 * @param str buffer
	 * @return if the file was written
	 */
	inline bool WriteFile(const std::filesystem::path& path, const std::string& str) {
		return WriteFile(path, str.data(), str.size());
	}
	/*
	 * write a buffer into a file
	 * @param path file path
	 * @param str buffer
	 * @return if the file was written
	 */
	inline bool WriteFile(const std::filesystem::path& path, const std::vector<byte>& data) {
		return WriteFile(path, data.data(), data.size());
	}
	/*
	 * Align a pointer
	 * @param Type type to align
	 * @param ptr pointer to align
	 * @return aligned ptr value
	 */
	template<typename Type>
	inline byte* Aligned(byte* ptr) {
		return reinterpret_cast<byte*>((reinterpret_cast<uintptr_t>(ptr) + (sizeof(Type) - 1)) & ~(sizeof(Type) - 1));
	}
	/*
	 * Align a pointer
	 * @param Type type to align
	 * @param TypeIn pointer type
	 * @param ptr pointer to align
	 * @return aligned ptr value
	 */
	template<typename Type, typename TypeIn = uintptr_t>
	inline TypeIn Aligned(TypeIn ptr) {
		return (ptr + (sizeof(Type) - 1)) & ~(sizeof(Type) - 1);
	}

	/*
	 * Align a vector buffer with its size
	 * @param Type type to align
	 * @param data vector buffer to align
	 */
	template<typename Type>
	inline void Aligned(std::vector<byte>& data) {
		size_t pre = data.size();
		size_t post = Aligned<Type, size_t>(pre);

		// add padding bytes
		for (size_t i = pre; i < post; i++) {
			data.push_back(0);
		}
	}
	/*
	 * Write a value into a vector buffer
	 * @param Type value type
	 * @param data buffer
	 * @param ptr value to write
	 * @param size value to write
	 * @return data location before write
	 */
	inline size_t WriteValue(std::vector<byte>& data, void* ptr, size_t size) {
		size_t begin = data.size();
		const byte* valLoc = (const byte*)ptr;
		data.insert(data.end(), valLoc, valLoc + size);
		return begin;
	}
	/*
	 * Write a value into a vector buffer
	 * @param Type value type
	 * @param data buffer
	 * @param val value to write
	 * @return data location before write
	 */
	template<typename Type>
	inline size_t WriteValue(std::vector<byte>& data, Type val) {
		size_t begin = data.size();
		const byte* valLoc = reinterpret_cast<const byte*>(&val);
		data.insert(data.end(), valLoc, valLoc + sizeof(val));
		return begin;
	}
	/*
	 * Write a string into a vector buffer
	 * @param data buffer
	 * @param val value to write
	 * @return data location before write
	 */
	inline size_t WriteString(std::vector<byte>& data, const char* val) {
		size_t begin = data.size();
		const byte* valLoc = reinterpret_cast<const byte*>(val);
		data.insert(data.end(), valLoc, valLoc + strlen(val) + 1);
		return begin;
	}
	/*
	 * Write a string into a vector buffer
	 * @param data buffer
	 * @param val value to write
	 * @return data location before write
	 */
	template<typename S>
	inline size_t WriteSizedString(std::vector<byte>& data, const char* val) {
		size_t begin = data.size();
		const byte* valLoc = reinterpret_cast<const byte*>(val);
		size_t len = strlen(val);
		constexpr size_t maxSize = GetMaxSize<S>();
		if (len >= maxSize) {
			throw std::runtime_error(va("can't write more than %llu elements", maxSize));
		}
		WriteValue<S>(data, (S)len);
		data.insert(data.end(), valLoc, valLoc + len);
		return begin;
	}
	/*
	 * Write a value into a vector buffer
	 * @param Type value type
	 * @param data buffer
	 * @param val value to write
	 * @return data location before write
	 */
	template<typename SizeType, typename Type>
	inline size_t WritePaddedValue(std::vector<byte>& data, Type val) {
		size_t begin = data.size();
		const byte* valLoc = reinterpret_cast<const byte*>(&val);
		data.insert(data.end(), valLoc, valLoc + sizeof(val));
		static_assert(sizeof(SizeType) >= sizeof(Type), "Trying to write bigger elements possible");
		constexpr auto delta = sizeof(SizeType) - sizeof(Type);
		if (delta) {
			Type t{};
			// fill with 0
			data.insert(data.end(), reinterpret_cast<byte*>(&t), reinterpret_cast<byte*>(&t) + delta);
		}
		return begin;
	}
	/*
	 * Allocate a pointer inside a vector
	 * @param data buffer
	 * @param len pointer size
	 * @return data location before write
	 */
	size_t Allocate(std::vector<byte>& data, size_t len);

	/*
	 * Allocate a pointer inside a vector
	 * @param data buffer
	 * @return pointer
	 */
	template<typename T>
	T& Allocate(std::vector<byte>& data) {
		return *reinterpret_cast<T*>(&data[Allocate(data, sizeof(T))]);
	}
	/*
	 * Write a padding into a stream
	 * @param out stream
	 * @param padding padding size
	 * @return stream
	 */
	std::ostream& Padding(std::ostream& out, int padding);
	/*
	 * Cat 2 uint32_t into a uint64_t
	 * @param name_space high value
	 * @param local low value
	 * @return cat value
	 */
	uint64_t CatLocated(uint32_t name_space, uint32_t local);
	/*
	 * Uncat 2 uint32_t into a uint64_t
	 * @param located cat value
	 * @return high value and low value
	 */
	std::pair<uint32_t, uint32_t> UnCatLocated(uint64_t located);
	/*
	 * Cat 2 uint16_t into a uint32_t
	 * @param a high value
	 * @param b low value
	 * @return cat value
	 */
	uint32_t CatLocated32(uint16_t a, uint16_t b);
	/*
	 * Uncat 2 uint16_t into a uint32_t
	 * @param located cat value
	 * @return high value and low value
	 */
	std::pair<uint16_t, uint16_t> UnCatLocated32(uint32_t located);
	/*
	 * Cat 2 byte into a uint16_t
	 * @param a high value
	 * @param b low value
	 * @return cat value
	 */
	uint16_t CatLocated16(byte a, byte b);
	/*
	 * Uncat 2 byte into a uint16_t
	 * @param located cat value
	 * @return high value and low value
	 */
	std::pair<byte, byte> UnCatLocated16(uint16_t located);

	/*
	 * Get all the files in a directory
	 * @param parent parent dir
	 * @param files output files
	 */
	void GetFileRecurse(const std::filesystem::path& parent, std::vector<std::filesystem::path>& files);

	/*
	 * Get all the files in a directory matching a predicate
	 * @param parent parent dir
	 * @param files output files
	 * @param predicate predicate
	 */
	void GetFileRecurse(const std::filesystem::path& parent, std::vector<std::filesystem::path>& files, std::function<bool(const std::filesystem::path&)> predicate, bool removeParent = false);

	std::ostream& PrintFormattedString(std::ostream& out, const char* str);
	/*
	 * Get a char to a byte value, throw invalid_argument for bad character
	 * @param c char value (a-fA-F0-9)
	 * @return byte value 0x0-0xf
	 */
	inline byte ctob(char c) {
		if (c >= 'A' && c <= 'F') {
			return (byte)(c - 'A' + 0xA);
		}
		if (c >= 'a' && c <= 'f') {
			return (byte)(c - 'a' + 0xA);
		}
		if (c >= '0' && c <= '9') {
			return (byte)(c - '0' + 0);
		}
		throw std::invalid_argument(std::format("Invalid character {:x}", c));
	}

	/*
	 * Convert wstring to string
	 * @param wstr wstring
	 * @return string
	 */
	std::string WStrToStr(const std::wstring& wstr);

	/*
	 * Convert string to wstring
	 * @param str string
	 * @return wstring
	 */
	std::wstring StrToWStr(const std::string& str);

	/*
	 * Upper case a string and return it
	 * @param buffer string
	 * @return string
	 */
	char* UpperCase(char* buffer);

	/*
	 * Lower case a string and return it
	 * @param buffer string
	 * @return string
	 */
	char* LowerCase(char* buffer);

	/*
	 * Replace all the spaces with a character
	 * @param buffer string
	 * @param map mapping function
	 * @return string
	 */
	char* MapString(char* buffer, std::function<char(char)> map);

	/*
	 * @return prog exe directory
	 */
	std::filesystem::path GetProgDir();

	/*
	 * Parse a number considering format (0xYYYY = 16, 0bYYYY = 2, 0YYYY = 8)
	 * @param number number
	 * @return number
	 * @throws std::runtime_error if invalid
	 */
	int64_t ParseFormatInt(const char* number);

	/**
	 * Add suffix to number K,M,G,T,etc.
	 * @param number number
	 * @param si base 1000(true) or 1024(false)
	 * @return fancy version
	 */
	const char* FancyNumber(uint64_t number, bool si = true);

	/*
	 * Clone a string
	 * @param str String to clone
	 * @return cloned string
	 */
	inline char* CloneString(const char* str) {
		return va("%s", str);
	}

	/*
	 * @param ptr Pointer
	 * @param defaultVal default value
	 * @param Type type
	 * @return get a pointer or another value
	 */
	template<typename Type>
	inline Type* PtrOrElse(Type* ptr, Type* defaultVal = nullptr) {
		return ptr ? ptr : defaultVal;
	}

	class CloseEnd {
		std::function<void()> func;
	public:
		CloseEnd(std::function<void()> func) : func(func) {}
		template<typename T>
		CloseEnd(T& obj) : func([&obj]() { obj.close(); }) {};
		~CloseEnd() { func(); };
	};

	class OutFileCE {
		std::ofstream os;

	public:
		OutFileCE() : os() {}
		OutFileCE(const char* f) : os(f) {}
		OutFileCE(const std::filesystem::path& f) : os(f) {}
		OutFileCE(const std::string& f) : os(f) {}

		~OutFileCE() {
			os.close();
		}

		bool operator!() {
			return !os;
		}

		std::ofstream& operator*() {
			return os;
		}


		operator std::ofstream& () {
			return os;
		}

		std::ofstream* operator->() {
			return &os;
		}

		template<typename T>
		OutFileCE& operator<<(T&& t) {
			os << std::move(t);
			return *this;
		}
		template<typename T>
		OutFileCE& operator<<(const T& t) {
			os << t;
			return *this;
		}
	};


	template<typename Type>
	struct BasicFormatter {
		template<class ParseContext>
		constexpr ParseContext::iterator parse(ParseContext& ctx) {
			auto it = ctx.begin();
			if (it == ctx.end()) {
				return it;
			}
			if (*it != '}') {
				throw std::format_error("Invalid format args.");
			}

			return it;
		}

		template<class FmtContext>
		FmtContext::iterator format(const Type& p, FmtContext& ctx) const {
			std::ostringstream out;

			out << p;

			return std::ranges::copy(std::move(out).str(), ctx.out()).out;
		}
	};
}