#pragma once

namespace utils {
	template<typename Type, Type invalidValue, size_t maxValue = (size_t)invalidValue>
	class EnumList {
		Type(*Lookup)(const char* type);
		uint8_t buffer[((maxValue - 1) >> 3) + 1]{};
		bool empty{};
	public:
		EnumList(Type(*lookup)(const char* type)) : Lookup(lookup) {}

		void Clear() {
			std::memset(buffer, 0, sizeof(buffer));
			empty = true;
		}

		bool operator[](Type value) const {
			return buffer[value >> 3] & (1 << (value & 7));
		}

		void Set(Type value, bool val) {
			if (val) {
				empty = false;
				buffer[value >> 3] |= 1 << (value & 7);
			}
			else {
				buffer[value >> 3] &= ~(1 << (value & 7));
			}
		}

		constexpr bool Empty() const {
			return empty;
		}

		void Add(const char* name) {
			Type t{ Lookup(name) };

			if (t == invalidValue) {
				throw std::runtime_error(std::format("Invalid enum name '{}'", name));
			}

			Set(t, true);
		}

		void LoadConfig(const char* cfg) {
			if (!cfg || !*cfg) return;

			std::string c{ cfg };

			size_t idx{};

			while (idx < c.size()) {
				while (c[idx] == ',') {
					idx++;
					if (idx == c.size()) return; // end
				}

				size_t next{ c.find(',', idx) };

				if (next == std::string::npos) {
					next = c.size() + 1;
				}
				else {
					c[next] = 0;
				}

				Add(&c[idx]);

				idx = next + 1;
			}
		}
	};

}