#ifndef __ACTS_API_TOOL_H__
#define __ACTS_API_TOOL_H__
#include "api.h"
/*
 * Atian Tools Tool API
 */

typedef enum {
	ActsAPITool_OK = 0,
	ActsAPITool_BASIC_ERROR = -1,
	ActsAPITool_BAD_USAGE = -2
} ActsAPITool_ERROR;

 /*
  * Register a new tool
  * @param name tool name
  * @param filename file name
  * @param line file line
  * @param category category
  * @param usage usage
  * @param description description
  * @param func function callback
  */
ACTS_COMMON_API void ActsAPITool_RegisterActsTool_(const char* name, const char* filename, size_t line, const char* category, const char* usage, const char* description, int(*func)(int argc, const char* argv[]));


/*
 * Register a new tool
 * @param name tool name
 * @param category category
 * @param usage usage
 * @param description description
 * @param func function callback
 */
#define ActsAPITool_RegisterActsTool(name, category, usage, description, func) ActsAPITool_RegisterActsTool_(name, __FILE__, __LINE__, category, usage, description, func)
/*
 * Register a new tool category
 * @param description description
 * @param visible if the category is visible
 */
ACTS_COMMON_API void ActsAPITool_RegisterActsCategory(const char* name, const char* description, bool visible);



#endif // __ACTS_API_TOOL_H__