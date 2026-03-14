#include "../actslib.hpp"
#include "parser.hpp"

namespace actslib::rdf {
	RDFFormat GuessFormat(const std::filesystem::path& path) {
		std::string str = path.string();

		if (str.ends_with(".nt")) {
			return RDFFormat::RF_NTRIPLES;
		}
		return RDFFormat::RF_COUNT;
	}
	const char* FormatName(RDFFormat format) {
		static const char* names[]{
			"ntriples",
		};

		if (format >= 0 && format < RF_COUNT) {
			return names[format];
		}
		return "unknown";
	}

	std::unique_ptr<RDFParser> CreateParser(RDFFormat format, std::istream& is, const std::string& baseUri) {
		switch (format) {
		case RF_NTRIPLES:
			return std::make_unique<RDFParserNTriple>(is);
		default:
			throw std::invalid_argument("Unknown format");
		}
	}
}
