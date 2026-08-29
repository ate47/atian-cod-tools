#include <fstream>
#include <filesystem>
#include <format>
#include <string>
#include <iostream>
#include <map>
#include <nlohmann/json.hpp>
#include <inja/inja.hpp>

namespace {
    bool ReadFile(const std::filesystem::path& path, std::string& buffer) {
        std::ifstream in{ path, std::ios::binary };
        if (!in) {
            return false;
        }

        in.seekg(0, std::ios::end);
        size_t length = in.tellg();
        in.seekg(0, std::ios::beg);
        buffer.resize(length);
        in.read(buffer.data(), length);

        in.close();
        return true;
    }
} // namespace

int main(int argc, char** argv) {
    if (argc < 4) {
        std::cerr << argv[0] << " [dir] [type] [in] [out]\n";
        return -1;
    }

    std::filesystem::path dir{ argv[1] };
    const char* gameId{ argv[2] };
    std::filesystem::path input{ argv[3] };
    std::filesystem::path outHPPPath{ std::format("{}.hpp.inl", argv[4]) };
    std::filesystem::path outCPPPath{ std::format("{}.cpp.inl", argv[4]) };

    std::ifstream in{ input };

    std::string tempHPP;
    std::string tempCPP;

    if (!ReadFile(dir / "template.hpp.inja", tempHPP)) {
        std::cerr << "Can't read template.hpp.inja\n";
        return -1;
    }

    if (!ReadFile(dir / "template.cpp.inja", tempCPP)) {
        std::cerr << "Can't read template.cpp.inja\n";
        return -1;
    }

    if (!in) {
        std::cerr << "Can't open " << input << "\n";
        return -1;
    }

    std::ofstream outHPP{ outHPPPath, std::ios::binary };
    if (!outHPP) {
        in.close();
        std::cerr << "Can't open " << outHPPPath << "\n";
        return -1;
    }

    std::ofstream outCPP{ outCPPPath, std::ios::binary };
    if (!outCPP) {
        in.close();
        outHPP.close();
        std::cerr << "Can't open " << outCPPPath << "\n";
        return -1;
    }

    nlohmann::json templateData;
    try {
        in >> templateData;
    } catch (std::runtime_error& e) {
        in.close();
        std::cerr << "Can't read template data " << input << ": " << e.what() << "\n";
        return -1;
    }
    in.close();

    templateData["gameId"] = gameId;
    templateData["inputFileName"] = input.filename().string();
    templateData["outFileNameHPP"] = outHPPPath.filename().string();
    templateData["outFileNameCPP"] = outCPPPath.filename().string();
    int r{};
    try {
        inja::render_to(outHPP, tempHPP, templateData);
    } catch (std::runtime_error& e) {
        std::cerr << "Can't generate " << outHPPPath << ": " << e.what() << "\n";
        r = -1;
    }
    try {
        inja::render_to(outCPP, tempCPP, templateData);
    } catch (std::runtime_error& e) {
        std::cerr << "Can't generate " << outCPPPath << ": " << e.what() << "\n";
        r = -1;
    }

    outHPP.close();
    outCPP.close();

    return r;
}