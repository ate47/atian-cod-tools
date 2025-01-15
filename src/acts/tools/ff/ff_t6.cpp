#include <includes.hpp>
#include <deps/oodle.hpp>
#include <tools/fastfile.hpp>
#include <tools/fastfile_handlers.hpp>
#include <games/bo4/pool.hpp>

namespace {
	static deps::oodle::Oodle oodle{};

	class T6FFHandler : public fastfile::FFHandler {
	public:
		T6FFHandler() : fastfile::FFHandler("Black Ops 2 Handler", 0x3030303066664154) {}

		void LoadFastFile(fastfile::FFAssetPool& pool, fastfile::FastFileOption& opt, core::bytebuffer::ByteBuffer& reader, const char* file) {

		}