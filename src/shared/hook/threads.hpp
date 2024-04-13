#pragma once

namespace hook::threads {
	struct ThreadData {
		DWORD id;
		DWORD owner;
		DWORD flags;
	};
	std::vector<ThreadData> ListThreads();

	void ForEachThread(std::function<void(ThreadData& data)> func);
}