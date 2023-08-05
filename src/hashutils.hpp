#pragma once
#include <Windows.h>

namespace hashutils {
	void ReadDefaultFile();
	void LoadMap(LPCWCH file);
	void Add(LPCCH str);
	bool Extract(LPCCH type, UINT64 hash, LPCH out, SIZE_T outSize);
	LPCCH ExtractTmp(LPCCH type, UINT64 hash);
	LPCCH ExtractPtr(UINT64 hash);
	SIZE_T Size();

	UINT32 Hash32(LPCCH str);
	UINT64 Hash64(LPCCH str);
}