#pragma once

namespace tool::mcp {
    /*
     * Handle a JSON-RPC request
     * @param body request body
     * @param status [out] http status
     * @param category category of the endpoint, only the tools of this category are available (nullptr for all)
     * @return response body
     */
    std::string HandleRpc(const std::string& body, int& status, const char* category = nullptr);
} // namespace tool::mcp
