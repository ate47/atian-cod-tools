#include <includes.hpp>
#include <cli/cli_options.hpp>
#include <utils/utils.hpp>
#include <utils/data_utils.hpp>

#include <tools/gsc/gsc_obfuscator.hpp>
#include <tools/gsc/obfuscator/gsc_obfuscator_private_file.hpp>
#include <tools/gsc/obfuscator/gsc_obfuscator_handler.hpp>
#include <tools/gsc/obfuscator/gsc_obfuscator_finder.hpp>
#include <tools/gsc/obfuscator/fastfile.hpp>

namespace {
    void HandleGscObject(tool::gsc::obfuscator::options::GscObfOptions& opt, byte* file, size_t fileSize) {
        tool::gsc::obfuscator::handler::GscObfuscator obf{ opt, file, fileSize };
        obf.RunTasks();
    }

    void HandleFFFile(
        tool::gsc::obfuscator::options::GscObfOptions& opt, const std::filesystem::path& in,
        const std::filesystem::path& out
    ) {
        std::vector<byte> buffer;

        if (!utils::ReadFile(in, buffer)) {
            throw std::runtime_error(std::format("Can't read {}", in.string()));
        }
        tool::gsc::obfuscator::fastfile::FastfileInfo info{
            tool::gsc::obfuscator::fastfile::DecompressFastfile(buffer.data(), buffer.size())
        };

        // todo: apply patches on data
        std::vector<tool::gsc::obfuscator::finder::GscObject> objects{
            tool::gsc::obfuscator::finder::FindGscInBuffer(info.out.data(), info.out.size())
        };

        LOG_DEBUG("found {} gsc objects", objects.size());

        for (size_t i = 0; i < objects.size(); i++) {
            tool::gsc::obfuscator::finder::GscObject& obj{ objects[i] };
            LOG_INFO(
                "{}/{} patching {}::{} (size=0x{:x})",
                i + 1,
                objects.size(),
                info.header.fastfileName,
                obj.name,
                obj.fileSize
            );
            opt.privateFileData.RenamedScript(obj.name);
            HandleGscObject(opt, obj.obj, obj.fileSize);
        }

        // reshape the fastfile code
        if (opt.fastfileBuilder) {
            sprintf_s(info.header.builder, "%s", opt.fastfileBuilder);
        }

        if (opt.fastfileCompression) {
            info.header.compression = ::fastfile::GetFastFileCompression(opt.fastfileCompression);
            if (info.header.compression >= ::fastfile::XFILE_BDELTA_UNCOMP) {
                throw std::runtime_error(std::format("Invalid fastfile compression: {}", opt.fastfileCompression));
            }
        }

        tool::gsc::obfuscator::fastfile::CompressFastfile(buffer, info);

        // write back the file

        if (!utils::WriteFile(out, buffer)) {
            throw std::runtime_error(std::format("Can't write {}", out.string()));
        }
        LOG_INFO("Fastfile {} recompressed into {}", info.header.fastfileName, out.string());
    }

    void HandleGSCFile(
        tool::gsc::obfuscator::options::GscObfOptions& opt, const std::filesystem::path& in,
        const std::filesystem::path& out
    ) {
        std::vector<byte> buffer;

        if (!utils::ReadFile(in, buffer)) {
            throw std::runtime_error(std::format("Can't read {}", in.string()));
        }

        HandleGscObject(opt, buffer.data(), buffer.size());

        // write back the file

        if (!utils::WriteFile(out, buffer)) {
            throw std::runtime_error(std::format("Can't write {}", out.string()));
        }
        LOG_DEBUG("Write back {}", out.string());
    }

    void HandleFile(
        tool::gsc::obfuscator::options::GscObfOptions& opt, const std::filesystem::path& in,
        const std::filesystem::path& out
    ) {

        LOG_INFO("Reading {} to {}...", in.string(), out.string());
        std::filesystem::create_directories(out.parent_path());

        if (in.extension() == ".ff") {
            // convert fastfile
            HandleFFFile(opt, in, out);
        } else if (in.extension() == ".gscc" || in.extension() == ".cscc") {
            // convert gsc
            HandleGSCFile(opt, in, out);
        } else {
            throw std::runtime_error(
                std::format(
                    "Invalid extension for file {}, only fastfile and compiled gsc files are accepted",
                    in.string()
                )
            );
        }
    }

    int gsco(int argc, const char* argv[]) {
        cli::options::CliOptions opts{};

        tool::gsc::obfuscator::options::GscObfOptions opt{};
        opts.addOption(&opt.printHelp, "show help", "--help", "", "-h");
        opts.addOption(&opt.printData, "print script header", "--header", "", "-H");
        opts.addOption(&opt.noDebugKill, "no debug data kill", "--no-debug", "", "-d");
        opts.addOption(&opt.noRemoveLocals, "no remove locals", "--no-locals");
        opts.addOption(&opt.noRemovePrivateExports, "no remove private exports", "--no-private");
        opts.addOption(&opt.noTrampoline, "no trampoline build", "--no-trampoline", "", "-t");
        opts.addOption(&opt.recomputeCRC, "recompute export crc", "--export-crc-recomp", "", "-r");
        opts.addOption(&opt.privateFile, "private file", "--private", " (file)", "-p");
        opts.addOption(&opt.fastfileBuilder, "replace fastfile builder name", "--fastfile-builder", " (builder)");
        opts.addOption(
            &opt.fastfileCompression,
            "replace fastfile compression (uncompressed, lz4, lz4_hc, zlib or zlib_hc)",
            "--fastfile-compression",
            " (compression)"
        );
        opts.addOption(&opt.output, "output dir (default: output)", "--output", " (path)", "-o");

        if (!opts.ComputeOptions(1, argc, argv) || opt.printHelp || opts.NotEnoughParam(1)) {
            LOG_INFO("usage: {} (file)", argv[0]);
            opts.PrintOptions();
            return opt.printHelp ? 0 : -1;
        }

        if (opt.privateFile) {
            try {
                if (!opt.privateFileData.ReadFile(opt.privateFile)) {
                    return -1;
                }
            } catch (std::runtime_error& err) {
                LOG_ERROR("Can't load private file: {}", err.what());
                return -1;
            }
        }

        std::filesystem::path outDir{ opt.output };
        int r{};

        utils::Timestamp startMS{ utils::GetTimestamp() };

        for (size_t i = 0; i < opts.ParamsCount(); i++) {
            std::vector<std::filesystem::path> paths{};
            std::filesystem::path parent{ opts[i] };
            if (std::filesystem::is_regular_file(parent)) {
                paths.emplace_back(parent.filename());
                parent = std::filesystem::absolute(parent).parent_path();
            } else {
                utils::GetFileRecurseExt(parent, paths, ".gscc\0.cscc\0.ff\0", true);

                if (paths.empty()) {
                    LOG_WARNING("Can't find compiled gsc files (.gscc/.cscc) in {}", opts[i]);
                    continue;
                }
            }

            for (const std::filesystem::path& path : paths) {
                const std::filesystem::path inFile{ parent / path };
                std::filesystem::path outFile{ outDir / path };
                try {
                    HandleFile(opt, inFile, outFile);
                } catch (std::runtime_error& err) {
                    LOG_ERROR("Error when handling {}: {}", inFile.string(), err.what());
                    r = -1;
                }
            }
        }
        utils::Timestamp endMS{ utils::GetTimestamp() };
        double delta{ (double)((endMS - startMS) / 100) / 10 };

        if (r) {
            LOG_ERROR("Error during obfuscation");
        } else {
            LOG_INFO("obfuscation in {}s", delta);
        }

        return r;
    }

    ADD_TOOL(gsco, "gsc", " [files*]", "obfuscate GSC files", gsco);
} // namespace