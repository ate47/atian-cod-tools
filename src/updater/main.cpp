#include <includes_shared.hpp>
#include <core/updater.hpp>
#include <utils/memapi.hpp>

int main(int argc, char const *argv[]) {
    constexpr size_t maxWait = 10;
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

    core::updater::ApplyUpdate();
    return 0;
}
