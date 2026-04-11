#include <stdio.h>
#include <acts_api/version.h>
#include <acts_api/hash.h>

void example_hash() {
	// see api.c
	if (ACTS_NOT_OK(ActsAPIVersion_ValidateVersion())) {
		fprintf(stderr, "ActsAPIVersion_ValidateVersion error : %s\n", ActsGetAPILastMessage());
		return; // we should not continue after failing a version
	}

	/*
	 * The hash api allows to hash and to compute hashes
	 * #include <acts_api/hash.h>
	 */


	// a hash function can be get for single usage
	ActsAPIHash_HashFunction hashFunction = ActsAPIHash_GetHashFunction(ACTS_API_HASH_XHASH);

	if (!hashFunction) {
		fprintf(stderr, "Can't find hash " ACTS_API_HASH_XHASH "\n");
		return;
	}

	printf("xhash(test) = hash_%llx\n", hashFunction("test"));


	// the api allows to load an index, for that you can call this function to load the file on disk
	ActsAPIHash_ReadDefaultHashFiles();

	// hashes can be added in the index using
	ActsAPIHash_AddPrecomputed(hashFunction("my string"), "my string", true);

	// hashes can be extracted from the index using
	printf("extract(0x17a849abd13c1604) = %s\n", ActsAPIHash_ExtractTmp("hash", 0x17a849abd13c1604));
}