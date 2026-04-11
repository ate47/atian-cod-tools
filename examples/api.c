#include <stdio.h>
#include <acts_api/version.h>


void example_api() {
	// get the different version information
	printf(
		"API Version %s (%x / %x)\n", 
		ActsAPIVersion_GetVersion(), 
		ActsAPIVersion_GetBuildVersion(), 
		ActsAPIVersion_GetVersionId()
	);

	// this check that the linked dll version is the same as the include version
	// ACTS_NOT_OK check a return status
	if (ACTS_NOT_OK(ActsAPIVersion_ValidateVersion())) {
		// when an error status is returned, ActsGetAPILastMessage contains the error message
		fprintf(stderr, "ActsAPIVersion_ValidateVersion error : %s\n", ActsGetAPILastMessage());

		return; // we should not continue after failing a version
	}


}