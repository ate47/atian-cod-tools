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
	class KMergerTest : public actslib::data::KMergerConfig {

		actslib::rdf::RDFParser& parser;
		size_t tripleId{ 1 };
		std::string buffers[rdf::RDF_TRIPLE_COUNT]{};
		size_t bufferOffsets[rdf::RDF_TRIPLE_COUNT]{};
		std::vector<size_t> strings[rdf::RDF_TRIPLE_COUNT]{};
	public:

		KMergerTest(actslib::rdf::RDFParser& parser, size_t bufferSize) : parser(parser) {
			for (auto& buff : buffers) {
				buff.resize(bufferSize);
			}
		}

		void CreateDefaultChunk(const std::filesystem::path& chunkLocation) override {
			std::ofstream out{ chunkLocation, std::ios::binary };

			if (!out) throw std::runtime_error("Can't create default chunk");

			actslib::rdf::raio::CompressComponentWriter w{ out };

			w.WriteEnd();

			out.close();
		}

		inline bool WriteChunkData(const rdf::Triple& triple, bool force, size_t index) {
			auto& stringss = strings[rdf::RDF_SUBJECT];
			auto& bufferss = buffers[rdf::RDF_SUBJECT];

			auto& stringsp = strings[rdf::RDF_PREDICATE];
			auto& buffersp = buffers[rdf::RDF_PREDICATE];

			auto& stringso = strings[rdf::RDF_OBJECT];
			auto& bufferso = buffers[rdf::RDF_OBJECT];

			// check if we can add the components (reallocate if we force them)
			if (bufferOffsets[rdf::RDF_SUBJECT] + triple.subject->length + sizeof(size_t) + 1 > bufferss.size()) {
				if (!force) {
					return false;
				}
				bufferss.resize((size_t)((triple.subject->length + sizeof(size_t) + 1) * 1.5));
			}
			if (bufferOffsets[rdf::RDF_PREDICATE] + triple.predicate->length + sizeof(size_t) + 1 > bufferss.size()) {
				if (!force) {
					return false;
				}
				buffersp.resize((size_t)((triple.predicate->length + sizeof(size_t) + 1) * 1.5));
			}
			if (bufferOffsets[rdf::RDF_OBJECT] + triple.object->length + sizeof(size_t) + 1 > bufferso.size()) {
				if (!force) {
					return false;
				}
				bufferso.resize((size_t)((triple.object->length + sizeof(size_t) + 1) * 1.5));
			}

			size_t offs = bufferOffsets[rdf::RDF_SUBJECT];
			stringss.push_back(offs);
			bufferOffsets[rdf::RDF_SUBJECT] += triple.subject->length + sizeof(size_t) + 1;

			size_t offp = bufferOffsets[rdf::RDF_PREDICATE];
			stringsp.push_back(offp);
			bufferOffsets[rdf::RDF_PREDICATE] += triple.predicate->length + sizeof(size_t) + 1;

			size_t offo = bufferOffsets[rdf::RDF_OBJECT];
			stringso.push_back(offo);
			bufferOffsets[rdf::RDF_OBJECT] += triple.object->length + sizeof(size_t) + 1;

			*reinterpret_cast<size_t*>(bufferss.data() + offs) = index;
			*reinterpret_cast<size_t*>(buffersp.data() + offp) = index;
			*reinterpret_cast<size_t*>(bufferso.data() + offo) = index;

			memcpy(bufferss.data() + offs + sizeof(size_t), triple.subject->buffer + triple.subject->offset, triple.subject->length);
			memcpy(buffersp.data() + offp + sizeof(size_t), triple.predicate->buffer + triple.predicate->offset, triple.predicate->length);
			memcpy(bufferso.data() + offo + sizeof(size_t), triple.object->buffer + triple.object->offset, triple.object->length);

			// end str
			bufferss.data()[bufferOffsets[rdf::RDF_SUBJECT]] = 0;
			buffersp.data()[bufferOffsets[rdf::RDF_PREDICATE]] = 0;
			bufferso.data()[bufferOffsets[rdf::RDF_OBJECT]] = 0;

			return true;
		}

		inline void SortBuffer(rdf::RDFComponentType comp) {
			auto& str = strings[comp];
			const auto& buff = buffers[comp];

			std::sort(str.begin(), str.end(), [&buff](size_t off1, size_t off2) -> bool {
				const char* s1 = &buff[off1 + sizeof(size_t)];
				const char* s2 = &buff[off2 + sizeof(size_t)];
				return strcmp(s1, s2) < 0;
			});

		}

		bool CreateChunk(const std::filesystem::path& chunkLocation) override {
			if (!parser) {
				return false;
			}
			WriteChunkData(*parser, true, tripleId);
			
			++parser;
			tripleId++;

			while (parser) {
				if (!WriteChunkData(*parser, true, tripleId)) {
					break;
				}
				++parser;
				tripleId++;
			}

			// sort strings
			SortBuffer(rdf::RDF_SUBJECT);
			SortBuffer(rdf::RDF_PREDICATE);
			SortBuffer(rdf::RDF_OBJECT);

			// write strings


			return true;
		}

		void MergeChunks(const std::vector<actslib::data::KMergerChunk>& chunks, const std::filesystem::path& chunkLocation) override {

		}
	};

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