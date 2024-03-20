#pragma once
#include "rdf.hpp"

namespace actslib::rdf {
	enum RDFFormat {
		RF_NTRIPLES = 0,
		RF_COUNT
	};

	class RDFParser {
	public:
		virtual operator bool() = 0;
		bool operator!() {
			return !(bool)*this;
		}
		virtual const Triple& operator*() = 0;
		virtual RDFParser& operator++() = 0;

		const Triple* operator->() {
			return &**this;
		}
	};

	RDFFormat GuessFormat(const std::filesystem::path& path);
	const char* FormatName(RDFFormat format);
	std::unique_ptr<RDFParser> CreateParser(RDFFormat format, std::istream& is, const std::string& baseUri);
}
