#ifndef __ACTS_API_HTTP_SERVER_H__
#define __ACTS_API_HTTP_SERVER_H__
#include "api.h"
/*
 * Embedded http server (with the MCP server) of the ACTS common library.
 * It doesn't use any thread: the owner drives it with ActsAPIHttpServer_Poll. Start, Poll and the close of the handle
 * must be done from the same thread.
 */

#define ACTS_HTTP_DEFAULT_PORT 22808
#define ACTS_HTTP_DEFAULT_HOST "127.0.0.1"
/*
 * Http server options
 */
typedef struct {
    size_t structSize;
    // host to bind, null for the default one
    const char* host;
    // port to bind, 0 for the default one
    uint16_t port;
    // disable the MCP endpoint
    bool noMCP;
} ActsAPIHttpServer_Options;

/*
 * MCP tool information
 */
typedef struct {
    // tool name
    const char* name;
    // tool category, the tool is available on the /mcp and /mcp/{category} endpoints
    const char* category;
    // tool description
    const char* description;
    // JSON schema of the tool arguments, can be null if the tool doesn't have any
    const char* inputSchema;
} ActsAPIHttpServer_McpTool;

// callback for ActsAPIHttpServer_ListMcpTools, called for each tool
// the tool is only valid during the call
// return if we need to continue the listing
typedef bool (*ActsAPIHttpServer_ListMcpTools_Callback)(const ActsAPIHttpServer_McpTool* tool, void* ud);

/*
 * List the registered MCP tools
 * @param callback callback called for each tool
 * @param ud user data sent to the callback
 * @return success or failure, ActsGetAPILastMessage is set to the result status
 */
ACTS_COMMON_API ActsStatus ActsAPIHttpServer_ListMcpTools(ActsAPIHttpServer_ListMcpTools_Callback callback, void* ud);

/*
 * Create and start the embedded http server, use ActsAPICloseHandle to stop and free it.
 * @param options server options
 * @return server handle, or an invalid handle if the server can't be started (see ActsGetAPILastMessage)
 */
ACTS_COMMON_API ActsHandle ActsAPIHttpServer_Start(const ActsAPIHttpServer_Options* options);

/*
 * Process the pending events of the http server without blocking
 * @param server server handle
 */
ACTS_COMMON_API ActsStatus ActsAPIHttpServer_Poll(ActsHandle server);

#endif // __ACTS_API_HTTP_SERVER_H__
