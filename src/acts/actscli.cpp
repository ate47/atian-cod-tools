#include <includes.hpp>
#include "actscli.hpp"
#include "tools/gsc.hpp"
#include "tools/gsc_opcodes.hpp"
#include "tools/gsc_opcodes_load.hpp"
#include <core/updater.hpp>

namespace {

	// DO NOT CHANGE THIS HEADER OR MAGIC
	constexpr uint64_t ActsPackMagic = 0x4B415073746361;
	struct ActsPackHeader {
		byte magic[sizeof(ActsPackMagic)];
		byte version;
		byte minversion;
	};

	// current pack version
	constexpr byte ActsPackCurrentVersion = 0x11;
	// change this version when the changes are too importants
	constexpr byte ActsPackCurrentMinVersion = 0x10;

	enum ActsPackMinVersion : byte {
		APMV_Hashes = 0x10,
		APMV_VMs = 0x10,
		APMV_FLAGS = 0x11,
		APMV_VM64s = 0x11
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

	struct ActsPackNewVM {
		uint32_t nameOffset;
		uint64_t vmflags;
		uint64_t vmMagic;
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
		uint64_t flags;
		uint32_t newVmCount;
		uint32_t newVmOffset;

		constexpr bool HasFeature(ActsPackMinVersion feat) {
			return header.version >= feat;
		}

		constexpr bool HasFlag(uint64_t flags) {
			return HasFeature(APMV_FLAGS) && (this->flags & flags) == flags;
		}
	};

	enum ActsPackFlags : uint64_t {
		APF_IGNORE_OLD_VM = 1,
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

		if (pack->HasFeature(ActsPackMinVersion::APMV_Hashes)) {
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

		if (pack->HasFeature(ActsPackMinVersion::APMV_VMs) && !pack->HasFlag(APF_IGNORE_OLD_VM)) {
			// load VM opcodes
			if (pack->vmCount && (pack->vmOffset + sizeof(ActsPackVM) * pack->vmCount) > bufferSize) {
				LOG_ERROR("Invalid vm list in acts pack file");
				return false;
			}

			ActsPackVM* vms = reinterpret_cast<ActsPackVM*>(pack->header.magic + pack->vmOffset);

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

				uint64_t nvm{ tool::gsc::opcode::OldVmOf(vm.vm) };

				if (!nvm) {
					LOG_WARNING("Can't load old vm 0x{:x} from acts pack", (int)vm.vm);
					continue;
				}

				tool::gsc::opcode::VmInfo* nfo{ tool::gsc::opcode::RegisterVM(nvm, str, str, str, vm.vmflags) };

				auto* platforms = reinterpret_cast<ActsPackVMPlatform*>(pack->header.magic + vm.platformsOffset);

				// add the platforms
				for (size_t i = 0; i < vm.platformsCount; i++) {
					auto& plt = platforms[i];

					if (plt.opCount && plt.opOffset + sizeof(ActsPackOpCode) * plt.opCount > bufferSize) {
						LOG_ERROR("Invalid vm platform opcodes location in acts pack file");
						return false;
					}


					LOG_DEBUG("{} - plt {:x} {} -> {}", name, nvm, tool::gsc::opcode::PlatformName(plt.platform), plt.opCount);
					nfo->AddPlatform(plt.platform);

					auto* ops = reinterpret_cast<ActsPackOpCode*>(pack->header.magic + plt.opOffset);

					// load the opcodes for this vm/platform
					for (size_t i = 0; i < plt.opCount; i++) {
						auto& op = ops[i];
						
						nfo->RegisterOpCode(plt.platform, op.opcode, op.value);
					}
				}
			}
		}

		if (pack->HasFeature(ActsPackMinVersion::APMV_VM64s)) {
			// load VM opcodes
			if (pack->vmCount && (pack->vmOffset + sizeof(ActsPackNewVM) * pack->newVmCount) > bufferSize) {
				LOG_ERROR("Invalid new vm list in acts pack file");
				return false;
			}

			ActsPackNewVM* vms = reinterpret_cast<ActsPackNewVM*>(pack->header.magic + pack->newVmOffset);

			for (size_t i = 0; i < pack->vmCount; i++) {
				ActsPackNewVM& vm = vms[i];
				if (vm.nameOffset >= bufferSize) {
					LOG_ERROR("Invalid new vm name location in acts pack file");
					return false;
				}


				if (vm.platformsCount && vm.platformsOffset + sizeof(ActsPackVMPlatform) * vm.platformsCount > bufferSize) {
					LOG_ERROR("Invalid new vm platforms location in acts pack file");
					return false;
				}

				if (vm.platformsCount) {
					// load default opcodes
					tool::gsc::opcode::RegisterOpCodes();
				}

				auto* name = (const char*)(pack->header.magic + vm.nameOffset);

				// register the vm itself
				const char* str = strcontainer.AddString(name);
				tool::gsc::opcode::VmInfo* nfo{ tool::gsc::opcode::RegisterVM(vm.vmMagic, str, str, str, vm.vmflags) };

				auto* platforms = reinterpret_cast<ActsPackVMPlatform*>(pack->header.magic + vm.platformsOffset);

				// add the platforms
				for (size_t i = 0; i < vm.platformsCount; i++) {
					auto& plt = platforms[i];

					if (plt.opCount && plt.opOffset + sizeof(ActsPackOpCode) * plt.opCount > bufferSize) {
						LOG_ERROR("Invalid new vm platform opcodes location in acts pack file 0x{:x}[0x{:x}] > 0x{:x}", plt.opOffset, plt.opCount, bufferSize);
						return false;
					}


					LOG_DEBUG("{} - plt {:x} {} -> {}", name, vm.vmMagic, tool::gsc::opcode::PlatformName(plt.platform), plt.opCount);
					nfo->AddPlatform(plt.platform);

					auto* ops = reinterpret_cast<ActsPackOpCode*>(pack->header.magic + plt.opOffset);

					// load the opcodes for this vm/platform
					for (size_t i = 0; i < plt.opCount; i++) {
						auto& op = ops[i];

						nfo->RegisterOpCode(plt.platform, op.opcode, op.value);
					}
				}
			}
		}

		return true;
	}
	int exitcli(Process& proc, int argc, const char* argv[]) {
		actscli::options().exitAfterEnd = true;
		return tool::OK;
	}

	int replcli(Process& proc, int argc, const char* argv[]) {
		actscli::options().type = actscli::ACTS_REPL;
		return tool::OK;
	}

	int echocli(Process& proc, int argc, const char* argv[]) {
		for (size_t i = 2; i < argc; i++) {
			LOG_INFO("{}", argv[i]);
		}
		return tool::OK;
	}

	int packfile(Process& proc, int argc, const char* argv[]) {
		// params
		const char* output = "acts.acpf";

		std::unordered_set<tool::gsc::NameLocated, tool::gsc::NameLocatedHash, tool::gsc::NameLocatedEquals> vms{};
		std::unordered_set<uint64_t> vmsAny{};

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

				tool::gsc::opcode::VMId vm = tool::gsc::opcode::VMOf(vmStr.c_str());

				if (!vm) {
					LOG_ERROR("Invalid vm: {}", vmStr);
					return tool::BASIC_ERROR;
				}

				vms.insert(tool::gsc::NameLocated{ plt, vm });
				vmsAny.insert((uint64_t)vm);
			}
		}

		auto doPackVm = [&vms](uint64_t vm, tool::gsc::opcode::Platform plt) {
			return vms.empty() || vms.contains(tool::gsc::NameLocated{ plt, vm }) || vms.contains(tool::gsc::NameLocated{ 0, vm });
		};
		auto doPackAnyVm = [&vmsAny](uint64_t vm) {
			return vmsAny.empty() || vmsAny.contains(vm);
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
				packFileData.reserve(std::max(offset + size * 2, offset * 2));
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
				auto stroff = AppendString(str);

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
			auto nvmoffset = CreateBlock(vmmap.size() * sizeof(ActsPackNewVM));

			size_t vmIdx{};
			size_t nvmIdx{};
			for (const auto& [vm, vminfo] : vmmap) {
				if (!doPackAnyVm(vm)) continue;
				byte vmOld{ tool::gsc::opcode::MapAsOldVM(vm) };

				if (!vmOld) {
					LOG_WARNING("Can't map to old vm {:x}, it'll be ignored", vm);
				}
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
						uint32_t opIdx{};
						uint32_t opcodeoff{};
						auto pit = vminfo.opcodemappltlookup.find(p);

						size_t opcodelen{};
						if (pit != vminfo.opcodemappltlookup.end()) {

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
						LOG_INFO("Dump vm {} / {} / {} (0x{:x}[0x{:x}])", vminfo.name, tool::gsc::opcode::PlatformName(p), opcodelen, opcodeoff, opIdx);
					}
				}

				if (vmOld) {
					auto& vmv = reinterpret_cast<ActsPackVM*>(&packFileData[vmoffset])[vmIdx++];

					vmv.nameOffset = stroff;
					vmv.vm = vmOld;
					vmv.vmflags = vminfo.flags;
					vmv.platformsCount = (byte)pltIdx;
					vmv.platformsOffset = plts;
				}
				auto& nvmv = reinterpret_cast<ActsPackNewVM*>(&packFileData[nvmoffset])[nvmIdx++];

				nvmv.nameOffset = stroff;
				nvmv.vmMagic = vm;
				nvmv.vmflags = vminfo.flags;
				nvmv.platformsCount = (byte)pltIdx;
				nvmv.platformsOffset = plts;
				LOG_DEBUG("Dump vm {} done -> {}/{} (0x{:x}[0x{:x}])", vminfo.name, vmIdx, nvmIdx, plts, pltIdx);
			}

			//LOG_TRACE("Vm offset: 0x{:x}[{}]", vmoffset, vmIdx);
			ActsPack* pack{ reinterpret_cast<ActsPack*>(packFileData.data()) };
			pack->vmCount = (uint32_t)vmIdx;
			pack->vmOffset = vmoffset;
			pack->newVmCount = (uint32_t)nvmIdx;
			pack->newVmOffset = nvmoffset;
			pack->flags = APF_IGNORE_OLD_VM; // compatibility

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

	int forceupdate(int argc, const char* argv[]) {
		core::updater::CheckUpdate(true);
		return tool::OK;
	}

	int checkupdate(int argc, const char* argv[]) {
		if (core::updater::CheckUpdate()) {
			// force the exit for the retards
			actscli::options().exitAfterEnd = true;
		}
		else {
			LOG_INFO("Already up to date");
		}
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
ADD_TOOL(packfile, "acts", " [file=acts.acpf]", "Create ACTS pack file", nullptr, packfile);
ADD_TOOL(exit, "acts", "", "Exit repl cli", nullptr, exitcli);
ADD_TOOL(echo, "acts", "", "echo", nullptr, echocli);
ADD_TOOL(repl, "acts", "", "Use repl cli", nullptr, replcli);
ADD_TOOL(forceupdate, "acts", "", "force update", forceupdate);
ADD_TOOL(checkupdate, "acts", "", "check update", checkupdate);
