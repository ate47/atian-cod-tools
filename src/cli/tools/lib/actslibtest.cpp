#include <includes.hpp>
#include <actslib/actslib.hpp>
#include <actslib/profiler.hpp>
#include <actslib/crc.hpp>
#include <actslib/hdt.hpp>
#include <actslib/data/kmerger.hpp>
#include <actslib/data/iterator.hpp>
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
				LOG_ERROR("{} {} km file", argv[0], argv[1]);
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

	int actslibhdtcookie(Process& proc, int argc, const char* argv[]) {
		if (argc < 3) {
			return tool::BAD_USAGE;
		}

		std::ifstream is{ argv[2], std::ios::binary };

		if (!is) {
			LOG_ERROR("Can't open {}", argv[2]);
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
			LOG_ERROR("Can't read profiler {}", err.what());
			is.close();
			return tool::BASIC_ERROR;
		}
		is.close();

		return tool::OK;
	}

}

#ifndef CI_BUILD
ADD_TOOL("actslibtest", "", "Acts lib test", nullptr, actslibtest);

ADD_TOOL("actslibhdt", " [hdt]", "Read hdt cookie", nullptr, actslibhdtcookie);
#endif

ADD_TOOL("actslibprofiler", " [profile file]", "Read profiler", nullptr, actslibprofiler);