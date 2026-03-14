#pragma once
#include "io.hpp"
#include <chrono>
#include <ctime>
#include <iostream>

/*
 * Profiler compatible with the qEndpoint database's profile files.
 * https://github.com/the-qa-company/qEndpoint/blob/master/qendpoint-core/src/main/java/com/the_qa_company/qendpoint/core/util/Profiler.java
 */
namespace actslib::profiler {
	// File magic
	constexpr char PROFILER_MAGIC[] = "HDTPROFILE";

	typedef long long ProfTs;
	
	/*
	 * Get current timestamp in millis
	 * @return timestamp in millis
	 */
	constexpr ProfTs GetTimestamp() {
		return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	}

	// Profiling section
	class ProfilerSection {
		ProfTs startTime;
		ProfTs endTime;
		std::vector<ProfilerSection> sections{};
		ProfilerSection* current{};

	public:
		// section's name
		std::string name;
		using iterator = decltype(sections)::iterator;
		using const_iterator = decltype(sections)::const_iterator;

		/*
		 * Empty section
		 */
		ProfilerSection() : name(""), startTime(), endTime() {
		}

		/*
		 * Section with name
		 * @param name name
		 */
		ProfilerSection(std::string name) : name(name) {
			endTime = startTime = GetTimestamp();
		}

		/*
		 * Read section from stream
		 * @param is stream
		 */
		ProfilerSection(std::istream& is) {
			startTime = actslib::io::ReadInt64(is);
			endTime = actslib::io::ReadInt64(is);
			size_t nameLen = (size_t)actslib::io::ReadUInt64(is);

			name.resize(nameLen);
			is.read(name.data(), nameLen);
			size_t subLen = (size_t)actslib::io::ReadUInt64(is);

			if (is.bad()) {
				throw std::runtime_error("Error when reading profiler, bad bit set!");
			}

			for (size_t i = 0; i < subLen; i++) {
				sections.emplace_back(ProfilerSection{ is });
			}
		}

		/*
		 * Write section to stream
		 * @param os stream
		 */
		void Write(std::ostream& os) const {
			actslib::io::WriteInt64(os, startTime);
			actslib::io::WriteInt64(os, endTime);
			actslib::io::WriteInt64(os, name.size());
			os.write(name.data(), name.size());

			actslib::io::WriteInt64(os, sections.size());
			for (const auto& s : sections) {
				s.Write(os);
			}
		}

		static void WritePrettyMillisTime(std::ostream& os, ProfTs millis) {
			ProfTs ms{ millis % 1000 };
			ProfTs sc{ (millis / 1000) % 60 };
			ProfTs min{ (millis / 60000) % 60 };
			ProfTs hr{ (millis / 3600000) % 24 };
			ProfTs days{ (millis / (3600000 * 24)) };
			
			os << std::dec;
			bool any{};
			if (days) {
				os << days << "d";
				any = true;
			}
			if (hr) {
				if (any) os << " ";
				os << hr << "h";
				any = true;
			}
			if (min) {
				if (any) os << " ";
				os << min << "min";
				any = true;
			}
			if (sc) {
				if (any) os << " ";
				os << sc << "s";
				any = true;
			}
			if (ms) {
				if (any) os << " ";
				os << ms << "ms";
			}
		}
	private:
		void WriteToStr(std::ostream& out, size_t depth) const {

			for (size_t i = 0; i < depth; i++) {
				out << "  ";
			}
			if (depth) {
				out << "+-";
			}
			out << name << " elapsed=" << std::dec << GetMillis() << "ms (";
			WritePrettyMillisTime(out, GetMillis());
			out << ")\n";

			for (const auto& sub : sections) {
				sub.WriteToStr(out, depth + 1);
			}
		}
	public:

		/*
		 * Write string representation of the section to a stream
		 * @param out stream
		 */
		void WriteToStr(std::ostream& out) const {
			WriteToStr(out, 0);
		}

		constexpr const_iterator cbegin() const {
			return sections.cbegin();
		}

		constexpr const_iterator cend() const {
			return sections.cend();
		}

		constexpr iterator begin() {
			return sections.begin();
		}

		constexpr iterator end() {
			return sections.end();
		}

		constexpr const std::vector<ProfilerSection>& GetSubSections() const {
			return sections;
		}

		/*
		 * @return is a sub section running in this section
		 */
		constexpr bool IsRunning() const {
			return current != nullptr;
		}

		/*
		 * Create a sub section
		 * @param name sub section name
		 */
		void PushSection(std::string name) {
			if (IsRunning()) {
				current->PushSection(name);
				return;
			}

			sections.emplace_back(name);
			current = &sections[sections.size() - 1];
		}

		/*
		 * End the latest sub section or this section
		 */
		bool PopSection() {
			if (IsRunning()) {
				if (current->PopSection()) {
					current = nullptr;
				}
				return false;
			}
			endTime = GetTimestamp();
			return true;
		}

		/*
		 * Stop this section and all running sub sections
		 */
		void Stop() {
			if (IsRunning()) {
				current->Stop();
			}
			endTime = GetTimestamp();
		}

		constexpr ProfTs GetMillis() const {
			return endTime - startTime;
		}

		constexpr ProfTs GetStartMillis() const {
			return startTime;
		}

		constexpr ProfTs GetEndMillis() const {
			return endTime;
		}

		/*
		 * @return checksum of the section
		 */
		int ComputeCheckSum() const {
			int res = (int)name.size();
			res = 31 * res + (int)(startTime ^ (startTime >> 32));
			res = 31 * res + (int)(endTime ^ (endTime >> 32));
			for (const auto& s : sections) {
				res = 31 * res ^ s.ComputeCheckSum();
			}
			return res;
		}

		/*
		 * @return current running sub section or this
		 */
		ProfilerSection& GetCurrent() {
			if (IsRunning()) {
				return current->GetCurrent();
			}
			return *this;
		}
		
	};

	// Profiler
	class Profiler {
		std::string name;
		ProfilerSection mainSec;

	public:
		/*
		 * Create profiler
		 * @param name name
		 */
		Profiler(std::string name) : name(name), mainSec(name) {}
		/*
		 * Load profiler from stream
		 * @param is stream
		 */
		Profiler(std::istream& is) : name("") {
			char magicBuffer[sizeof(PROFILER_MAGIC)];

			static_assert(sizeof(PROFILER_MAGIC) != 0);
			is.read(magicBuffer, sizeof(PROFILER_MAGIC) - 1);
			
			if (memcmp(magicBuffer, PROFILER_MAGIC, sizeof(PROFILER_MAGIC) - 1)) {
				throw std::runtime_error("Invalid profiler magic");
			}

			mainSec = ProfilerSection{ is };
			int checksum = mainSec.ComputeCheckSum();

			int checksumRead = (int) actslib::io::ReadInt64(is);

			if (checksum != checksumRead) {
				throw std::runtime_error("Invalid profiler checksum");
			}
		}

		/*
		 * @return main section
		 */
		const ProfilerSection& GetMainSection() const {
			return mainSec;
		}

		/*
		 * @return current running section
		 */
		ProfilerSection& GetCurrent() {
			return mainSec.GetCurrent();
		}

		/*
		 * Start new section
		 * @param name section's name
		 */
		void PushSection(std::string name) {
			mainSec.PushSection(name);
		}

		/*
		 * End a section
		 */
		void PopSection() {
			mainSec.PopSection();
		}

		/*
		 * End the profiler
		 */
		void Stop() {
			mainSec.Stop();
		}

		/*
		 * Reset the profiler
		 */
		void Reset() {
			mainSec = ProfilerSection{ name };
		}

		/*
		 * Serialize the profiler into a stream
		 * @param os stream
		 */
		void Write(std::ostream& os) const {
			// write header
			os.write(PROFILER_MAGIC, sizeof(PROFILER_MAGIC) - 1);
			mainSec.Write(os);
			actslib::io::WriteInt64(os, mainSec.ComputeCheckSum());
		}

		/*
		 * Write the profiler into a stream
		 * @param out stream
		 */
		void WriteToStr(std::ostream& out) const {
			mainSec.WriteToStr(out);
		}
	};

	// Object to profile a section, push and pop when constructed/destroyed
	class ProfiledSection {
		Profiler& prof;
	public:
		/*
		 * Create profiled section
		 * @param prof parent profiler
		 * @param name section's name
		 */
		ProfiledSection(Profiler& prof, std::string name) : prof(prof) {
			prof.PushSection(name);
		};
		~ProfiledSection() {
			prof.PopSection();
		}
	};
}