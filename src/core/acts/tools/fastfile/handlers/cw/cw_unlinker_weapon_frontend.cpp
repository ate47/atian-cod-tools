#include <includes.hpp>
#include <core/raw_file_json.hpp>
#include <core/hashes/raw_file_extractor.hpp>
#include <tools/fastfile/handlers/handler_game_cw.hpp>
#include <tools/fastfile/handlers/cw/cw_unlinker_scriptbundle.hpp>

namespace {
    using namespace fastfile::handlers::cw;

    class ImplWorker : public Worker {
        void Unlink(fastfile::FastFileOption& opt, void* ptr) override {
            struct WeaponFrontend {
                CWXHash name;
                scriptbundle::SB_ObjectsArray bundle;
            };

            static_assert(sizeof(WeaponFrontend) == 0x28);
            WeaponFrontend* asset{ (WeaponFrontend*)ptr };

            std::filesystem::path outFile{ opt.m_output / "cw" / "source" / "tables" / "weaponfrontend" /
                                           std::format("{}.json", hashutils::ExtractTmp("file", asset->name)) };

            BOCWJsonWriter json{};

            json.BeginObject();
            json.WriteFieldValueXHash("name", asset->name);
            scriptbundle::WriteObject(json, "bundle", asset->bundle);
            json.EndObject();

            std::filesystem::create_directories(outFile.parent_path());
            if (!json.WriteToFile(outFile)) {
                LOG_ERROR("Can't write to {}", outFile.string());
                return;
            }
            LOG_OPT_INFO("Dump WeaponFrontend {}", outFile.string());
        }
    };

    utils::MapAdder<ImplWorker, XAssetType, Worker> impl{ GetWorkers(), XAssetType::ASSET_TYPE_WEAPONFRONTEND };
} // namespace