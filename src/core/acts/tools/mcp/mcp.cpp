#include <includes.hpp>
#include <core/config.hpp>
#include <cli/clicolor.hpp>
#include <tools/mcp/mcp.hpp>
#include <tools/mcp/mcp_tool.hpp>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <core/actsinfo.hpp>

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
            core::config::JDoc doc{};
            doc.SetString("jsonrpc", "2.0");
            if (id) {
                doc.Set("id", *id);
            }
            core::config::ConfigGeneric err{ doc.GetSubVal("error", true) };
            err.SetInteger("code", code);
            err.SetString("message", message);

            return doc.ToString();
        }
    } // namespace

    std::string HandleRpc(const std::string& body, int& status) {
        status = 200;
        core::config::JDoc req{};
        if (!req.ParseDocument(body.data(), body.size())) {
            return RpcError(nullptr, RPC_PARSE_ERROR, "Parse error");
        }

        const char* method{ req.GetCString("method") };

        if (!method) {
            return RpcError(nullptr, RPC_INVALID_REQUEST, "Invalid request");
        }

        core::config::RapidJsonGeneric* id{ &req.GetVal("id") };
        // notifications don't have any response
        if (id->IsNull()) {
            status = 202;
            return "";
        }

        core::config::JDoc doc{};
        doc.SetString("jsonrpc", "2.0");
        doc.Set("id", *id);

        core::config::ConfigGeneric result{ doc.GetSubVal("result", true) };

        const char* logInfo{};

        switch (hash::HashX64(method)) {
            using namespace hash::literals;
        case "initialize"_x: {
            result.SetString("protocolVersion", MCP_PROTOCOL_VERSION);
            result.Set("capabilities.tools", rapidjson::Value{ rapidjson::kObjectType });
            core::config::ConfigGeneric serverInfo{ result.GetSubVal("serverInfo", true) };

            serverInfo.SetString("name", "acts-mcp");
            serverInfo.SetString("version", core::actsinfo::VERSION);
            break;
        }
        case "ping"_x: {
            // empty result
            break;
        }
        case "tools/list"_x: {
            rapidjson::Value list{ rapidjson::kArrayType };
            for (const McpTool* tool : GetMcpTools()) {
                core::config::ConfigGeneric toolVal{ result.GetSubVal("tools[]", true) };
                toolVal.SetString("name", tool->name);
                toolVal.SetString("description", tool->description);

                rapidjson::Document schema{};
                schema.Parse(tool->inputSchema ? tool->inputSchema : EMPTY_SCHEMA);
                if (schema.HasParseError()) {
                    schema.Parse(EMPTY_SCHEMA);
                }
                toolVal.Set("inputSchema", schema);
            }
            break;
        }
        case "tools/call"_x: {
            const char* name{ req.GetCString("params.name") };
            if (!name) {
                return RpcError(id, RPC_INVALID_PARAMS, "Missing tool name");
            }
            logInfo = name;
            const McpTool* tool{ FindMcpTool(name) };
            if (!tool) {
                return RpcError(id, RPC_INVALID_PARAMS, "Unknown tool");
            }

            rapidjson::Value noArgs{};
            const rapidjson::Value& args{ req.GetVal("params.arguments") };

            bool isError{};
            std::string text{};
            try {
                text = tool->handler(args, isError);
            } catch (std::exception& e) {
                isError = true;
                text = e.what();
            }

            core::config::ConfigGeneric item{ result.GetSubVal("content[]", true) };
            item.SetString("type", "text");
            item.SetString("text", text);
            result.SetBool("isError", isError);
            break;
        }
        default:
            return RpcError(id, RPC_METHOD_NOT_FOUND, "Method not found");
        }

        LOG_INFO(
            "[RPC/MCP] {} {}",
            cli::clicolor::COLOR_AMBER.Text(method),
            cli::clicolor::COLOR_CORAL.Text(logInfo ? logInfo : "")
        );
        return doc.ToString();
    }
} // namespace tool::mcp