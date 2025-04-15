#include <includes.hpp>
#include <core/memory_allocator.hpp>
#include <core/bytebuffer_file.hpp>
#include <tools/utils/compress_utils.hpp>
#include "acts_extension.hpp"

namespace acts::extension {

	struct AcefData {
		std::unordered_map<uint64_t, std::vector<AcefArray>> map{};
	};

	namespace {
		core::memory_allocator::MemoryAllocator acefAlloc{};
		AcefData acefdata{};
	}

	void LoadExtensionFiles() {
		static std::once_flag of;

		std::call_once(of, [] {
			std::vector<std::filesystem::path> files{};

			utils::GetFileRecurseExt(utils::GetProgDir() / "package_index", files, ".acef\0");

			for (const std::filesystem::path& path : files) {
				// load acpf files
				LOG_DEBUG("read ACEF {}", path.string());

				try {
					utils::InFileCE is{ path, true };

					core::bytebuffer::FileReader reader{ is };

					if (reader.Read<uint32_t>() != ACEF_MAGIC) {
						throw std::runtime_error("not an acef file");
					}
					
					uint16_t version{ reader.Read<uint16_t>() };
					uint16_t flags{ reader.Read<uint16_t>() };

					if (version < ACEF_VERSION_MIN) {
						throw std::runtime_error(std::format("too old 0x{:x}", version));
					}

					std::endian fileEndian = flags & ACEF_BIGENDIAN ? std::endian::big : std::endian::little;

					if (fileEndian != std::endian::native) {
						throw std::runtime_error("non native endian");
					}

					// read header
					uint32_t headerSize{ reader.Read<uint32_t>() };
					AcefHeader header{};

					reader.Goto(0);
					reader.Read(&header, std::min<size_t>(sizeof(header), (uint32_t)headerSize));

					utils::compress::CompressionAlgorithm compress{ (utils::compress::CompressionAlgorithm)header.compress };

					if ((compress & utils::compress::COMP_TYPE_MASK) >= utils::compress::COMP_COUNT) {
						throw std::runtime_error(std::format("unknown compression format {}", compress));
					}

					//LOG_TRACE("{} v{:x}: {}/{}B->{}B/h=0x{:x}/hb=0x{:x} * {}",
					//		  path.filename().string(), header.version,
					//		  compress,
					//		  utils::FancyNumber(header.compressedSize),
					//		  utils::FancyNumber(header.decompressedSize),
					//		  header.headerSize,
					//		  header.sizeObjectHeader,
					//		  header.numObjects							  
					//);

					byte* data{ acefAlloc.Alloc<byte>(header.decompressedSize) };
					auto sourceWindow{ std::make_unique<byte[]>(header.windowSize)};

					// skip future header data
					reader.Goto(headerSize);

					AcefBlockHeader blockHeader{};
					for (size_t i = 0; i < header.numObjects; i++) {
						// read object header
						size_t oloc{ reader.Loc() };
						reader.Read(&blockHeader, std::min<size_t>(sizeof(blockHeader), (uint32_t)header.sizeObjectHeader));

						
						//LOG_TRACE("B#{} #{:x}: 0x{:x}->0x{:x}/loc=0x{:x}#0x{:x}/data=0x{:x}",
						//		  i,
						//		  blockHeader.blockId,
						//		  blockHeader.blockSize,
						//		  blockHeader.decompSize,
						//		  blockHeader.checkLoc, oloc,
						//		  blockHeader.dataOffset
						//);


						if (oloc != blockHeader.checkLoc) throw std::runtime_error("invalid block location check");
						
						reader.Goto(blockHeader.checkLoc + header.sizeObjectHeader);
						
						if (blockHeader.blockSize > header.windowSize) throw std::runtime_error("window too small");
						if (blockHeader.dataOffset + blockHeader.decompSize > header.decompressedSize) throw std::runtime_error("too many data in block");

						byte* decomp{ data + blockHeader.dataOffset };

						reader.Read(sourceWindow.get(), blockHeader.blockSize);

						int r{ utils::compress::Decompress2(compress, decomp, blockHeader.decompSize, sourceWindow.get(), blockHeader.blockSize) };

						if (r < 0) {
							throw std::runtime_error(std::format("can't decompress block: {}", utils::compress::DecompressResultName(r)));
						}

						// add data to registry
						acefdata.map[blockHeader.blockId].emplace_back((size_t)r, decomp);
					}
				} catch (std::runtime_error& e) {
					LOG_WARNING("Can't load acef file {}: {}", path.string(), e.what());
				}
			}

		});
	}


	void GetExtensionData(uint64_t path, AcefArray** data, size_t* count) {
		LoadExtensionFiles(); // force loading of the data

		auto it{ acefdata.map.find(path) };

		if (it == acefdata.map.end()) {
			*count = 0;
			return;
		}

		*data = it->second.data();
		*count = it->second.size();
	}
}