#include <includes.hpp>
#include <tools/fastfile/handlers/handler_game_bo4.hpp>
#include <hook/error.hpp>

namespace {
    using namespace fastfile::handlers::bo4;
    struct PhysPreset {
        XHash name;
        int flags;
        float mass;
        float bounce;
        float friction;
        float damping_linear;
        float damping_angular;
        float bulletForceScale;
        float explosiveForceScale;
        int canFloat;
        float gravityScale;
        vec3_t centerOfMassOffset;
        vec3_t buoyancyBoxMin;
        vec3_t buoyancyBoxMax;
        FxEffectDef* trailFX;
        FxImpactTable* impactFX;
        SoundsImpactTable* impactSounds;
    };

    static_assert(sizeof(PhysPreset) == 0x78);

    class ImplWorker : public Worker {
        using Worker::Worker;

        void Unlink(fastfile::FastFileOption& opt, void* ptr) {
            PhysPreset* asset{ (PhysPreset*)ptr };

            std::filesystem::path outFile{ opt.m_output / "bo4" / "source" / "tables" / "physpreset" /
                                           fastfile::GetCurrentContext().ffname /
                                           std::format("{}.json", hashutils::ExtractTmp("file", asset->name)) };
            std::filesystem::create_directories(outFile.parent_path());
            BO4JsonWriter json{};

            LOG_OPT_INFO("Dump physpreset {}", outFile.string());

            json.BeginObject();

            json.WriteFieldValueXHash("name", asset->name);
            XHash name;
            json.WriteFieldValueNumber("flags", asset->flags);
            json.WriteFieldValueNumber("mass", asset->mass);
            json.WriteFieldValueNumber("bounce", asset->bounce);
            json.WriteFieldValueNumber("friction", asset->friction);
            json.WriteFieldValueNumber("damping_linear", asset->damping_linear);
            json.WriteFieldValueNumber("damping_angular", asset->damping_angular);
            json.WriteFieldValueNumber("bulletForceScale", asset->bulletForceScale);
            json.WriteFieldValueNumber("explosiveForceScale", asset->explosiveForceScale);
            json.WriteFieldValueNumber("canFloat", asset->canFloat);
            json.WriteFieldValueNumber("gravityScale", asset->gravityScale);
            json.WriteFieldValueVector("centerOfMassOffset", asset->centerOfMassOffset);
            json.WriteFieldValueVector("buoyancyBoxMin", asset->buoyancyBoxMin);
            json.WriteFieldValueVector("buoyancyBoxMax", asset->buoyancyBoxMax);
            json.WriteFieldValueXAsset("trailFX", games::bo4::pool::ASSET_TYPE_FX, asset->trailFX);
            json.WriteFieldValueXAsset("impactFX", games::bo4::pool::ASSET_TYPE_IMPACT_FX, asset->impactFX);
            json.WriteFieldValueXAsset("impactSounds", games::bo4::pool::ASSET_TYPE_IMPACT_SOUND, asset->impactSounds);

            json.EndObject();

            if (!json.WriteToFile(outFile)) {
                LOG_ERROR("Error when dumping {}", outFile.string());
            }
        }
    };

    utils::MapAdder<ImplWorker, games::bo4::pool::XAssetType, Worker> impl{
        GetWorkers(), games::bo4::pool::XAssetType::ASSET_TYPE_PHYSPRESET
    };
} // namespace