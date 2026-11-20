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
        // category of the tool, it is used by the /mcp/{category} endpoint (the /mcp endpoint has all the tools)
        const char* category;
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

    /*
     * @return the sorted list of the categories used by the Mcp tools
     */
    std::vector<std::string> GetMcpCategories();

    /*
     * Check if a category is used by at least one Mcp tool
     * @param category category name (case insensitive)
     * @return true if the category exists
     */
    bool HasMcpCategory(const char* category);

    /*
     * Check if a tool is in a category
     * @param tool tool
     * @param category category name (case insensitive), nullptr for all the categories
     * @return true if the tool is in the category
     */
    bool IsMcpToolInCategory(const McpTool& tool, const char* category);

} // namespace tool::mcp

// Register a MCP tool (same order as ADD_TOOL: name, category), the category is a string ("common", "gsc", ...) used to
// filter the tools by endpoint
#define ADD_MCP_TOOL(name, category, desc, schema, handler)                                                            \
    utils::ArrayAdder<tool::mcp::McpTool, tool::mcp::McpTool> __mcpTool_##name {                                       \
        tool::mcp::GetMcpTools(), #name, category, desc, schema, handler                                               \
    }
