#include <includes.hpp>
#include <cli/cli_options.hpp>
#include <core/bytebuffer.hpp>
#include <deps/mio.hpp>


namespace {

#define SWAP_ENDIAN_64(l) ((((l) >> 56) & 0x00000000000000FFLL) \
                         | (((l) >> 40) & 0x000000000000FF00LL) \
                         | (((l) >> 24) & 0x0000000000FF0000LL) \
                         | (((l) >> 8) & 0x00000000FF000000LL) \
                         | (((l) << 8) & 0x000000FF00000000LL) \
                         | (((l) << 24) & 0x0000FF0000000000LL) \
                         | (((l) << 40) & 0x00FF000000000000LL) \
                         | (((l) << 56) & 0xFF00000000000000LL))

#ifdef assert
#undef assert
#endif
#define assert(x) if (!(x)) throw std::runtime_error(std::format("Assert error: !(" #x ")"))
	constexpr uint64_t ReadLittleEndian(uint64_t le) {
		if constexpr (std::endian::native == std::endian::little) {
			return le;
		}
		else {
			return SWAP_ENDIAN_64(le);
		}
	}

	uint64_t NumWords(int bitsField, uint64_t total) {
		return (bitsField * total + 63) / 64;
	}

	uint64_t LastWordNumBits(int bitsField, uint64_t total) {
		uint64_t totalBits = bitsField * total;
		if (totalBits == 0) {
			return 0;
		}
		return (totalBits - 1) % 64 + 1;
	}

	class SequenceLog {
		int numBits{};
		byte* data{};
		uint64_t numwords{};
		uint64_t lastWord{};
		uint64_t numentries{};
	public:
		SequenceLog() {}
		void Load(core::bytebuffer::ByteBuffer& buff) {
			if (buff.Read<byte>() != 1) {
				throw std::runtime_error("Not a sequence log: Invalid type");
			}

			numBits = (int)buff.Read<byte>();
			numentries = buff.ReadVByteInv();

			buff.Skip(1); // crc check

			if (numBits > 64) {
				throw std::runtime_error("Numbits can't be above 64");
			}

			numwords = NumWords(numBits, numentries);
			size_t add{ LastWordNumBits(numBits, numentries) };
			if (numwords > 0) {
				size_t addBytes{ (add - 1) / 8 + 1 };
				size_t toRead{ (numwords - 1) * 8 };
				LOG_TRACE("Loaded SequenceLog with {} bits (max 0x{:x}), {} entries ({} words) {}/{}", numBits, (1ull << numBits), numentries, numwords, add, toRead);
				if (addBytes) {
					data = buff.ReadPtr<byte>(toRead);
					byte* lastBytes{ buff.ReadPtr<byte>(addBytes) };
					lastWord = 0;
					std::memcpy(&lastWord, lastBytes, addBytes);
				}
				else {
					data = buff.ReadPtr<byte>(toRead - 1);
					lastWord = *buff.ReadPtr<uint64_t>(1);
				}
			}
			else {
				data = nullptr;
				lastWord = 0;
			}
			buff.Skip(4); // crc check
		}

		constexpr uint64_t GetNumEntries() const {
			return numentries;
		}

		inline uint64_t GetWord(size_t idx) const {
			assert(idx < numwords);
			if (idx + 1 == numwords) {
				return ReadLittleEndian(lastWord);
			}
			return ReadLittleEndian(((uint64_t*)data)[idx]);
		}

		uint64_t operator[](size_t idx) const {
			if (!numBits) {
				return 0;
			}

			size_t bitPos{ idx * numBits };
			size_t i{ bitPos >> 6 };
			size_t j{ bitPos & 63 };
			if (j + numBits <= 64) {
				return (GetWord(i) << (64ull - j - numBits)) >> (64ull - numBits);
			}
			else {
				return (GetWord(i) >> j) | ((GetWord(i + 1) << ((64ull << 1) - j - numBits)) >> (64ull - numBits));
			}
		}
	};
	class Bitmap64 {
		byte* data{};
		size_t words{};
		size_t numbits{};
	public:
		Bitmap64() {}

		void Load(core::bytebuffer::ByteBuffer& buff) {
			if (buff.Read<byte>() != 1) {
				throw std::runtime_error("Not a bitmap plain");
			}

			numbits = buff.ReadVByteInv();
			buff.Skip<uint8_t>(); // skip crc

			if (numbits > 0) {
				words = ((numbits - 1) >> 3) + 1;

				// should be converted to LE if required
				LOG_TRACE("Loaded Bitmap64 with {} bits ({} words)", numbits, words);
				data = buff.ReadPtr<byte>(words);
			}
			else {
				words = 0;
				data = nullptr;
			}

			buff.Skip<uint32_t>(); // skip crc
		}

		constexpr uint64_t GetNumEntries() const {
			return numbits;
		}

		bool operator[](size_t idx) const {
			size_t word{ idx >> 3 };
			size_t bitIndex{ idx & 7 };
			assert(word < words);
			return data[word] & (1L << bitIndex);
		}
	};

	class HDTBitmapTriplesIndex;

	struct TripleID {
		uint64_t s{}, p{}, o{};
	};

	std::ostream& operator<<(std::ostream& os, const TripleID& id) {
		return os << "(" << id.s << "," << id.p << "," << id.o << ")";
	}
}
template<>
struct std::formatter<TripleID, char> : utils::BasicFormatter<TripleID> {};
namespace {

	class HDTBitmapTriplesIndexIterator {
		HDTBitmapTriplesIndex& src;
		TripleID curr{};
		uint64_t y{};
		uint64_t z{};
		uint64_t end{};
	public:
		HDTBitmapTriplesIndexIterator(HDTBitmapTriplesIndex& src);

		const TripleID& operator*() const {
			return curr;
		}

		const TripleID* operator->() const {
			return &curr;
		}

		constexpr operator bool() const {
			return z < end;
		}

		HDTBitmapTriplesIndexIterator& operator++();
	};

	enum TripleComponent : int {
		TC_SUBJECT = 0,
		TC_PREDICATE = 1,
		TC_OBJECT = 2,
		TC_GRAPH = 3,
	};

	enum TripleOrder : uint64_t {
		TO_UNKNOWN = 0,

		TO_SHIFT_SUB = TC_SUBJECT * 2,
		TO_SHIFT_PRE = TC_PREDICATE * 2,
		TO_SHIFT_OBJ = TC_OBJECT * 2,
		TO_SHIFT_GRA = TC_GRAPH * 2,
		TO_MASK = 3,

		TO_SPO = (TC_SUBJECT << TO_SHIFT_SUB) | (TC_PREDICATE << TO_SHIFT_PRE) | (TC_OBJECT << TO_SHIFT_OBJ) | (TC_GRAPH << TO_SHIFT_GRA),
		TO_SOP = (TC_SUBJECT << TO_SHIFT_SUB) | (TC_OBJECT << TO_SHIFT_PRE) | (TC_PREDICATE << TO_SHIFT_OBJ) | (TC_GRAPH << TO_SHIFT_GRA),
		TO_PSO = (TC_PREDICATE << TO_SHIFT_SUB) | (TC_SUBJECT << TO_SHIFT_PRE) | (TC_OBJECT << TO_SHIFT_OBJ) | (TC_GRAPH << TO_SHIFT_GRA),
		TO_POS = (TC_PREDICATE << TO_SHIFT_SUB) | (TC_OBJECT << TO_SHIFT_PRE) | (TC_SUBJECT << TO_SHIFT_OBJ) | (TC_GRAPH << TO_SHIFT_GRA),
		TO_OPS = (TC_OBJECT << TO_SHIFT_SUB) | (TC_PREDICATE << TO_SHIFT_PRE) | (TC_SUBJECT << TO_SHIFT_OBJ) | (TC_GRAPH << TO_SHIFT_GRA),
		TO_OSP = (TC_OBJECT << TO_SHIFT_SUB) | (TC_SUBJECT << TO_SHIFT_PRE) | (TC_PREDICATE << TO_SHIFT_OBJ) | (TC_GRAPH << TO_SHIFT_GRA),
	};

	constexpr TripleComponent GetTripleMapping(TripleOrder order, TripleComponent comp) {
		return (TripleComponent)((order >> (comp << 1)) & TO_MASK);
	}

	TripleOrder GetTripleOrderByName(const char* name) {
		if (!_strcmpi(name, "SPO")) return TO_SPO;
		if (!_strcmpi(name, "SOP")) return TO_SOP;
		if (!_strcmpi(name, "PSO")) return TO_PSO;
		if (!_strcmpi(name, "POS")) return TO_POS;
		if (!_strcmpi(name, "OPS")) return TO_OPS;
		if (!_strcmpi(name, "OSP")) return TO_OSP;
		return TO_UNKNOWN;
	}

	class HDTBitmapTriplesIndex {
		SequenceLog seqY;
		Bitmap64 bitY;
		SequenceLog seqZ;
		Bitmap64 bitZ;
		TripleOrder order;
	public:
		HDTBitmapTriplesIndex(core::bytebuffer::ByteBuffer& buff) {
			constexpr const char magic[] = "$HDTIDX";
			char magicTmp[sizeof(magic)]{};
			buff.Read(&magicTmp[0], sizeof(magic) - 1);

			if (std::memcmp(magicTmp, magic, sizeof(magic) - 1)) {
				throw std::runtime_error(std::format("Invalid MAGIC {}", magicTmp));
			}

			char version{ buff.Read<char>() };

			LOG_DEBUG("Loading version {}", version);

			switch (version) {
			case '0':
				// nothing
				break;
			case '1':
				buff.Skip(8); // signature
				break;
			default:
				throw std::runtime_error(std::format("Invalid version {}", version));
			}

			std::string idxOrder{ buff.ReadVByteInvSizedString() };
			order = GetTripleOrderByName(idxOrder.data());
			if (!order) {
				throw std::runtime_error(std::format("Invalid index order: {}", idxOrder));
			}
			LOG_DEBUG("order: {} {}", idxOrder, (uint64_t)order);

			seqY.Load(buff);
			bitY.Load(buff);

			if (seqY.GetNumEntries() != bitY.GetNumEntries()) {
				throw std::runtime_error(std::format("Invalid num entries for seqy/bity: {} != {}", seqY.GetNumEntries(), bitY.GetNumEntries()));
			}

			seqZ.Load(buff);
			bitZ.Load(buff);

			if (seqY.GetNumEntries() != bitY.GetNumEntries()) {
				throw std::runtime_error(std::format("Invalid num entries for seqy/bity: {} != {}", seqY.GetNumEntries(), bitY.GetNumEntries()));
			}
		}
		const SequenceLog& GetSeqY() const { return seqY; }
		const Bitmap64& GetBitY() const { return bitY; }
		const SequenceLog& GetSeqZ() const { return seqZ; }
		const Bitmap64& GetBitZ() const { return bitZ; }

		HDTBitmapTriplesIndexIterator CreateIterator() {
			return HDTBitmapTriplesIndexIterator(*this);
		}
	};

	HDTBitmapTriplesIndexIterator::HDTBitmapTriplesIndexIterator(HDTBitmapTriplesIndex& src) : src(src) {
		y = 1;
		z = 1;
		end = src.GetSeqZ().GetNumEntries();
		if (z != end) {
			curr.s = 1;
			curr.p = src.GetSeqY()[0];
			curr.o = src.GetSeqZ()[0];
		}
	}
	HDTBitmapTriplesIndexIterator& HDTBitmapTriplesIndexIterator::operator++() {
		if (!*this) {
			throw std::runtime_error("End of the iterator");
		}

		if (z + 1 == end) {
			z++;
			return *this;
		}

		if (src.GetBitZ()[z]) {
			// new predicate
			if (src.GetBitY()[y]) {
				// new subject
				curr.s++;
			}
			curr.p = src.GetSeqY()[++y];
		}

		curr.o = src.GetSeqZ()[++z];

		return *this;
	}

	int disksort(int argc, const char* argv[]) {
		cli::options::CliOptions opts{};

		struct {
			bool showHelp{};
		} opt;

		opts
			.addOption(&opt, "show help", "--help", "", "-h")
			.ComputeOptions(2, argc, argv);

		if (opts.ParamsCount() < 2 || opt.showHelp) {
			return tool::BAD_USAGE;
		}

		std::filesystem::path file{ opts[0] };
		const char* order{ opts[1] };



		mio::mmap_source mmap{ file.string() };

		LOG_INFO("Loading {} into {} (size: 0x{:x})", file.string(), order, mmap.size());

		core::bytebuffer::ByteBuffer buff{ (byte*)mmap.begin(), mmap.size() };
		
		HDTBitmapTriplesIndex idx{ buff };

		HDTBitmapTriplesIndexIterator it{ idx.CreateIterator() };

		for (size_t i = 0; i < 100; i++) {
			if (!it) break;

			LOG_INFO("{}", *it)

			++it;
		}

		return tool::OK;
	}

	ADD_TOOL(disksort, "lib", "[file] [order]", "Reorder HDT other IDX", disksort);
}