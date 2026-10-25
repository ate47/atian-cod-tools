#pragma once
#include <rapidjson/document.h>

namespace tool::mcp {
    /*
     * Handler of an MCP tool
     * @param args "arguments" object of the call (can be null if none were sent)
     * @param isError [out] set to true to report a tool error
     * @return text content returned to the client
     */
    using McpToolHandler = std::string (*)(const rapidjson::Value& args, bool& isError);

    /*
     * MCP tool definition
     */
    struct McpTool {
        const char* name;
        const char* description;
        // JSON schema of the arguments
        const char* inputSchema;
        McpToolHandler handler;
    };

    /*
     * @return the Mcp tools list
     */
    std::vector<McpTool*>& GetMcpTools();

    /*
     * Find a Mcp tool
     * @param name tool name
     * @return tool or nullptr
     */
    McpTool* FindMcpTool(const char* name);

} // namespace tool::mcp

#define ADD_MCP_TOOL(name, desc, schema, handler)                                                                      \
    utils::ArrayAdder<tool::mcp::McpTool, tool::mcp::McpTool> __mcpTool_##name {                                       \
        tool::mcp::GetMcpTools(), #name, desc, schema, handler                                                         \
    }