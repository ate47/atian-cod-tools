#include <includes.hpp>
#include "actscli.hpp"
#include "tools/gsc_opcodes.hpp"
#include "tools/gsc_opcodes_load.hpp"

namespace {

	// DO NOT CHANGE THIS HEADER OR MAGIC
	constexpr uint64_t ActsPackMagic = 0x4B415073746361;
	struct ActsPackHeader {
		byte magic[sizeof(ActsPackMagic)];
		byte version;
		byte minversion;
	};

	// current pack version
	constexpr byte ActsPackCurrentVersion = 0x10;
	// change this version when the changes are too importants
	constexpr byte ActsPackCurrentMinVersion = 0x10;

	enum ActsPackMinVersion : byte {
		APMV_Hashes = 0x10,
		APMV_VMs = 0x10
	};

	struct ActsPackVMPlatform {
		tool::gsc::opcode::Platform platform;
		uint32_t opOffset;
		uint32_t opCount;
	};

	struct ActsPackVM {
		uint32_t nameOffset;
		uint64_t vmflags;
		byte vm;
		byte platformsCount;
		uint32_t platformsOffset;
	};

	struct ActsPackHash {
		uint64_t value;
		uint32_t str;
	};
	struct ActsPackOpCode {
		tool::gsc::opcode::OPCode opcode;
		uint16_t value;
	};

	struct ActsPack {
		ActsPackHeader header;
		uint32_t vmCount;
		uint32_t vmOffset;
		uint32_t hashesCount;
		uint32_t hashesOffset;
	};

	class StringContainer {
		std::vector<char*> strings{};
	public:
		~StringContainer() {
			for (char* s : strings) {
				delete s;
			}
		}

		const char* AddString(const char* str) {
			auto len = strlen(str) + 1;
			char* ns = new char[len];

			memcpy(ns, str, len);

			strings.push_back(ns);

			return ns;
		}
	};
	StringContainer strcontainer{};

	bool LoadPackFileData(void* buffer, size_t bufferSize) {
		if (bufferSize < sizeof(ActsPackHeader)) {
			LOG_ERROR("acts pack file header is too small");
			return false;
		}

		auto* header = reinterpret_cast<ActsPackHeader*>(buffer);

		if (*reinterpret_cast<decltype(ActsPackMagic)*>(header->magic) != ActsPackMagic) {
			LOG_ERROR("Invalid acts pack file map file magic");
			return false;
		}

		if (header->minversion > ActsPackCurrentVersion) {
			LOG_ERROR("acts pack file min version too recent, current 0x{:x} < 0x{:x}", ActsPackCurrentVersion, (int)header->minversion);
			return false;
		}

		// version ok, magic ok, check data
		if (bufferSize < sizeof(ActsPack)) {
			LOG_ERROR("acts pack file too small");
			return false;
		}

		auto* pack = reinterpret_cast<ActsPack*>(buffer);

		if (pack->header.version >= ActsPackMinVersion::APMV_Hashes) {
			// load precomputed hashes
			if (pack->hashesCount && (pack->hashesOffset + sizeof(ActsPackHash) * pack->hashesCount) > bufferSize) {
				LOG_ERROR("Invalid hash list in acts pack file");
				return false;
			}

			auto* hashes = reinterpret_cast<ActsPackHash*>(pack->header.magic + pack->hashesOffset);

			for (size_t i = 0; i < pack->hashesCount; i++) {
				if (hashes[i].str > bufferSize) {
					LOG_ERROR("Invalid string location in acts pack file");
					return false;
				}
				hashutils::AddPrecomputed(hashes[i].value, (const char*)(pack->header.magic + hashes[i].str));
			}
		}

		if (pack->header.version >= ActsPackMinVersion::APMV_VMs) {
			// load VM opcodes
			if (pack->vmCount && (pack->vmOffset + sizeof(ActsPackVM) * pack->vmCount) > bufferSize) {
				LOG_ERROR("Invalid vm list in acts pack file");
				return false;
			}

			auto* vms = reinterpret_cast<ActsPackVM*>(pack->header.magic + pack->vmOffset);

			for (size_t i = 0; i < pack->vmCount; i++) {
				auto& vm = vms[i];
				if (vm.nameOffset >= bufferSize) {
					LOG_ERROR("Invalid vm name location in acts pack file");
					return false;
				}


				if (vm.platformsCount && vm.platformsOffset + sizeof(ActsPackVMPlatform) * vm.platformsCount > bufferSize) {
					LOG_ERROR("Invalid vm platforms location in acts pack file");
					return false;
				}

				if (vm.platformsCount) {
					// load default opcodes
					tool::gsc::opcode::RegisterOpCodes();
				}

				auto* name = (const char*)(pack->header.magic + vm.nameOffset);

				// register the vm itself
				const char* str = strcontainer.AddString(name);
				tool::gsc::opcode::RegisterVM(vm.vm, str, str, vm.vmflags);

				auto* platforms = reinterpret_cast<ActsPackVMPlatform*>(pack->header.magic + vm.platformsOffset);

				// add the platforms
				for (size_t i = 0; i < vm.platformsCount; i++) {
					auto& plt = platforms[i];

					if (plt.opCount && plt.opOffset + sizeof(ActsPackOpCode) * plt.opCount > bufferSize) {
						LOG_ERROR("Invalid vm platform opcodes location in acts pack file");
						return false;
					}


					LOG_DEBUG("{} - plt {} {} -> {}", name, (int)vm.vm, tool::gsc::opcode::PlatformName(plt.platform), plt.opCount);
					tool::gsc::opcode::RegisterVMPlatform(vm.vm, plt.platform);

					auto* ops = reinterpret_cast<ActsPackOpCode*>(pack->header.magic + plt.opOffset);

					// load the opcodes for this vm/platform
					for (size_t i = 0; i < plt.opCount; i++) {
						auto& op = ops[i];
						
						tool::gsc::opcode::RegisterOpCode(vm.vm, plt.platform, op.opcode, op.value);
					}
				}
			}
		}

		return true;
	}
	int packfile(Process& proc, int argc, const char* argv[]) {
		// params
		const char* output = "acts.acpf";

		std::unordered_set<uint16_t> vms{};

		if (argc > 2) {
			output = argv[2];

			for (size_t params = 3; params < argc; params++) {
				std::string_view arg{ argv[params] };

				size_t idx = arg.find(':');

				tool::gsc::opcode::Platform plt{};

				if (idx != std::string::npos) {
					std::string pltStr{ arg.substr(idx + 1, arg.length() - idx - 1) };
					plt = tool::gsc::opcode::PlatformOf(pltStr.c_str());
					if (!plt) {
						LOG_ERROR("Invalid platform: {}", pltStr);
						return tool::BASIC_ERROR;
					}
				}
				else {
					idx = arg.length();
				}

				std::string vmStr{ arg.substr(0, idx) };

				tool::gsc::opcode::VM vm = tool::gsc::opcode::VMOf(vmStr.c_str());

				if (!vm) {
					LOG_ERROR("Invalid vm: {}", vmStr);
					return tool::BASIC_ERROR;
				}

				vms.insert(utils::CatLocated16(plt, vm));
			}
		}

		auto doPackVm = [&vms](byte vm, tool::gsc::opcode::Platform plt) {
			return vms.empty() || vms.contains(utils::CatLocated16(plt, vm)) || vms.contains(utils::CatLocated16(0, vm));
		};

		// read maps
		hashutils::ReadDefaultFile();
		tool::gsc::opcode::RegisterOpCodesMap();

		std::vector<byte> packFileData{};

		// allocate header

		// add a string to this buffer
		auto AppendString = [&packFileData](const char* str) -> uint32_t {
			size_t offset = packFileData.size();
			while (*str) {
				packFileData.push_back(*(str++));
			}
			packFileData.push_back(0);

			return (uint32_t)offset;
		};

		// add a block to this buffer
		auto CreateBlock = [&packFileData](size_t size) -> uint32_t {
			size_t offset = packFileData.size();
			if (offset + size < packFileData.capacity()) {
				packFileData.reserve(max(offset + size * 2, offset * 2));
			}

			for (size_t i = 0; i < size; i++) {
				packFileData.push_back(0);
			}

			return (uint32_t)offset;
		};

		{
			CreateBlock(sizeof(ActsPack));

			auto& header = *reinterpret_cast<ActsPack*>(packFileData.data());

			*reinterpret_cast<uint64_t*>(header.header.magic) = ActsPackMagic;
			header.header.minversion = ActsPackCurrentMinVersion;
			header.header.version = ActsPackCurrentVersion;
		}

		// dump hashes
		if (!actscli::options().noDefaultHash) {
			LOG_INFO("Dumping hashmap...");
			
			const auto& hashmap = hashutils::GetMap();

			auto hashmapoffset = CreateBlock(hashmap.size() * sizeof(ActsPackHash));

			reinterpret_cast<ActsPack*>(packFileData.data())->hashesOffset = hashmapoffset;
			reinterpret_cast<ActsPack*>(packFileData.data())->hashesCount = (uint32_t)hashmap.size();

			size_t hashIdx{};
			for (const auto& [val, str] : hashmap) {
				auto stroff = AppendString(str.c_str());

				auto& hv = reinterpret_cast<ActsPackHash*>(&packFileData[hashmapoffset])[hashIdx++];

				hv.str = stroff;
				hv.value = val;
			}
			LOG_INFO("Done.");
		}

		// dump opcodes
		{
			LOG_INFO("Dumping vms...");


			auto& vmmap = tool::gsc::opcode::GetVMMaps();

			auto vmoffset = CreateBlock(vmmap.size() * sizeof(ActsPackVM));

			size_t vmIdx{};
			for (const auto& [vm, vminfo] : vmmap) {
				auto stroff = AppendString(vminfo.name);

				uint32_t platformCount{};
				// compute platform count
				for (size_t i = 1; i < tool::gsc::opcode::Platform::PLATFORM_COUNT; i++) {
					if (vminfo.HasPlatform((tool::gsc::opcode::Platform)i)) {
						platformCount++;
					}
				}

				auto plts = CreateBlock(platformCount * sizeof(ActsPackVMPlatform));

				size_t pltIdx{};
				for (size_t i = 1; i < tool::gsc::opcode::Platform::PLATFORM_COUNT; i++) {
					auto p = (tool::gsc::opcode::Platform)i;
					if (vminfo.HasPlatform(p) && doPackVm(vm, p)) {
						LOG_INFO("Dump vm {} / {}", vminfo.name, tool::gsc::opcode::PlatformName(p));
						uint32_t opIdx{};
						uint32_t opcodeoff{};
						auto pit = vminfo.opcodemappltlookup.find(p);

						if (pit != vminfo.opcodemappltlookup.end()) {
							size_t opcodelen{};

							for (const auto& [_, opv] : pit->second) {
								opcodelen += opv.size();
							}

							opcodeoff = CreateBlock(opcodelen * sizeof(ActsPackOpCode));
							for (const auto& [ope, opvl] : pit->second) {
								for (auto opv : opvl) {
									auto& opl = reinterpret_cast<ActsPackOpCode*>(&packFileData[opcodeoff])[opIdx++];

									opl.opcode = ope;
									opl.value = opv;
								}
							}
						}

						auto& pltv = reinterpret_cast<ActsPackVMPlatform*>(&packFileData[plts])[pltIdx++];
						pltv.platform = p;
						pltv.opCount = opIdx;
						pltv.opOffset = opcodeoff;
					}
				}


				auto& vmv = reinterpret_cast<ActsPackVM*>(&packFileData[vmoffset])[vmIdx++];

				vmv.nameOffset = stroff;
				vmv.vm = vminfo.vm;
				vmv.vmflags = vminfo.flags;
				vmv.platformsCount = (byte)pltIdx;
				vmv.platformsOffset = plts;
			}

			reinterpret_cast<ActsPack*>(packFileData.data())->vmCount = (uint32_t)vmmap.size();
			reinterpret_cast<ActsPack*>(packFileData.data())->vmOffset = vmoffset;

			LOG_INFO("Done.");
		}

		LOG_INFO("Writing into {}...", output);

		if (!utils::WriteFile(output, packFileData.data(), packFileData.size())) {
			LOG_ERROR("Can't write {}", output);
			return tool::BASIC_ERROR;
		}

		LOG_INFO("Done");

		return tool::OK;
	}
}

namespace actscli {

	ActsOptions& options() {
		static ActsOptions opt{};
		return opt;
	}

	actslib::profiler::Profiler& GetProfiler() {
		static actslib::profiler::Profiler profiler{ "acts" };
		return profiler;
	}

	bool LoadPackFile(const std::filesystem::path& mapFile) {
		actslib::profiler::ProfiledSection ps{ actscli::GetProfiler(), "LoadActsPackFile" };
		void* buffer{};
		size_t bufferSize{};

		// add null end for the strings
		if (!utils::ReadFileNotAlign(mapFile, buffer, bufferSize, true)) {
			LOG_ERROR("Can't read ACTS pack file: {}", mapFile.string());
			return false;
		}

		LOG_DEBUG("Loading pack file {}", mapFile.string());

		bool res = LoadPackFileData(buffer, bufferSize);

		LOG_DEBUG("Loaded.");

		std::free(buffer);

		return res;
	}
}
ADD_TOOL("packfile", "acts", " [file=acts.acpf]", "Create ACTS pack file", nullptr, packfile);
