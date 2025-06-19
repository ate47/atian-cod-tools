#pragma once
#include <tools/utils/ps4_process.hpp>

namespace tool::ps4::codtool {
    struct CodToolHandler;

	struct CodTool {
        char gscFileIn[MAX_PATH + 1]{};
        char cbuffIn[0x100]{};
        char ps4In[0x50]{};
        char handlernameDefault[1]{};
        const char* handlername{};

        std::unique_ptr<utils::ps4::PS4Process> ps4{};

        CodToolHandler* handler{};

        std::string notif{};
        bool loadAfter{};


        void Handle();
        void Load();
	};

    extern CodTool codTool;

    struct CodToolHandler {
        const char* name;
        const char* process;

        void (*CBuf)(CodTool* tool, const char* cbuf) {};
        void (*InjectGSC)(CodTool* tool, const std::filesystem::path& path) {};
    };

    std::vector<CodToolHandler*>& GetHandlers();
    CodToolHandler* GetHandler(const char* name);
}