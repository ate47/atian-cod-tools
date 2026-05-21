#include <Windows.h>
#include <utils/utils.hpp>
#include <acts_api/internal_ui.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPSTR lpCmdLine, int nShowCmd) {
    int argCount;
    LPWSTR* szArgList{ CommandLineToArgvW(GetCommandLineW(), &argCount) };

    std::vector<const char*> argvv;
    std::vector<std::string> argvs;

    argvs.reserve(argCount);
	argvv.resize(argCount);

    for (size_t i = 0; i < argCount; i++) {
        argvs.emplace_back(utils::WStrToStr(szArgList[i]));
    }
    for (size_t i = 0; i < argCount; i++) {
        argvv[i] = argvs[i].data();
    }

	return MainActsUI(argCount, argvv.data());
}
