#include <includes.hpp>
#include <unit_test.hpp>
#include <core/raw_file.hpp>

namespace {
	void rawfiletest() {
		core::raw_file::RawFileWriter writer{};

		writer << "test: " << "#\"" << core::raw_file::RFHash{ hash::Hash64("test") } << "\"";

		std::vector<byte> buff{ writer.Build() };

		ASSERT_VAL("bad size", buff.size() >= 8);
		ASSERT_EQ("bad magic", *(uint64_t*)buff.data(), core::raw_file::MAGIC);

		core::bytebuffer::ByteBuffer bb{ buff };
		core::raw_file::RawFileReader reader{ bb };

		ASSERT_VAL("no field 1", reader.GotoNextField());
		ASSERT_EQ("bad type 1", reader.GetType(), core::raw_file::RFDT_STRING);
		ASSERT_EQ("bad str 1", "test: #\""s, reader.GetString());
		ASSERT_VAL("no field 2", reader.GotoNextField());
		ASSERT_EQ("bad type 2", reader.GetType(), core::raw_file::RFDT_HASH);
		ASSERT_EQ("bad hash 2", hash::Hash64("test"), reader.GetHash());
		ASSERT_VAL("no field 3", reader.GotoNextField());
		ASSERT_EQ("bad type 3", reader.GetType(), core::raw_file::RFDT_STRING);
		ASSERT_EQ("bad str 3", "\""s, reader.GetString());
		ASSERT_VAL("field 4", !reader.GotoNextField());

	}

	void rawfilereadalltest() {
		core::raw_file::RawFileWriter writer{};

		writer << "test: " << "#\"" << core::raw_file::RFHash{ hash::Hash64("test") } << "\"";

		std::vector<byte> buff{ writer.Build() };

		core::bytebuffer::ByteBuffer bb{ buff };
		core::raw_file::RawFileReader reader{ bb, nullptr, [](uint64_t c) -> const char* {return hashutils::ExtractTmp("hash", c); } };

		hashutils::AddPrecomputed(hash::Hash64("test"), "test", true);
		std::string expected{ "test: #\"test\"" };

		std::string actual{ reader.ReadAll() };

		LOG_DEBUG("'{}'", actual);

		ASSERT_EQ("not the same", expected, actual);
	}

	ADD_TEST(rawfile, rawfiletest);
	ADD_TEST(rawfilereadall, rawfilereadalltest);
}