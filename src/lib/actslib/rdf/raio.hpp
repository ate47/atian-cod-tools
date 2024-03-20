#pragma once
#include "../data/iterator.hpp"
#include "../io.hpp"
#include "../crc.hpp"
#include "rdf.hpp"

namespace actslib::rdf::raio {
	using namespace actslib;

	struct IdComponent {
		StreamAppendStringComponent comp{};
		size_t id;
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
		bool remaining{ 1 };

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

		operator bool() override {
			if (nextWait) {
				return true;
			}

			if (!remaining) {
				return false;
			}

			next.id = io::DecodeVByte(is);

			if (!next.id) {
				return false; // end of stream
			}

			size_t delta = io::DecodeVByte(is);
			next.comp.Append(is, delta);
			nextWait = true;

			return true;
		}

		bool CheckEnd() {
			return io::DecodeVByte(is) == 4
				|| io::DecodeVByte(is) == 5
				|| io::DecodeVByte(is) == 6;
		}
	};



}