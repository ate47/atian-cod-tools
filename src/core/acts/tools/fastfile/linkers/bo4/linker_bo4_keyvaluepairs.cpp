#include <includes.hpp>
#include <rapidcsv.h>
#include <tools/fastfile/linkers/linker_bo4.hpp>

namespace {
	using namespace fastfile::linker::bo4;

	struct KeyValuePair {
		uint64_t keyHash;
		const char* value;
	};

	struct KeyValuePairs {
		XHash name;
		int32_t numVariables;
		KeyValuePair* keyValuePairs;
	};
	static_assert(sizeof(KeyValuePairs) == 0x20);

	class XAssetLinkerImpl : public XAssetLinker {
	public:
		using XAssetLinker::XAssetLinker;

		void Compute(BO4LinkContext& ctx, const char* id, BO4FFContext& ff) override {
			std::filesystem::path rfpath{ id };
			std::filesystem::path path{ ctx.linkCtx.input / rfpath };
			utils::InFileCE is{ path };
			if (!is) {
				LOG_ERROR("Can't read {}", path.string());
				ctx.error = true;
				return;
			}

			rapidcsv::Document doc{};

			doc.Load(is, rapidcsv::LabelParams(-1, -1), rapidcsv::SeparatorParams(','));

			if (doc.GetColumnCount() != 2) {
				LOG_ERROR("Invalid keyvaluepairs csv format in {}", path.string());
				ctx.error = true;
				return;
			}

			ff.data.PushStream(XFILE_BLOCK_TEMP);

			KeyValuePairs& table{ ff.data.AllocStreamRef<KeyValuePairs>() };
			table.name.name = ctx.HashPathName(rfpath);

			ff.data.PushStream(XFILE_BLOCK_VIRTUAL);

			if (table.numVariables) {
				ff.data.Align(8);
				table.numVariables = (int32_t)doc.GetRowCount();
				table.keyValuePairs = (KeyValuePair*)fastfile::linker::memory::POINTER_NEXT;

				KeyValuePair* array{ ff.data.AllocStreamPtr<KeyValuePair>(doc.GetRowCount()) };
			
				for (size_t i = 0; i < doc.GetRowCount(); ++i) {
					KeyValuePair& kvp{ array[i] };
					std::string k{ doc.GetCell<std::string>(0, i) };
					std::string v{ doc.GetCell<std::string>(1, i) };
					
					if (hash::TryHashPattern(k.data(), kvp.keyHash)) {
						kvp.keyHash = hash::HashKVP(k.data());
					}
					ff.data.AddXString(kvp.value, v.data());
				}
			}

			ff.data.PopStream();

			ff.data.PopStream();

			LOG_INFO("Added asset keyvaluepairs {} (hash_{:x})", rfpath.string(), table.name.name);
		}
	};

	utils::MapAdder<XAssetLinkerImpl, XAssetType, XAssetLinker> impl{ GetWorkers(), XAssetType::ASSET_TYPE_KEYVALUEPAIRS };
}