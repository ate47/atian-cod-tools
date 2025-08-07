#include <includes.hpp>
#include <tools/fastfile.hpp>
#include <tools/ff/fastfile_handlers.hpp>

namespace {
	
	class ZoneFFDecompressor : public fastfile::FFDecompressor {
	public:
		ZoneFFDecompressor() : fastfile::FFDecompressor("Uncompressed Zone") {}

		bool MatchFile(const std::filesystem::path& path, core::bytebuffer::ByteBuffer& data) const override {
			std::filesystem::path ext{ path.extension() };
			return ext.string() == ".zone";
		}
		
		void LoadFastFile(fastfile::FastFileOption& opt, core::bytebuffer::ByteBuffer& reader, fastfile::FastFileContext& ctx, std::vector<byte>& ffdata) override {
			byte* data{ reader.Ptr() };
			ffdata.insert(ffdata.begin(), data, data + reader.Remaining());

			// knowing we don't have the ff header, I don't think we can get the block sizes
			// ctx.blocksCount = ???;
			// ctx.blockSizes[i].size = ???;

			// use the filename because no header
			std::filesystem::path ffnamet{ ctx.file };
			ffnamet.replace_extension(); // .zone
			if (ffnamet.extension() == ".ff") ffnamet.replace_extension(); // .ff

			ffnamet = ffnamet.filename();
			std::string ffnamets{ ffnamet.string() };
			sprintf_s(ctx.ffname, "%s", ffnamets.data());
		}
	};

	utils::ArrayAdder<ZoneFFDecompressor, fastfile::FFDecompressor> arr{ fastfile::GetDecompressors() };
}