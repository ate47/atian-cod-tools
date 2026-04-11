#ifndef __ACTS_API_GSC_H__
#define __ACTS_API_GSC_H__
#include "api.h"
/*
 * Atian Tools GSC API
 */

// acts opcode value
enum ActsAPIGsc_OpCode {
	OPCODE_Undefined = 0,
};
	
// ingame opcode value
typedef uint16_t ActsAPIGsc_OpCodeValue;
// gsc file magic
typedef uint64_t ActsAPIGsc_VmMagic;
#define ActsAPIGsc_VmMagic_INVALID 0

// vm platform
enum ActsAPIGsc_Platform {
	PLATFORM_UNKNOWN = 0,
	PLATFORM_PC = 1,
	PLATFORM_PLAYSTATION = 2,
	PLATFORM_XBOX = 3,
	PLATFORM_PC_ALPHA = 4,
	PLATFORM_ACTS_TEST = 5,
	PLATFORM_BO6_V1 = 6,
	PLATFORM_BO6 = 7,
	PLATFORM_BO7 = 8,
	PLATFORM_COUNT,
};
/*
 * Find a gsc magic by a vm name
 * @param name
 * @return magic or ActsAPIGsc_VmMagic_INVALID if not found
 */
ACTS_COMMON_API ActsAPIGsc_VmMagic ActsAPIGsc_FindGscMagic(const char* name);

/*
 * Create an magic by a vm name
 * @param name vm name
 * @return gsc vm handle or INVALID_ACTS_HANDLE_VALUE if invalid
 */
ACTS_COMMON_API ActsHandle ActsAPIGsc_FindGscVm(const char* name);
/*
 * Create an magic by a vm magic
 * @param magic vm magic
 * @return gsc vm handle or INVALID_ACTS_HANDLE_VALUE if invalid
 */
ACTS_COMMON_API ActsHandle ActsAPIGsc_FindGscVmByMagic(ActsAPIGsc_VmMagic magic);
/*
 * Find an opcode by its name
 * @param name name
 * @return opcode or OPCODE_Undefined if undefined
 */
ACTS_COMMON_API ActsAPIGsc_OpCode ActsAPIGsc_FindOpCodeByName(const char* name);
/*
 * Get the name of an opcode
 * @param opcode opcode
 * @return name of the opcode
 */
ACTS_COMMON_API const char* ActsAPIGsc_GetOpCodeName(ActsAPIGsc_OpCode opcode);

/* gsc vm handle related functions */

/*
 * Use the hash field function of a gsc vm
 * @param gscvm gsc vm handle
 * @param string string to hash
 * @return hashed string
 */
ACTS_COMMON_API uint64_t ActsAPIGsc_HashField(ActsHandle gscvm, const char* string);
/*
 * Use the hash path function of a gsc vm
 * @param gscvm gsc vm handle
 * @param string string to hash
 * @return hashed string
 */
ACTS_COMMON_API uint64_t ActsAPIGsc_HashPath(ActsHandle gscvm, const char* string);
/*
 * Lookup an opcode value for a gsc vm and platform
 * @param gscvm gsc vm handle
 * @param platform platform
 * @param opcode game opcode
 * @return opcode or OPCODE_Undefined
 */
ACTS_COMMON_API ActsAPIGsc_OpCode LookupOpCode(ActsHandle gscvm, ActsAPIGsc_Platform platform, ActsAPIGsc_OpCodeValue opcode);
/*
 * Get an opcode value for a gsc vm and platform
 * @param gscvm gsc vm handle
 * @param platform platform
 * @param modTool use mod tool opcodes (if available)
 * @param outValue valid pointer to the out value
 * @return OK if the value was found, otherwise ERROR with ActsAPISetLastMessage set to error message
 */
ACTS_COMMON_API ActsStatus GetOpCode(ActsHandle gscvm, ActsAPIGsc_Platform platform, ActsAPIGsc_OpCode opcode, bool modTool, ActsAPIGsc_OpCodeValue* outValue);
/*
 * Test if an opcode value exists for a gsc vm and platform
 * @param gscvm gsc vm handle
 * @param platform platform
 * @param modTool use mod tool opcodes (if available)
 * @return if the opcode value is registered for this vm
 */
ACTS_COMMON_API bool HasOpCode(ActsHandle gscvm, ActsAPIGsc_Platform platform, ActsAPIGsc_OpCode opcode, bool modTool);
/*
 * Get the magic of a gsc vm
 * @param gscvm gsc vm handle
 * @return value
 */
ACTS_COMMON_API ActsAPIGsc_VmMagic ActsAPIGsc_GetMagic(ActsHandle gscvm);
/*
 * Get the gdb magic of a gsc vm
 * @param gscvm gsc vm handle
 * @return value
 */
ACTS_COMMON_API ActsAPIGsc_VmMagic ActsAPIGsc_GetGDBMagic(ActsHandle gscvm);
/*
 * Get the name of a gsc vm
 * @param gscvm gsc vm handle
 * @return value
 */
ACTS_COMMON_API const char* ActsAPIGsc_GetName(ActsHandle gscvm);
/*
 * Get the code name of a gsc vm
 * @param gscvm gsc vm handle
 * @return value
 */
ACTS_COMMON_API const char* ActsAPIGsc_GetCodeName(ActsHandle gscvm);
/*
 * Get the internal name of a gsc vm
 * @param gscvm gsc vm handle
 * @return value
 */
ACTS_COMMON_API const char* ActsAPIGsc_GetInternalName(ActsHandle gscvm);


#endif // __ACTS_API_GSC_H__