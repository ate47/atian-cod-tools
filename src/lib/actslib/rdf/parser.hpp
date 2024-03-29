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

	class RDFParserNTriple : public RDFParser {
		std::istream& is;
		Component s{}, p{}, o{};
		Triple next{ &s, &p, &o };
		bool loaded{};
		std::string lineBuffer{};
	public:
		RDFParserNTriple(std::istream& is) : is(is) {}

		const Triple& operator*() override {
			if (!*this) {
				throw std::runtime_error("Empty iterator");
			}
			return next;
		}

		RDFParser& operator++() override {
			loaded = false;
			return *this;
		}

		static size_t FindNextSpace(const char* str, size_t start, size_t end) {
			for (size_t i = start; i < end; i++) {
				if (isspace(str[i])) {
					return i;
				}
			}
			return std::string::npos;
		}

		virtual operator bool() {
			if (loaded) {
				return true;
			}
			// we have to compute the next

			while (is && std::getline(is, lineBuffer)) {
				size_t start{};
				while (start < lineBuffer.length() && isspace(lineBuffer[start])) {
					start++;
				}

				if (start == lineBuffer.length() || lineBuffer[start] == '#') {
					continue; // ignore comments/empty lines
				}

				size_t end{ lineBuffer.length() - 1 };

				while (isspace(lineBuffer[end])) {
					end--;
					continue;
				}

				if (lineBuffer[end] != '.') {
					continue; // invalid line, no end '.'
				}

				while (start < end && isspace(lineBuffer[--end])) {
					continue;
				}

				if (start == end) {
					continue; // invalid line only one '.'
				}

				// subject load
				{
					size_t b1 = start;
					size_t b2 = FindNextSpace(lineBuffer.data(), b1, end);

					if (b2 == std::string::npos) {
						continue; // not enought component
					}

					// match IRI
					if (lineBuffer[b1] == '<') {
						b1++;
						if (lineBuffer[b2 - 1] == '>') {
							b2--;
						}
					}

					s.buffer = lineBuffer.data();
					s.offset = b1;
					s.length = b2 - b1;

					start = b2 + 1;
				}

				while (start < end && isspace(lineBuffer[start])) {
					start++;
				}

				if (start == end) {
					continue; // not enought component
				}

				// predicate load
				{
					size_t b1 = start;
					size_t b2 = FindNextSpace(lineBuffer.data(), b1, end);

					if (b2 == std::string::npos) {
						continue; // not enought component
					}

					// match IRI
					if (lineBuffer[b1] == '<') {
						b1++;
						if (lineBuffer[b2 - 1] == '>') {
							b2--;
						}
					}

					p.buffer = lineBuffer.data();
					p.offset = b1;
					p.length = b2 - b1;

					start = b2 + 1;
				}

				while (start < end && isspace(lineBuffer[start])) {
					start++;
				}

				if (start == end) {
					continue; // not enought component
				}

				// match IRI
				if (lineBuffer[start] == '<') {
					start++;
					if (lineBuffer[end] == '>') {
						end--;
					}
				}

				o.buffer = lineBuffer.data();
				o.offset = start;
				o.length = end - start + 1;

				loaded = true;

				return true;
			}
			return false;
		}
	};

	RDFFormat GuessFormat(const std::filesystem::path& path);
	const char* FormatName(RDFFormat format);
	std::unique_ptr<RDFParser> CreateParser(RDFFormat format, std::istream& is, const std::string& baseUri);
}
