#include <includes_shared.hpp>
#include <core/updater.hpp>
#include <utils/memapi.hpp>
#include <cli/cli_options.hpp>

int main(int argc, char const *argv[]) {
    cli::options::CliOptions opts{};

    bool ui{};
    bool update{};
    bool force{};
    bool help{};
    size_t maxWait{ 10 };

    opts
        .addOption(&help, "help", "--help", "", "-h")
        .addOption(&ui, "ui", "--ui", "", "-u")
        .addOption(&force, "force update", "--force", "", "-f")
        .addOption(&update, "run update", "--update", "", "-U")
        .addOption(&maxWait, "max wait time (s)", "--maxWait", " [time]", "-w")
        ;

    if (!opts.ComputeOptions(1, argc, argv) || help) {
        opts.PrintOptions();
        return help ? 0 : -1;
    }

    if (!update) {
        // we are updating the process
        if (force) {
            if (!core::updater::CheckUpdate(true, false, ui)) {
                LOG_INFO("Already up to date");
            }
        }
        else {
            if (!core::updater::FindUpdate(!ui)) {
                LOG_INFO("Already up to date");
            }
        }
        return 0;
    }
    // wait for acts to stop
    size_t i;
    for (i = 1; i <= maxWait; i++) {
        Process acts{ L"acts.exe" };
        Process actsui{ L"acts-ui.exe" };
        
        if (!actsui) {
            if (!acts) {
                break;
            }
            else {
                LOG_INFO("Waiting for acts.exe {}... ({}/{})", acts, i, maxWait);
            }
        }
        else {
            LOG_INFO("Waiting for acts-ui.exe {}... ({}/{})", actsui, i, maxWait);
        }
        Sleep(1000);
    }
    
    if (i > maxWait) {
        LOG_ERROR("Waited too long for stop");
        return -1;
    }

    core::updater::ApplyUpdate(ui);
    return 0;
}
