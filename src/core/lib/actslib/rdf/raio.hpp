#pragma once
#include "../data/kmerger.hpp"
#include "../data/iterator.hpp"
#include "../io.hpp"
#include "../crc.hpp"
#include "parser.hpp"
#include "rdf.hpp"

namespace actslib::rdf::raio {
	using namespace actslib;

	struct IdComponent {
		StreamAppendStringComponent comp{};
		size_t id;

		friend std::ostream& operator<<(std::ostream& out, const IdComponent& comp) {
			out << "(" << comp.comp << ", " << comp.id << ")";
			return out;
		}
	};

	class CompressComponentWriter {
		std::ostream& os;
		StringComponent last{};

	public:
		CompressComponentWriter(std::ostream& os) : os(os) {
		}

		void WriteNode(size_t id, const Component& component) {
			io::EncodeVByte(os, id);
			size_t delta = last.GetDeltaLen(component);

			io::EncodeVByte(os, delta);

			if (component.length != delta) {
				os.write(component.buffer + component.offset + delta, component.length - delta);
			}
			os.put(0);
			last.ReplaceString(component);
		}

		void WriteEnd() {
			io::EncodeVByte(os, 0);
			io::EncodeVByte(os, 4);
			io::EncodeVByte(os, 5);
			io::EncodeVByte(os, 6);
		}
	};

	class CompressComponentReader : public data::iterator::AIterator<const IdComponent> {
		std::istream& is;
		IdComponent next{};
		bool nextWait{};
		bool remaining{ true };

	public:
		CompressComponentReader(std::istream& is) : is(is) {
		}

		CompressComponentReader& operator++() override {
			nextWait = false;
			return *this;
		}
		
		const IdComponent& operator*() override {
			if (!*this) {
				throw std::runtime_error("Empty iterator");
			}
			return next;
		}

		bool CheckEnd() {
			return io::DecodeVByte(is) == 4
				&& io::DecodeVByte(is) == 5
				&& io::DecodeVByte(is) == 6;
		}

		operator bool() override {
			if (nextWait) {
				return true;
			}

			if (!remaining) {
				return false;
			}

#ifdef DEBUG
			if (!is) throw std::runtime_error("Invalid stream after compress read");
#endif

			next.id = io::DecodeVByte(is);

			if (!next.id) {
				remaining = false;
				if (!CheckEnd()) {
					if (!is) {
						throw std::runtime_error("Invalid when checking end");
					}
					throw std::runtime_error("Can't find compress nodes file end of stream");
				}
				return false; // end of stream
			}

			size_t delta = io::DecodeVByte(is);
			next.comp.Append(is, delta);
			nextWait = true;

			return true;
		}
	};

	class CompressComponentReaderFile : public CompressComponentReader {
		std::ifstream stream;

	public:
		CompressComponentReaderFile(const std::filesystem::path& path) : stream(path, std::ios::binary), CompressComponentReader(stream) {
			if (!stream) {
				const auto str = path.string();
				throw std::runtime_error(actslib::va("Can't open compress reader file path '%s'", str.c_str()));
			}
		}

		void close() {
			stream.close();
		}
	};

	class CompressComponentKMerger : public actslib::data::KMergerConfig {
		actslib::rdf::RDFParser& parser;
		size_t tripleId{ 1 };
		std::string buffers[rdf::RDF_TRIPLE_COUNT]{};
		size_t bufferOffsets[rdf::RDF_TRIPLE_COUNT]{};
		std::vector<size_t> strings[rdf::RDF_TRIPLE_COUNT]{};
	public:
		CompressComponentKMerger(actslib::rdf::RDFParser& parser, size_t bufferSize) : parser(parser) {
			for (auto& buff : buffers) {
				buff.resize(bufferSize);
			}
		}

		inline void WriteEmptyChunk(const std::filesystem::path& chunkLocation, rdf::RDFComponentType type) {
			std::filesystem::path s = chunkLocation / rdf::GetRDFComponentTypeName(type);

			std::ofstream out{ s, std::ios::binary };

			if (!out) throw std::runtime_error(actslib::va("Can't create default chunk %s", rdf::GetRDFComponentTypeName(type)));

			ToClose<std::ofstream> outClose{ out };

			actslib::rdf::raio::CompressComponentWriter w{ out };

			w.WriteEnd();
		}

		void CreateDefaultChunk(const std::filesystem::path& chunkLocation) override {
			std::filesystem::create_directories(chunkLocation);

			WriteEmptyChunk(chunkLocation, rdf::RDF_SUBJECT);
			WriteEmptyChunk(chunkLocation, rdf::RDF_PREDICATE);
			WriteEmptyChunk(chunkLocation, rdf::RDF_OBJECT);
		}

		inline bool WriteChunkData(const rdf::Triple& triple, bool force, size_t index) {
			auto& bufferss = buffers[rdf::RDF_SUBJECT];
			auto& buffersp = buffers[rdf::RDF_PREDICATE];
			auto& bufferso = buffers[rdf::RDF_OBJECT];

			// check if we can add the components (reallocate if we force them)
			if (bufferOffsets[rdf::RDF_SUBJECT] + triple.subject->length + sizeof(size_t) + 1 > bufferss.size()) {
				if (!force) {
					return false;
				}
				bufferss.resize(bufferOffsets[rdf::RDF_SUBJECT] + (size_t)((triple.subject->length + sizeof(size_t) + 1) * 1.5));
			}
			if (bufferOffsets[rdf::RDF_PREDICATE] + triple.predicate->length + sizeof(size_t) + 1 > buffersp.size()) {
				if (!force) {
					return false;
				}
				buffersp.resize(bufferOffsets[rdf::RDF_PREDICATE] + (size_t)((triple.predicate->length + sizeof(size_t) + 1) * 1.5));
			}
			if (bufferOffsets[rdf::RDF_OBJECT] + triple.object->length + sizeof(size_t) + 1 > bufferso.size()) {
				if (!force) {
					return false;
				}
				bufferso.resize(bufferOffsets[rdf::RDF_OBJECT] + (size_t)((triple.object->length + sizeof(size_t) + 1) * 1.5));
			}

			size_t offs = bufferOffsets[rdf::RDF_SUBJECT];
			strings[rdf::RDF_SUBJECT].push_back(offs);
			bufferOffsets[rdf::RDF_SUBJECT] += triple.subject->length + sizeof(size_t) + 1;

			size_t offp = bufferOffsets[rdf::RDF_PREDICATE];
			strings[rdf::RDF_PREDICATE].push_back(offp);
			bufferOffsets[rdf::RDF_PREDICATE] += triple.predicate->length + sizeof(size_t) + 1;

			size_t offo = bufferOffsets[rdf::RDF_OBJECT];
			strings[rdf::RDF_OBJECT].push_back(offo);
			bufferOffsets[rdf::RDF_OBJECT] += triple.object->length + sizeof(size_t) + 1;

			*reinterpret_cast<size_t*>(bufferss.data() + offs) = index;
			*reinterpret_cast<size_t*>(buffersp.data() + offp) = index;
			*reinterpret_cast<size_t*>(bufferso.data() + offo) = index;

			assert((bufferss.size() > offs + sizeof(size_t) && bufferss.size() - (offs + sizeof(size_t)) >= triple.subject->length) && "bad buffer s");
			memcpy(bufferss.data() + offs + sizeof(size_t), triple.subject->buffer + triple.subject->offset, triple.subject->length);
			assert((buffersp.size() > offp + sizeof(size_t) && buffersp.size() - (offp + sizeof(size_t)) >= triple.predicate->length) && "bad buffer p");
			memcpy(buffersp.data() + offp + sizeof(size_t), triple.predicate->buffer + triple.predicate->offset, triple.predicate->length);
			assert((bufferso.size() > offo + sizeof(size_t) && bufferso.size() - (offo + sizeof(size_t)) >= triple.object->length) && "bad buffer o");
			memcpy(bufferso.data() + offo + sizeof(size_t), triple.object->buffer + triple.object->offset, triple.object->length);

			// end str
			bufferss.data()[bufferOffsets[rdf::RDF_SUBJECT] - 1] = 0;
			buffersp.data()[bufferOffsets[rdf::RDF_PREDICATE] - 1] = 0;
			bufferso.data()[bufferOffsets[rdf::RDF_OBJECT] - 1] = 0;

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

		inline void WriteChunkFile(const std::filesystem::path& chunkLocation, rdf::RDFComponentType type) {
			try {
				std::filesystem::path su = chunkLocation / rdf::GetRDFComponentTypeName(type);

				std::ofstream out{ su, std::ios::binary };

				if (!out) {
					auto sus = su.string();
					throw std::runtime_error(actslib::va("Can't create chunk file of %s into '%s'", rdf::GetRDFComponentTypeName(type), sus.c_str()));
				}

				ToClose<std::ofstream> outClose{ out };

				actslib::rdf::raio::CompressComponentWriter w{ out };

				const auto& str = strings[type];
				const auto& buff = buffers[type];

				rdf::Component cmp{};
				for (size_t offset : str) {
					cmp.buffer = &buff[offset + sizeof(size_t)];
					cmp.length = strlen(cmp.buffer);
					cmp.offset = 0;

					assert(*reinterpret_cast<const size_t*>(buff.data() + offset) != 0 && "write empty node");
					w.WriteNode(*reinterpret_cast<const size_t*>(buff.data() + offset), cmp);
				}

				w.WriteEnd();
			}
			catch (std::exception& e) {
				throw std::runtime_error(actslib::va("can't create chunk %s : %s", rdf::GetRDFComponentTypeName(type), e.what()));
			}
		};

		bool CreateChunk(const std::filesystem::path& chunkLocation) override {
			if (!parser) {
				return false;
			}
			ALOG_TRACE("Create chunk into {}", chunkLocation.string());
			// clear buffers
			for (size_t i = 0; i < rdf::RDF_TRIPLE_COUNT; i++) {
				bufferOffsets[i] = 0;
				strings[i].clear();
			}

			WriteChunkData(*parser, true, tripleId);

			++parser;
			tripleId++;

			while (parser) {
				if (!WriteChunkData(*parser, false, tripleId)) {
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
			std::filesystem::create_directories(chunkLocation);

			WriteChunkFile(chunkLocation, rdf::RDF_SUBJECT);
			WriteChunkFile(chunkLocation, rdf::RDF_PREDICATE);
			WriteChunkFile(chunkLocation, rdf::RDF_OBJECT);

			ALOG_TRACE("Done create chunk into {}", chunkLocation.string());

			return true;
		}

		void MergeChunks(const std::vector<actslib::data::KMergerChunk>& chunks, const std::filesystem::path& chunkLocation) override {
			std::vector<std::shared_ptr<rdf::raio::CompressComponentReaderFile>> readers{};

			if (ALOG_IFTRACE) {
				ALOG_TRACE("Merging into {}", chunkLocation.string());
				for (const auto& chunk : chunks) {
					ALOG_TRACE("- Chunk {} ({})", chunk.file.string(), chunk.layer);
				}
			}

			readers.reserve(chunks.size());
			for (size_t i = 0; i < rdf::RDF_TRIPLE_COUNT; i++) {
				rdf::RDFComponentType type = (rdf::RDFComponentType)i;
				ToCloseFunc tcf{ [&readers, &chunks, type] {
					for (auto& r : readers) {
						r->close();
					}
					readers.clear();

					for (const auto& chunk : chunks) {
						std::filesystem::remove(chunk.file / rdf::GetRDFComponentTypeName(type));
					}
				} };
				const char* typeName = rdf::GetRDFComponentTypeName(type);
				for (const auto& chunk : chunks) {
					readers.emplace_back(std::make_shared<rdf::raio::CompressComponentReaderFile>(chunk.file / typeName));
				}

				class IdCompComparator {
				public:
					static inline bool Compare(const rdf::raio::IdComponent& a, const rdf::raio::IdComponent& b) {
						return a.comp < b.comp;
					}
				};

				actslib::data::iterator::AllocatedMergeAIterator<const rdf::raio::IdComponent, std::shared_ptr<rdf::raio::CompressComponentReaderFile>, IdCompComparator> merger{
					readers, [](auto& e) { return e; }
				};


				std::filesystem::create_directories(chunkLocation);

				std::filesystem::path su = chunkLocation / typeName;

				std::ofstream out{ su, std::ios::binary };


				if (!out) {
					auto sus = su.string();
					throw std::runtime_error(actslib::va("Can't create merge chunk file of %s into '%s'", typeName, sus.c_str()));
				}

				ToClose<std::ofstream> outClose{ out };

				actslib::rdf::raio::CompressComponentWriter w{ out };

				while (merger) {
					const auto& c = *merger;
					w.WriteNode(c.id, c.comp);

					++merger;
				}

				w.WriteEnd();
			}
			for (const auto& chunk : chunks) {
				std::filesystem::remove(chunk.file);
			}
			ALOG_TRACE("Done merging {}", chunkLocation.string());
		}
	};

}
template<>
struct std::formatter<actslib::rdf::raio::IdComponent, char> : actslib::BasicFormatter<actslib::rdf::raio::IdComponent> {};