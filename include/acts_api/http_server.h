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
