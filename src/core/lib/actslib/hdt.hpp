#pragma once
#include "actslib.hpp"
#include "crc.hpp"
#include "rdf/parser.hpp"

namespace actslib::hdt {
	constexpr const char HDT_COOKIE_MAGIC[] = "$HDT";

	enum HDTCookieType : uint8_t {
		HCT_UNKNOWN = 0,
		HCT_GLOBAL,
		HCT_HEADER,
		HCT_DICTIONARY,
		HCT_TRIPLES,
		HCT_INDEX,
		HCT_QEPCORE_MERGE,
		HCT_COUNT
	};

	const char* FormatName(HDTCookieType type);

	class HDTCookie {
		HDTCookieType type;
		std::string format;
		std::unordered_map<std::string, std::string> props{};
	public:
		using iterator = decltype(props)::iterator;
		using const_iterator = decltype(props)::const_iterator;

		HDTCookie(HDTCookieType type, std::string format) : type(type), format(format) {}

		void LoadCookie(std::istream& is) {
			actslib::crc::CRC16 crc{};
			char buffer[sizeof(HDT_COOKIE_MAGIC) - 1];
			is.read(buffer, sizeof(HDT_COOKIE_MAGIC) - 1);

			if (memcmp(buffer, HDT_COOKIE_MAGIC, sizeof(HDT_COOKIE_MAGIC) - 1)) {
				throw std::runtime_error("Invalid HDT cookie magic");
			}
			crc.Update(buffer, 0, sizeof(HDT_COOKIE_MAGIC) - 1);

			type = (HDTCookieType)is.get();
			crc.Update((const char)type);

			std::getline(is, format, '\0');
			crc.Update(format.data(), 0, format.size() + 1);

			std::string propsVal;
			std::getline(is, propsVal, '\0');

			crc.Update(propsVal.data(), 0, propsVal.size() + 1);

			size_t off{};
			while (off < propsVal.size()) {
				size_t idx = propsVal.find(';', off);

				if (idx == std::string::npos) {
					idx = propsVal.size();
				}

				size_t idxeq = propsVal.find('=', off);

				if (idxeq != std::string::npos && idxeq < idx) {
					// split
					props[propsVal.substr(off, idxeq - off)] = propsVal.substr(idxeq + 1, idx - idxeq - 1);
				}

				off = idx + 1;
			}

			if (!crc.CheckCRC(is)) {
				throw std::runtime_error("Invalid HDT cookie CRC");
			}
		}

		HDTCookie(std::istream& is) {
			LoadCookie(is);
		}

		void Save(std::ostream& os) const {
			actslib::crc::CRC16 crc{};
			os.write(HDT_COOKIE_MAGIC, sizeof(HDT_COOKIE_MAGIC) - 1);
			crc.Update(HDT_COOKIE_MAGIC, 0, sizeof(HDT_COOKIE_MAGIC) - 1);

			os.put((char)type);
			crc.Update(type);

			os.write(format.data(), format.size() + 1);
			crc.Update(format.data(), 0, format.size() + 1);

			for (const auto& [key, val] : props) {

				os.write(key.data(), key.size());
				crc.Update(key.data(), 0, key.size());

				os.put('=');
				crc.Update('=');

				os.write(val.data(), val.size());
				crc.Update(val.data(), 0, val.size());

				os.put(';');
				crc.Update(';');
			}

			os.put(0);
			crc.Update(0);

			crc.WriteCRC(os);
		}

		void Save(std::filesystem::path path) const {
			std::ofstream os{ path, std::ios::binary };

			if (!os) {
				throw std::runtime_error("Can't open file");
			}

			Save(os);

			os.close();
		}

		constexpr HDTCookieType GetType() const {
			return type;
		}

		constexpr const std::string& GetFormat() const {
			return format;
		}

		constexpr const std::unordered_map<std::string, std::string>& GetProperties() const {
			return props;
		}

		std::string& operator[](const std::string& key) {
			return props[key];
		}

		std::string& operator[](const char* key) {
			return props[key];
		}

		const_iterator find(const std::string& key) const {
			return props.find(key);
		}

		const_iterator find(const char* key) const {
			return props.find(key);
		}

		iterator begin() {
			return props.begin();
		}

		iterator end() {
			return props.end();
		}

		const_iterator cbegin() const {
			return props.cbegin();
		}

		const_iterator cend() const {
			return props.cend();
		}

		int64_t GetInteger(const std::string& key, int64_t defaultVal = 0) {
			const_iterator it = find(key);

			if (it == cend()) {
				return defaultVal;
			}

			try {
				return std::strtoll(it->second.c_str(), nullptr, 10);
			}
			catch ([[maybe_unused]]std::invalid_argument& e) {
				return defaultVal;
			}
		}
	};

	class Header {
	public:
		std::vector<rdf::TripleAlloc*> data{};
		using iterator = decltype(data)::iterator;
		using const_iterator = decltype(data)::const_iterator;
		Header() {}

		virtual ~Header() {
			for (auto* t : data) {
				delete t;
			}
		}

		iterator begin() {
			return data.begin();
		}

		iterator end() {
			return data.end();
		}

		const_iterator cbegin() const {
			return data.cbegin();
		}

		const_iterator cend() const {
			return data.cend();
		}

	};

	class PlainHeader : public Header {
		HDTCookie cookie;
	public:
		PlainHeader(std::istream& is, HDTCookie& cookie) : cookie(cookie) {
			if (cookie.GetType() != HCT_HEADER) {
				throw std::invalid_argument("Cookie not valid for plain header");
			}

			int64_t length = cookie.GetInteger("length", -1);

			if (length < 0) {
				throw std::invalid_argument("Cookie doesn't contain a valid length");
			}

			std::string raw{};
			raw.resize(length);
			is.read(raw.data(), length);

			std::stringstream ss{ raw, std::ios::in };

			rdf::RDFParserNTriple parser{ ss };

			while (parser) {
				const auto& triple = *parser;

				data.emplace_back(new rdf::TripleAlloc(triple));

				++parser;
			}

		}

	};

	Header* LoadHeader(std::istream& is);

	constexpr const char HDTV1[] = "<http://purl.org/HDT/hdt#HDTv1>";

	class HDT {
		Header* header{};
		HDTCookie cookie{ HCT_GLOBAL, HDTV1 };

	public:
		HDT() {}
		~HDT() {
			if (header) delete header;
		}


		void LoadStream(std::istream& is) {
			if (header) {
				delete header;
			}
			header = nullptr;

			cookie.LoadCookie(is);

			if (cookie.GetType() != HCT_GLOBAL || cookie.GetFormat() != HDTV1) {
				throw std::runtime_error(actslib::va("Global HDT v1 cookie excepted, find %s/%s", FormatName(cookie.GetType()), cookie.GetFormat().c_str()));
			}

			header = LoadHeader(is);
		}

		const HDTCookie& GetCookie() const {
			return cookie;
		}

		const Header* GetHeader() const {
			return header;
		}
	};
}