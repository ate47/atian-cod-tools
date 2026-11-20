#include <includes.hpp>
#include <tools/mcp/mcp_tool.hpp>
#include <acts_api/http_server.h>
#include <core/config.hpp>

namespace tool::mcp {
    std::vector<McpTool*>& GetMcpTools() {
        static std::vector<McpTool*> tools;
        return tools;
    }
    McpTool* FindMcpTool(const char* name) {
        for (McpTool* tool : GetMcpTools()) {
            if (!_strcmpi(tool->name, name)) {
                return tool;
            }
        }
        return nullptr;
    }
    std::vector<std::string> GetMcpCategories() {
        std::set<std::string> categories{};
        for (const McpTool* tool : GetMcpTools()) {
            categories.insert(tool->category);
        }
        return { categories.begin(), categories.end() };
    }
    bool HasMcpCategory(const char* category) {
        for (const McpTool* tool : GetMcpTools()) {
            if (!_strcmpi(tool->category, category)) {
                return true;
            }
        }
        return false;
    }
    bool IsMcpToolInCategory(const McpTool& tool, const char* category) {
        return !category || !_strcmpi(tool.category, category);
    }
    namespace {
#ifndef CI_BUILD

        // Blank tools, placeholders to be replaced by real tools

        std::string BlankTool(const rapidjson::Value& args, bool& isError) { return "blank"; }

        std::string BlankToolArgs(const rapidjson::Value& args, bool& isError) {
            if (!args.IsObject() || !args.HasMember("value") || !args["value"].IsString()) {
                isError = true;
                return "Missing 'value' argument";
            }
            return std::format("blank: {}", args["value"].GetString());
        }

        ADD_MCP_TOOL(blank_tool, "common", "Blank tool, does nothing", nullptr, BlankTool);
        ADD_MCP_TOOL(
            blank_tool_args, "common", "Blank tool with an argument",
            R"({"type":"object","properties":{"value":{"type":"string","description":"Blank value"}},"required":["value"]})",
            BlankToolArgs
        );
#endif

        int mcp_tools(int argc, const char* argv[]) {
            std::vector<mcp::McpTool*>& tools{ mcp::GetMcpTools() };
            LOG_INFO("MCP tools ({}):", tools.size());
            for (mcp::McpTool* tool : tools) {
                LOG_INFO("[{}] {} - {}", tool->category, tool->name, tool->description);
            }

            return tool::OK;
        }
        ADD_TOOL(mcp_tools, "dev", "", "print mcp tools", mcp_tools);
    } // namespace

} // namespace tool::mcp

ActsStatus ActsAPIHttpServer_ListMcpTools(ActsAPIHttpServer_ListMcpTools_Callback callback, void* ud) {
    ACTS_API_ASSERT_NOT_NULL(callback);
    for (const tool::mcp::McpTool* tool : tool::mcp::GetMcpTools()) {
        ActsAPIHttpServer_McpTool info{ tool->name, tool->category, tool->description, tool->inputSchema };
        if (!callback(&info, ud)) {
            break;
        }
    }
    return ACTS_STATUS_OK;
}