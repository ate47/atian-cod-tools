#include <includes.hpp>
#include <tools/mcp/mcp_tool.hpp>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

namespace {
    constexpr const char* HASH_BAD_FORMAT = "bad format";
    constexpr const char* HASH_UNKNOWN = "unknown hash";

    // Lookup a hash, returns the string, HASH_UNKNOWN or HASH_BAD_FORMAT
    const char* LookupHash(const rapidjson::Value& val) {
        if (!val.IsString()) {
            return HASH_BAD_FORMAT;
        }
        uint64_t myHash{};
        if (!hash::TryHashPattern(val.GetString(), myHash)) {
            return HASH_BAD_FORMAT;
        }
        const char* extracted{ hashutils::ExtractPtr(myHash) };
        if (!extracted) {
            return HASH_UNKNOWN;
        }
        return extracted;
    }

    std::string HashTool(const rapidjson::Value& args, bool& isError) {
        if (!args.IsObject() || !args.HasMember("hash")) {
            isError = true;
            return "Missing 'hash' argument";
        }
        const rapidjson::Value& hashes{ args["hash"] };

        rapidjson::StringBuffer sb{};
        rapidjson::Writer<rapidjson::StringBuffer> writer{ sb };

        if (hashes.IsArray()) {
            writer.StartArray();
            for (const rapidjson::Value& h : hashes.GetArray()) {
                writer.String(LookupHash(h));
            }
            writer.EndArray();
        } else {
            writer.String(LookupHash(hashes));
        }
        return std::string{ sb.GetString(), sb.GetSize() };
    }

    ADD_MCP_TOOL(
        hash_lookup,
        "Lookup hashes in the hash database. Takes a hash or an array of hashes ('hash_xxx' pattern) "
        "and "
        "returns, for each one, the string, \"unknown hash\" or \"bad format\". The answer is an array if the "
        "input is an array.",
        R"({"type":"object","properties":{"hash":{"description":"Hash or array of hashes","oneOf":[{"type":"string"},{"type":"array","items":{"type":"string"}}]}},"required":["hash"]})",
        HashTool
    );

} // namespace