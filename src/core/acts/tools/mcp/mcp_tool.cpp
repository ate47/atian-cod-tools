#include <includes.hpp>
#include <tools/mcp/mcp_tool.hpp>
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

        ADD_MCP_TOOL(blank_tool, "Blank tool, does nothing", nullptr, BlankTool);
        ADD_MCP_TOOL(
            blank_tool_args, "Blank tool with an argument",
            R"({"type":"object","properties":{"value":{"type":"string","description":"Blank value"}},"required":["value"]})",
            BlankToolArgs
        );
#endif

        int mcp_tools(int argc, const char* argv[]) {
            std::vector<mcp::McpTool*>& tools{ mcp::GetMcpTools() };
            LOG_INFO("MCP tools ({}):", tools.size());
            for (mcp::McpTool* tool : tools) {
                LOG_INFO("{} - {}", tool->name, tool->description);
            }

            return tool::OK;
        }
        ADD_TOOL(mcp_tools, "dev", "", "print mcp tools", mcp_tools);
    } // namespace

} // namespace tool::mcp