#include <includes.hpp>
#include <tools/fastfile/handlers/handler_game_bo4.hpp>
#include <tools/fastfile/handlers/bo4/bo4_unlinker_keyvaluepairs.hpp>

namespace fastfile::handlers::bo4::keyvaluepairs {
    namespace {
        struct KVPName {
            const char* key;
            uint32_t val;

            constexpr KVPName(const char* name) : key(name), val(hash::HashKVP(name)) {}
        };

        constinit KVPName knownKVPNames[]{
            "",
            "actor_shadow_count",
            "dont_force_semantic",
            "fog_sun_pos_gdt_enable",
            "force_character_models",
            "force_static_models",
            "force_view_models",
            "force_world_materials",
            "forced_model_lods",
            "initial_materials",
            "initial_xmodels",
            "initial",
            "omni_slice_scale_split_screen",
            "only_stream_visible",
            "spot_slice_scale_split_screen",
            "sst_mip_drop_clamp",
            "sun_border_padding",
            "sun_disable_caching_split_screen",
            "sun_umbra_planar",
            "trophies",
            "tu_version",
            "umbra_accurate_occlusion_threshold",
            "umbra_distance_scale",
            "umbra_frustum_grid_height",
            "umbra_frustum_grid_width",
            "umbra_min_relative_contribution",
            "xpak_read",
            "wz_as_mp",
            "mp_arena",
        };
    } // namespace

    const char* GetName(uint64_t hash) {
        for (const KVPName& n : knownKVPNames) {
            if (n.val == hash) {
                return n.key;
            }
        }
        return utils::va("hash_%llx", hash);
    }

    namespace {
        class KVPWorker : public Worker {

            void Unlink(fastfile::FastFileOption& opt, void* ptr) override {
                struct KeyValuePair {
                    int64_t keyHash;
                    const char* value;
                };

                struct KeyValuePairs {
                    XHash name;
                    int numVariables;
                    KeyValuePair* keyValuePairs;
                };
                static_assert(sizeof(KeyValuePairs) == 0x20);
                KeyValuePairs* asset{ (KeyValuePairs*)ptr };

                std::filesystem::path outFile{ opt.m_output / "bo4" / "source" / "tables" / "keyvaluepairs" /
                                               fastfile::GetCurrentContext().GetFFType() /
                                               std::format("{}.csv", hashutils::ExtractTmp("file", asset->name.name)) };

                std::filesystem::create_directories(outFile.parent_path());

                utils::OutFileCE os{ outFile };

                if (!os) {
                    LOG_ERROR("Error when dumping {}", outFile.string());
                }

                LOG_OPT_INFO("Dump keyvaluepairs {}", outFile.string());

                os << "key,value";

                for (size_t i = 0; i < asset->numVariables; i++) {
                    KeyValuePair& pair{ asset->keyValuePairs[i] };
                    os << "\n" << GetName(pair.keyHash) << "," << GetValidString(pair.value);
                }
            }
        };

        utils::MapAdder<KVPWorker, games::bo4::pool::XAssetType, Worker> impl{
            GetWorkers(), games::bo4::pool::XAssetType::ASSET_TYPE_KEYVALUEPAIRS
        };
    } // namespace
} // namespace fastfile::handlers::bo4::keyvaluepairs