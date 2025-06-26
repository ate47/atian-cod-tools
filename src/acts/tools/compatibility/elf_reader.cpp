#include <includes.hpp>
// remove ksdk
#define KSDK_H
#include <elf.h>


namespace {
	constexpr char ELF_MAGIC[4] = { 0x7F, 'E', 'L', 'F' };

	class ElfReader {
		void* elf{};
		size_t elfLen{};
		Elf64_Ehdr* header{};
	public:

		ElfReader() {}
		~ElfReader() {
			FreeElf();
		}
		
		void FreeElf() {
			if (!elf) {
				return;
			}

			VirtualFree(elf, elfLen, MEM_FREE);

			elf = nullptr;
			elfLen = 0;
		}

		void LoadElf(const std::filesystem::path& path) {
			elf = utils::ReadFilePtr(path, &elfLen, [](size_t len) { return VirtualAlloc(NULL, len, MEM_COMMIT, PAGE_READWRITE); });

			if (!elf) {
				throw std::runtime_error(std::format("Can't read {}", path.string()));
			}

			if (elfLen < EI_NIDENT || memcmp(ELF_MAGIC, elf, sizeof(ELF_MAGIC))) {
				throw std::runtime_error(std::format("Can't read {}: bad magic", path.string()));
			}

			byte* data{ (byte*)elf };
			
			if (data[EI_CLASS] != ELFCLASS64) throw std::runtime_error(std::format("Can't read {}: not a 64 bits elf", path.string()));
			if (data[EI_VERSION] != EV_CURRENT) throw std::runtime_error(std::format("Can't read {}: bad version", path.string()));
			int endian{ std::endian::native == std::endian::big ? ELFDATA2MSB : ELFDATA2LSB };
			if (data[EI_DATA] != endian) throw std::runtime_error(std::format("Can't read {}: invalid endian", path.string()));

			header = (Elf64_Ehdr*)data;

			if (elfLen < sizeof(*header)) {
				throw std::runtime_error(std::format("Can't read {}: invalid header file", path.string()));
			}

			//Elf64_Xword//
		}

		byte* Base() {
			return (byte*)elf;
		}

		Elf64_Ehdr* Header() {
			return header;
		}
	};

	int elf_reader(int argc, const char* argv[]) {
		if (tool::NotEnoughParam(argc, 1)) return tool::BAD_USAGE;
		ElfReader reader{};

		reader.LoadElf(argv[2]);

		LOG_INFO("loaded {}", reader.Header()->e_ehsize);

		return tool::OK;
	}

	
	ADD_TOOL(elf_reader, "dev", " [elf]", "read elf data", elf_reader);
}