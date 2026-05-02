#include <includes.hpp>
#include <tools/fastfile/fastfile_handlers.hpp>
#include <tools/fastfile/fastfile_asset_pool.hpp>
#include <acts_api/fastfile.h>

namespace {
	using namespace fastfile;
	core::memory_allocator::MemoryAllocator customHandlerAllocator{};


	class CustomFFHandler : public fastfile::FFHandler {
	public:
		void* ud;
		ActsAPIFastFile_InitHandler pInit;
		ActsAPIFastFile_HandleHandler pHandle;
		ActsAPIFastFile_CleanupHandler pCleanup;


		CustomFFHandler(
			ActsAPIFastFile_HandleHandler pHandle, ActsAPIFastFile_CleanupHandler pCleanup, ActsAPIFastFile_InitHandler pInit, void* ud,
			const char* name, const char* description, compatibility::scobalula::csi::CordycepGame game, bool noPatchOk = false)
			: 
			pHandle(pHandle), pCleanup(pCleanup), pInit(pInit), ud(ud),
			fastfile::FFHandler(name, description, game, noPatchOk) {
		}

		void Init(fastfile::FastFileOption& opt) override {
			if (pInit && !pInit(&opt, ud)) {
				throw std::runtime_error("Failed to init fastfile handler");
			}
		}

		void Cleanup() override {
			if (pCleanup) {
				pCleanup(ud);
			}
		}

		void Handle(FastFileOption& opt, core::bytebuffer::ByteBuffer& reader, FastFileContext& ctx) override {
			if (pHandle && !pHandle(&opt, &ctx, reader.Ptr(), reader.Remaining(), ud)) {
				throw std::runtime_error("Failed to handle fastfile");
			}
		}
	};
}

void ActsAPIFastFile_RegisterCustomHandler(
	ActsAPIFastFile_InitHandler init, ActsAPIFastFile_HandleHandler handle, ActsAPIFastFile_CleanupHandler cleanup, void* userdata,
	const char* name, const char* description, ActsAPIFastFile_CordycepGame cordycepGame, bool noPatchOk
) {
	std::vector<fastfile::FFHandler*>& handlers{ fastfile::GetHandlers() };

	handlers.push_back(customHandlerAllocator.New<CustomFFHandler>(handle, cleanup, init, userdata, name, description, (compatibility::scobalula::csi::CordycepGame)cordycepGame, noPatchOk));
}