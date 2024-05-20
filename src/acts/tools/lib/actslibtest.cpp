#include <includes.hpp>
#include <actslib/actslib.hpp>
#include <actslib/profiler.hpp>
#include <actslib/crc.hpp>
#include <actslib/hdt.hpp>
#include <actslib/data/compact.hpp>
#include <actslib/data/deltafile.hpp>
#include <actslib/data/iterator.hpp>
#include <actslib/data/kmerger.hpp>
#include <actslib/rdf/raio.hpp>
#include <mio.hpp>

namespace {
	using namespace actslib;
	
	void DumpCompressNodeFile(const std::filesystem::path& path) {
		actslib::rdf::raio::CompressComponentReaderFile reader{ path };
		actslib::ToClose tc{ reader };

		while (reader) {
			LOG_INFO("{}", *reader);
			++reader;
		}
		
	}

	void MergeNumbers() {
		size_t a1[]{ 1, 4, 7, 9 };
		size_t a2[]{ 2, 3, 5, 6, 8, 10 };

		struct Arr {
			size_t* start;
			size_t* end;
		};

		std::vector<Arr> vec{
			{ std::begin(a1), std::end(a1) },
			{ std::begin(a2), std::end(a2) },
		};

		actslib::data::iterator::AllocatedMergeAIterator<size_t, Arr> merger{
			vec,
			[](Arr& e) { return std::make_shared<actslib::data::iterator::HandleAIterator<size_t, size_t*>>(e.start, e.end); }
		};

		for (; merger; ++merger) {
			LOG_INFO("{}", *merger);
		}
	}

	int actslibtest(Process& proc, int argc, const char* argv[]) {
		const char* type;
		if (argc < 3) {
			type = "prof";
		}
		else {
			type = argv[2];
		}

		using namespace actslib::profiler;

		if (!_strcmpi(type, "prof")) {
			Profiler profiler{ "test" };

			{
				ProfiledSection test1{ profiler , "test 1" };
				Sleep(500);
			}
			{
				ProfiledSection test2{ profiler , "test 2" };
				{
					ProfiledSection test2{ profiler , "test 2-1" };
					Sleep(300);
				}
				{
					ProfiledSection test2{ profiler , "test 2-2" };
					Sleep(400);
				}
			}

			profiler.Stop();
			profiler.WriteToStr(std::cout);
		}
		else if (!_strcmpi(type, "cookie")) {

			actslib::hdt::HDTCookie cookie{ actslib::hdt::HCT_UNKNOWN, "http://atesab.fr/#actsTest" };

			cookie["test"] = "test2";
			cookie["test1"] = "test3";

			LOG_INFO("Type:   {}", actslib::hdt::FormatName(cookie.GetType()));
			LOG_INFO("Format: {}", cookie.GetFormat());

			LOG_INFO("Props:");
			for (const auto& [key, val] : cookie) {
				LOG_INFO("'{}' = {}", key, val);
			}

			LOG_INFO("===============");
			cookie.Save("testcookie.cookie");

			std::ifstream is{ "testcookie.cookie", std::ios::binary };

			if (!is) {
				LOG_ERROR("Can't open {}", "testcookie.cookie");
				return tool::BASIC_ERROR;
			}

			try {
				actslib::hdt::HDTCookie cookie{ is };
				is.close();
				LOG_INFO("Type:   {}", actslib::hdt::FormatName(cookie.GetType()));
				LOG_INFO("Format: {}", cookie.GetFormat());

				LOG_INFO("Props:");
				for (const auto& [key, val] : cookie) {
					LOG_INFO("'{}' = {}", key, val);
				}
			}
			catch (std::runtime_error& err) {
				LOG_ERROR("Can't read cookie {}", err.what());
			}
			is.close();
			std::filesystem::remove("testcookie.cookie");
		} 
		else if (!_strcmpi(type, "nt")) {

			if (argc < 4) {
				LOG_ERROR("{} {} nt file", argv[0], argv[1]);
				return tool::BAD_USAGE;
			}

			std::filesystem::path path{ argv[3] };

			auto format = actslib::rdf::GuessFormat(path);

			LOG_INFO("Format: {}", actslib::rdf::FormatName(format));

			std::ifstream is{ path };

			if (!is) {
				LOG_ERROR("Can't open {}", path.string());
				return tool::BASIC_ERROR;
			}

			std::unique_ptr<actslib::rdf::RDFParser> parserPtr = actslib::rdf::CreateParser(format, is, "http://atesab.fr/#");

			for (auto& parser = *parserPtr; parser; ++parser) {
				const auto& triple = *parser;
				LOG_INFO("triple: '{}'", triple);
			}

			is.close();
		}
		else if (!_strcmpi(type, "merge")) {
			MergeNumbers();
		}
		else if (!_strcmpi(type, "cfr")) {
			if (argc < 4) {
				LOG_ERROR("{} {} {} file", argv[0], argv[1], argv[2]);
				return tool::BAD_USAGE;
			}
			DumpCompressNodeFile(argv[3]);
		}
		else if (!_strcmpi(type, "cfrv")) {
			if (argc < 4) {
				LOG_ERROR("{} {} {} file", argv[0], argv[1], argv[2]);
				return tool::BAD_USAGE;
			}

			std::vector<std::filesystem::path> files{};

			utils::GetFileRecurse(argv[3], files);

			for (const auto& file : files) {
				try {
					actslib::rdf::raio::CompressComponentReaderFile reader{ file };
					actslib::ToClose tc{ reader };

					while (reader) {
						if (!reader->id) {
							throw std::exception("find comp with 0 id");
						}

						++reader;
					}

					//LOG_TRACE("Ok for {}", file.string());
				}
				catch (std::exception& e) {
					LOG_ERROR("Find error in {}: {}", file.string(), e.what());
				}
			}

		}
		else if (!_strcmpi(type, "raio")) {
			std::ofstream out{ "test.bin", std::ios::binary };

			if (!out) {
				LOG_ERROR("Can't open");
				return tool::BASIC_ERROR;
			}

			actslib::rdf::raio::CompressComponentWriter writer{ out };

			writer.WriteNode(1, actslib::rdf::StringComponent{ "test1" });
			writer.WriteNode(2, actslib::rdf::StringComponent{ "test     2" });
			writer.WriteNode(3, actslib::rdf::StringComponent{ "aaatest3" });
			writer.WriteEnd();

			out.close();


			std::ifstream in{ "test.bin", std::ios::binary };

			if (!in) {
				LOG_ERROR("Can't open 2");
				return tool::BASIC_ERROR;
			}

			actslib::rdf::raio::CompressComponentReader reader{ in };

			for (; reader; ++reader) {

				auto& n = *reader;
				std::cout << "'" << n.comp << "' " << n.id << "\n";
			}

			if (!reader.CheckEnd()) {
				LOG_ERROR("Bad end");
			}

			in.close();

		}
		else if (!_strcmpi(type, "km")) {
			if (argc < 4) {
				LOG_ERROR("{} {} {} file", argv[0], argv[1], argv[2]);
				return tool::BAD_USAGE;
			}

			std::filesystem::path path{ argv[3] };

			auto format = actslib::rdf::GuessFormat(path);

			LOG_INFO("Format: {}", actslib::rdf::FormatName(format));

			std::ifstream is{ path };

			if (!is) {
				LOG_ERROR("Can't open {}", path.string());
				return tool::BASIC_ERROR;
			}

			std::unique_ptr<actslib::rdf::RDFParser> parserPtr = actslib::rdf::CreateParser(format, is, "http://atesab.fr/#");

			rdf::raio::CompressComponentKMerger mergerCfg{ *parserPtr, 1024 }; // * 1024 * 1024

			data::KMerger kmerger{ "wmergertest", 20, 4, mergerCfg };

			kmerger.Init();

			auto end = kmerger.PushAndJoin();

			LOG_INFO("End file: {}", end.string());
		}
		else if (!_strcmpi(type, "mio")) {
			if (argc < 4) {
				LOG_ERROR("{} {} {} file", argv[0], argv[1], argv[2]);
				return tool::BAD_USAGE;
			}

			std::error_code err{};
			auto map = mio::make_mmap<mio::basic_mmap<mio::access_mode::read, char>, const char*>(argv[3], 0, mio::map_entire_file, err);

			std::cout.write(map.data(), map.size());
			std::cout << "\n";

			map.unmap();
		}
		else if (!_strcmpi(type, "hdt")) {
			if (argc < 4) {
				LOG_ERROR("{} {} {} file", argv[0], argv[1], argv[2]);
				return tool::BAD_USAGE;
			}

			std::ifstream is{ argv[3], std::ios::binary };

			if (!is) {
				LOG_ERROR("Can't open {}", argv[3]);
				return tool::BASIC_ERROR;
			}
			actslib::ToClose tc{ is };

			actslib::hdt::HDT hdt{};

			hdt.LoadStream(is);

			const auto& cookie = hdt.GetCookie();

			LOG_INFO("Type:   {}", actslib::hdt::FormatName(cookie.GetType()));
			LOG_INFO("Format: {}", cookie.GetFormat());

			LOG_INFO("Props:");
			for (const auto& [key, val] : cookie.GetProperties()) {
				LOG_INFO("'{}' = {}", key, val);
			}
			LOG_INFO("Header:");

			for (const auto* triple : hdt.GetHeader()->data) {
				LOG_INFO("- {}", *triple);
			}
		}
		else if (!_strcmpi(type, "cookie2")) {
			if (argc < 4) {
				LOG_ERROR("{} {} {} file", argv[0], argv[1], argv[2]);
				return tool::BAD_USAGE;
			}

			std::ifstream is{ argv[3], std::ios::binary };

			if (!is) {
				LOG_ERROR("Can't open {}", argv[3]);
				return tool::BASIC_ERROR;
			}


			actslib::ToClose tc{ is };
			actslib::hdt::HDTCookie cookie{ is };
			LOG_INFO("Type:   {}", actslib::hdt::FormatName(cookie.GetType()));
			LOG_INFO("Format: {}", cookie.GetFormat());

			LOG_INFO("Props:");
			for (const auto& [key, val] : cookie) {
				LOG_INFO("'{}' = {}", key, val);
			}
			}
		else if (!_strcmpi(type, "sequence")) {
			if (argc < 4) {
				LOG_ERROR("{} {} {} file", argv[0], argv[1], argv[2]);
				return tool::BAD_USAGE;
			}

			constexpr size_t bit = 42;
			constexpr size_t len = 789;

			std::error_code err{};
			{
				LOG_INFO("Mapping");
				auto map = mio::make_mmap<mio::basic_mmap<mio::access_mode::write, char>, const char*>(argv[3], 0, (bit * len - 1) / 8 + 1, err);

				LOG_INFO("Set");
				actslib::data::compact::Sequence seq{ map.data(), len, bit };

				for (size_t i = 0; i < len; i++) {
					seq.Set(i, 2 * i);
					LOG_INFO("{} = {}", i, seq[i]);
				}
			}

			{
				LOG_INFO("Mapping");
				auto map = mio::make_mmap<mio::basic_mmap<mio::access_mode::write, char>, const char*>(argv[3], 0, (bit * len - 1) / 8 + 1, err);

				actslib::data::compact::Sequence seq{ map.data(), len, bit };

				LOG_INFO("Read");
				for (size_t i = 0; i < len; i++) {
					LOG_INFO("{} = {}", i, seq[i]);
				}
			}
		}
		else if (!_strcmpi(type, "df")) {
			if (argc < 4) {
				LOG_ERROR("{} {} {} file", argv[0], argv[1], argv[2]);
				return tool::BAD_USAGE;
			}

			std::stringstream sh{ std::stringstream::binary | std::stringstream::out };
			sh.write(data::deltafile::DF_MAGIC, sizeof(data::deltafile::DF_MAGIC) - 1);
			io::WriteInt64(sh, 3); // count
			io::WriteInt64(sh, 0); // start
			io::WriteInt64(sh, 0xFFFFFFFFFFFFFFFull); // end
			io::WriteUInt8(sh, data::deltafile::FLAVOR_SIMPLE); // flavor
			io::WriteUInt8(sh, 0);
			io::WriteUInt8(sh, 0);
			io::WriteUInt8(sh, 0); // __pad[3]


			std::string headerStr = sh.str();
			crc::CRC8 crc{};

			crc.Update(headerStr.data(), 0, headerStr.length());

			std::ofstream out{ argv[3], std::ios::binary };

			if (!out) {
				LOG_ERROR("Can't open {}", argv[3]);
				return tool::BASIC_ERROR;
			}

			ToClose tc{ out };

			out.write(headerStr.data(), headerStr.length());
			crc.WriteCRC(out);
			LOG_INFO("write {}", headerStr.length());

			std::stringstream ss{ std::stringstream::binary | std::stringstream::out };


			auto EncodeObj = [&ss](const char* name, const char* data) {
				size_t nameLen = strlen(name);
				io::EncodeVByte(ss, nameLen);
				ss.write(name, nameLen);

				size_t dataLen = strlen(data);
				io::EncodeVByte(ss, dataLen);
				ss.write(data, dataLen);
			};

			EncodeObj("test1.nt", "<http://atesab.fr/#ate> <http://atesab.fr/#name> \"Antoine\" .");
			EncodeObj("test2.nt", "");
			EncodeObj("test3.ttl", "PREFIX as: <http://atesab.fr/#>");

			std::string str = ss.str();
			crc::CRC32 crc32{};
			crc32.Update(str.data(), 0, str.length());

			LOG_INFO("write {}", str.length());
			out.write(str.data(), str.length());
			crc32.WriteCRC(out);

			LOG_INFO("Created {}", argv[3]);
		}
		else if (!_strcmpi(type, "dfr")) {

		}
		else {
			LOG_ERROR("Unknown tool");
			return tool::BASIC_ERROR;
		}
		return tool::OK;
	}

	int actslibprofiler(Process& proc, int argc, const char* argv[]) {
		if (argc < 3) {
			return tool::BAD_USAGE;
		}

		std::ifstream is{ argv[2], std::ios::binary };

		if (!is) {
			LOG_ERROR("Can't open {}", argv[2]);
			return tool::BASIC_ERROR;
		}

		try {
			actslib::profiler::Profiler prof{ is };
			is.close();

			prof.WriteToStr(std::cout);
		}
		catch (std::runtime_error& err) {
			LOG_ERROR("Can't read profiler {}", err.what());
			is.close();
			return tool::BASIC_ERROR;
		}
		is.close();

		return tool::OK;
	}

}

#ifndef CI_BUILD
ADD_TOOL("actslibtest", "lib", "", "Acts lib test", nullptr, actslibtest);
#endif

ADD_TOOL("actslibprofiler", "lib", " [profile file]", "Read profiler", nullptr, actslibprofiler);