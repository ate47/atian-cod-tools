#pragma once

namespace hash {
	struct HashAlg {
		const char* id;
		const char* desc;
		uint64_t(*hashFunc)(const char* text);
		char buffer[0x20]{ 0 };
		bool selected{};
		
		static HashAlg algs[13];
		static void SyncAlgCfg();
	};


}