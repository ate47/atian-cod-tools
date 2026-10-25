#include <includes.hpp>
#include <core/config.hpp>
#include <tools/mcp/mcp.hpp>
#include <tools/mcp/mcp_tool.hpp>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

namespace tool::mcp {

    namespace {
        constexpr const char* MCP_PROTOCOL_VERSION = "2025-03-26";
        constexpr const char* EMPTY_SCHEMA = R"({"type":"object","properties":{}})";

        constexpr int RPC_PARSE_ERROR = -32700;
        constexpr int RPC_INVALID_REQUEST = -32600;
        constexpr int RPC_METHOD_NOT_FOUND = -32601;
        constexpr int RPC_INVALID_PARAMS = -32602;

        rapidjson::Value MakeString(const char* str, rapidjson::Document::AllocatorType& alloc) {
            rapidjson::Value val{};
            val.SetString(str, alloc);
            return val;
        }

        // Build a JSON-RPC error response
        std::string RpcError(const rapidjson::Value* id, int code, const char* message) {
            rapidjson::Document doc{};
            doc.SetObject();
            rapidjson::Document::AllocatorType& alloc{ doc.GetAllocator() };
            doc.AddMember("jsonrpc", "2.0", alloc);
            rapidjson::Value idVal{};
            if (id) {
                idVal.CopyFrom(*id, alloc);
            }
            doc.AddMember("id", idVal, alloc);
            rapidjson::Value err{ rapidjson::kObjectType };
            err.AddMember("code", code, alloc);
            err.AddMember("message", MakeString(message, alloc), alloc);
            doc.AddMember("error", err, alloc);
            return core::config::JsonToString(doc);
        }
    } // namespace

    std::string HandleRpc(const std::string& body, int& status) {
        status = 200;
        rapidjson::Document req{};
        req.Parse(body.data(), body.size());
        if (req.HasParseError()) {
            return RpcError(nullptr, RPC_PARSE_ERROR, "Parse error");
        }
        if (!req.IsObject() || !req.HasMember("method") || !req["method"].IsString()) {
            return RpcError(nullptr, RPC_INVALID_REQUEST, "Invalid request");
        }

        const rapidjson::Value* id{ req.HasMember("id") ? &req["id"] : nullptr };
        const char* method{ req["method"].GetString() };

        // notifications don't have any response
        if (!id) {
            status = 202;
            return "";
        }

        rapidjson::Document doc{};
        doc.SetObject();
        rapidjson::Document::AllocatorType& alloc{ doc.GetAllocator() };
        doc.AddMember("jsonrpc", "2.0", alloc);
        rapidjson::Value idVal{};
        idVal.CopyFrom(*id, alloc);
        doc.AddMember("id", idVal, alloc);

        rapidjson::Value result{ rapidjson::kObjectType };

        switch (hash::HashX64(method)) {
            using namespace hash::literals;
        case "initialize"_x: {
            result.AddMember("protocolVersion", MakeString(MCP_PROTOCOL_VERSION, alloc), alloc);
            rapidjson::Value caps{ rapidjson::kObjectType };
            caps.AddMember("tools", rapidjson::Value{ rapidjson::kObjectType }, alloc);
            result.AddMember("capabilities", caps, alloc);
            rapidjson::Value info{ rapidjson::kObjectType };
            info.AddMember("name", "acts-mcp", alloc);
            info.AddMember("version", "0.0.1", alloc);
            result.AddMember("serverInfo", info, alloc);
            break;
        }
        case "ping"_x: {
            // empty result
            break;
        }
        case "tools/list"_x: {
            rapidjson::Value list{ rapidjson::kArrayType };
            for (const McpTool* tool : GetMcpTools()) {
                rapidjson::Value t{ rapidjson::kObjectType };
                t.AddMember("name", MakeString(tool->name, alloc), alloc);
                t.AddMember("description", MakeString(tool->description, alloc), alloc);
                rapidjson::Document schema{};
                schema.Parse(tool->inputSchema ? tool->inputSchema : EMPTY_SCHEMA);
                if (schema.HasParseError()) {
                    schema.Parse(EMPTY_SCHEMA);
                }
                rapidjson::Value schemaVal{};
                schemaVal.CopyFrom(schema, alloc);
                t.AddMember("inputSchema", schemaVal, alloc);
                list.PushBack(t, alloc);
            }
            result.AddMember("tools", list, alloc);
            break;
        }
        case "tools/call"_x: {
            const rapidjson::Value* params{ req.HasMember("params") ? &req["params"] : nullptr };
            if (!params || !params->IsObject() || !params->HasMember("name") || !(*params)["name"].IsString()) {
                return RpcError(id, RPC_INVALID_PARAMS, "Missing tool name");
            }
            const char* name{ (*params)["name"].GetString() };
            const McpTool* tool{ FindMcpTool(name) };
            if (!tool) {
                return RpcError(id, RPC_INVALID_PARAMS, "Unknown tool");
            }

            rapidjson::Value noArgs{};
            const rapidjson::Value& args{ params->HasMember("arguments") ? (*params)["arguments"] : noArgs };

            bool isError{};
            std::string text{};
            try {
                text = tool->handler(args, isError);
            } catch (std::exception& e) {
                isError = true;
                text = e.what();
            }

            rapidjson::Value content{ rapidjson::kArrayType };
            rapidjson::Value item{ rapidjson::kObjectType };
            item.AddMember("type", "text", alloc);
            item.AddMember("text", MakeString(text.c_str(), alloc), alloc);
            content.PushBack(item, alloc);
            result.AddMember("content", content, alloc);
            result.AddMember("isError", isError, alloc);
            break;
        }
        default:
            return RpcError(id, RPC_METHOD_NOT_FOUND, "Method not found");
        }
        doc.AddMember("result", result, alloc);
        return core::config::JsonToString(doc);
    }
} // namespace tool::mcp