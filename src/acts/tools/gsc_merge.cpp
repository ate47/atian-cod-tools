#include <includes.hpp>
#include "tools/gsc.hpp"
#include "tools/gsc_opcodes.hpp"

namespace {
	using namespace tool::gsc;
	using namespace tool::gsc::opcode;



	struct MergeGscOpt {
		bool m_help = false;
		Platform m_platform = Platform::PLATFORM_PC;


		std::vector<const char*> m_inputFiles{};

		bool Compute(const char** args, INT startIndex, INT endIndex) {
			// default values
			for (size_t i = startIndex; i < endIndex; i++) {
				const char* arg = args[i];

				if (!strcmp("-?", arg) || !_strcmpi("--help", arg) || !strcmp("-h", arg)) {
					m_help = true;
				}
				else if (!strcmp("-t", arg) || !_strcmpi("--type", arg)) {
					if (i + 1 == endIndex) {
						std::cerr << "Missing value for param: " << arg << "!\n";
						return false;
					}
					m_platform = opcode::PlatformOf(args[++i]);

					if (!m_platform) {
						std::cerr << "Unknown platform: " << args[i] << "!\n";
						return false;
					}
				}
				else if (*arg == '-') {
					std::cerr << "Unknown option: " << arg << "!\n";
					return false;
				}
				else {
					m_inputFiles.push_back(arg);
				}
			}
			if (!m_inputFiles.size()) {
				return false;
			}
			return true;
		}

		void PrintHelp(std::ostream& out) {
			out << "-h --help          : Print help\n"
				<< "-t --type [t]      : Set type, default PC, values: 'ps', 'xbox', 'pc'\n";
		}
	};

	void MergeGsc(std::vector<std::filesystem::path>& files, std::filesystem::path& output, opcode::Platform platform) {
		// todo wip
	}


	int gscmerge(Process& _, int argc, const char* argv[]) {
		MergeGscOpt opt{};

		if (!opt.Compute(argv, 2, argc) || opt.m_help || !opt.m_inputFiles.size()) {
			opt.PrintHelp(std::cout);
			return opt.m_help ? tool::OK : tool::BASIC_ERROR;
		}

		std::vector<std::filesystem::path> files{};

		std::filesystem::path output{ argv[2] };

		for (const auto& p : opt.m_inputFiles) {
			utils::GetFileRecurse(p, files, [](const std::filesystem::path& f) { 
				const auto str = f.string();
				return str.ends_with(".cscc") || str.ends_with(".gscc")
					|| str.ends_with(".cscobj") || str.ends_with(".gscobj")
					|| str.ends_with(".cscbin") || str.ends_with(".gscbin");
			});
		}

		if (!files.size()) {
			std::cerr << "No file to merge\n";
			return tool::BASIC_ERROR;
		}

		if (files.size() == 1) {
			if (files[0] != output) {
				std::filesystem::copy_file(files[0], output);
				std::cout << "Copied " << files[0] << " into " << output.string() << ".\n";
			}
			else {
				std::cout << "Nothing to do.\n";
			}
			return tool::OK;
		}

		std::cout << "Merging " << files.size() << " files into " << output.string() << "...\n";

		try {

			MergeGsc(files, output, opt.m_platform);
		}
		catch (std::exception e) {
			std::cerr << e.what() << "\n";
			return tool::BASIC_ERROR;
		}
		std::cout << "Files merged into " << output.string() << "\n";
		return tool::OK;

	}

}

#ifdef DEBUG

ADD_TOOL("gscmerge", "[output] [files]+", "merge GSC files", nullptr, gscmerge);

#endif

