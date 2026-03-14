#pragma once
#include <tools/fastfile/fastfile_handlers.hpp>

namespace fastfile::handlers::assets {
	constexpr uint64_t AssetGameRef(fastfile::GameId gid, fastfile::GameRevId grid, uint32_t assetType) {
		return (uint64_t)assetType | ((uint64_t)gid << 32) | ((uint64_t)grid << 48);
	}

	typedef uint64_t XHash64;
	typedef uint64_t XHash32;

	struct ScrString_t {
		uint32_t id;

		operator uint32_t() {
			return id;
		}
	};

	class Worker {
	public:
		Worker() {}
		virtual void Unlink(fastfile::FastFileOption& opt, core::bytebuffer::ByteBuffer& reader, void** handle) = 0;
		virtual void PreXFileLoading(fastfile::FastFileOption& opt, fastfile::FastFileContext& ctx) {}
		virtual void PostXFileLoading(fastfile::FastFileOption& opt, fastfile::FastFileContext& ctx) {}
	};

	std::unordered_map<uint64_t, Worker*>& GetWorkers();

	bool UnlinkTypeRef(uint64_t ref, fastfile::FastFileOption& opt, core::bytebuffer::ByteBuffer& reader, void** handle);
	void UnlinkType(uint64_t ref, fastfile::FastFileOption& opt, core::bytebuffer::ByteBuffer& reader, void** handle);
	const char* GetScrString(ScrString_t id);
}