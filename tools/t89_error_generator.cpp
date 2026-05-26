#include <fstream>
#include <string>
#include <iostream>

int main(int argc, char **argv) {
    if (argc < 3) {
        std::cerr << argv[0] << " [in] [out]\n";
        return -1;
    }

    std::ifstream in{ argv[1] };

    if (!in) {
        std::cerr << "Can't open " << argv[1] << "\n";
        return -1;
    }
    std::ofstream out{ argv[2] };
    if (!out) {
        in.close();
        std::cerr << "Can't open " << argv[2] << "\n";
        return -1;
    }

    out << "// Auto-generated file, do not write in it\n\n";
    out << "static const struct { uint32_t code; const char* msg; } ERRORS[] {\n";

    std::string render, code, msg;
    while (std::getline(in, render, '\t') &&
           std::getline(in, code, '\t') &&
           std::getline(in, msg))
    {

        // escape quotes
        for (size_t i = 0; i < msg.size(); ++i)
            if (msg[i] == '"')
                msg.insert(i++, "\\");

        out << "    { " << code << ", \"" << msg << "\" },";
        
        if (!render.empty()) {
            out << " // " << render;
        }

        out << "\n";
    }

    out << "};\n";

    in.close();
    out.close();

    std::cout << "generated t89-errors into " << argv[2] << "\n";


    return 0;
}
