#include <includes.hpp>
#include <tools/ff/handlers/handler_assets.hpp>
#include <tools/bo6/bo6.hpp>


namespace fastfile::handlers::assets {

	class ImplWorker : public Worker {
		using Worker::Worker;

		void Unlink(fastfile::FastFileOption& opt, core::bytebuffer::ByteBuffer& reader, void** handle) override {


		}
	};
	//utils::MapAdder<ImplWorker, uint64_t, Worker> impl{ GetWorkers(), AssetGameRef(GameId::GID_BO6, GameRevId::GRID_BO6_PRE_ALPHA, bo6::T10AssetType::T10_ASSET_LOCALIZE) };
}