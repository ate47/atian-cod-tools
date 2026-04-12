#include <stdio.h>
#include <acts_api/version.h>
#include <acts_api/gsc_compiler.h>

void write_gsc_callback(void* ud, const uint8_t* buffer, size_t len) {
	// empty write callback, but the user can copy the compiled data data
}

void example_gsc() {
	// see api.c
	if (ACTS_NOT_OK(ActsAPIVersion_ValidateVersion())) {
		fprintf(stderr, "ActsAPIVersion_ValidateVersion error : %s\n", ActsGetAPILastMessage());
		return; // we should not continue after failing a version
	}

	/*
	 * The gsc api allows to compile gsc scripts
	 * #include <acts_api/gsc_compiler.h>
	 */

	// first we need to get the id of the vm we want to compile
	ActsAPIGsc_VmMagic vm = ActsAPIGsc_FindGscMagic("bo4");

	if (!vm) {
		fprintf(stderr, "Can't find vm bo4\n");
		return;
	}

	// we create a compiler context
	ActsHandle context = ActsAPIGscCompiler_CreateCompilerContext(
		vm, 
		PLATFORM_PC,
		"scripts/core_common/acts/my_custom_script.gsc",
		false,
		NULL
	);

	if (!IS_ACTS_HANDLE_VALID(context)) {
		fprintf(stderr, "Can't allocate context\n");
		return;
	}

	// we need to add input files, we can also mention a directory by using the recurse param to true
	ActsAPIGscCompiler_AddInput(context, "test/gsc-compiler/t89/demo.gsc", false);

	if (ACTS_NOT_OK(ActsAPIGscCompiler_Compile(context, write_gsc_callback, NULL))) {
		fprintf(stderr, "failed to compile : %s\n", ActsGetAPILastMessage());
	}
	printf("script compiled\n");

	// the handle should be closed after usage
	ActsAPICloseHandle(context);
}