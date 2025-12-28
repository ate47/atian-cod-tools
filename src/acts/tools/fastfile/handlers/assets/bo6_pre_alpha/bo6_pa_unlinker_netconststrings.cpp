#include <includes.hpp>
#include <tools/fastfile/handlers/handler_assets.hpp>
#include <tools/bo6/bo6.hpp>


namespace fastfile::handlers::assets {
	union NetConstString {
		const char* str;
		XHash64 hash;
	};

	struct NetConstStrings {
		XHash64 name;
		uint32_t unk8;
		byte unk0c;
		bool useHash;
		uint32_t unk10;
		uint32_t stringsCount;
		uint32_t unk18;
		NetConstString* strings;
	};


	class ImplWorker : public Worker {
		using Worker::Worker;

		void Unlink(fastfile::FastFileOption& opt, core::bytebuffer::ByteBuffer& reader, void** handle) override {
			NetConstStrings* ncs{ reader.ReadPtr<NetConstStrings>() };
			*handle = ncs;

			if (ncs->strings) {
				ncs->strings = reader.ReadPtr<NetConstString>(ncs->stringsCount);
				if (!ncs->useHash) {
					// todo
				}
			}
		}
	};
	utils::MapAdder<ImplWorker, uint64_t, Worker> impl{ GetWorkers(), AssetGameRef(GameId::GID_BO6, GameRevId::GRID_BO6_PRE_ALPHA, bo6::T10AssetType::T10_ASSET_NETCONSTSTRINGS) };
}