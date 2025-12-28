#pragma once

namespace hash {
	struct HashAlg {
		const char* id;
		const char* desc;
		std::function<uint64_t(const char* text)> hashFunc;
		char buffer[0x20]{ 0 };
		bool selected{};
		
		static HashAlg algs[13];
		static void SyncAlgCfg();
	};


}