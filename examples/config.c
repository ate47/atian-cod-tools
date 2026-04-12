#include <stdio.h>
#include <acts_api/version.h>
#include <acts_api/config.h>

void example_config() {
	// see api.c
	if (ACTS_NOT_OK(ActsAPIVersion_ValidateVersion())) {
		fprintf(stderr, "ActsAPIVersion_ValidateVersion error : %s\n", ActsGetAPILastMessage());
		return; // we should not continue after failing a version
	}

	/*
	 * The config api allows to read and set config data in acts configs
	 * #include <acts_api/config.h>
	 */

	// Sync the config with the one on disk
	ActsAPIConfig_SyncConfig(false);

	// different functions can be used to get config values
	const char* testString = ActsAPIConfig_GetString("example.testString", "my default value");
	int64_t testInteger = ActsAPIConfig_GetInteger("example.testInteger", 3);
	double testDouble = ActsAPIConfig_GetDouble("example.testDouble", 2.5);
	bool testBool = ActsAPIConfig_GetBool("example.testBool", false);

	printf("testString .. %s\n", testString);
	printf("testInteger . %lld\n", testInteger);
	printf("testDouble .. %f\n", testDouble);
	printf("testBool .... %s\n", testBool ? "true" : "false");

	// different functions can be used to set config values
	ActsAPIConfig_SetString("example.testSetString", "string");
	ActsAPIConfig_SetInteger("example.testSetInteger", 233);
	ActsAPIConfig_SetDouble("example.testSetDouble", 12);
	ActsAPIConfig_SetBool("example.testSetBool", true);

	// Once the data are set, they need to be saved for future usage
	ActsAPIConfig_SaveConfig();

}