#include <includes_shared.hpp>
#include "library.hpp"
#include "utils.hpp"
#include "threads.hpp"

namespace hook::threads {

	std::vector<ThreadData> ListThreads() {
		std::vector<ThreadData> res{};


		HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);

		if (hSnap == INVALID_HANDLE_VALUE) {
			throw std::runtime_error("Can't create snapshot for threads");
		}
		THREADENTRY32 te;
		te.dwSize = sizeof(te);

		if (!Thread32First(hSnap, &te)) {
			CloseHandle(hSnap);
			throw std::runtime_error("Can't fetch first thread");
		}
		do {
			res.emplace_back(te.th32ThreadID, te.th32OwnerProcessID, te.dwFlags);
		} while (Thread32Next(hSnap, &te));

		CloseHandle(hSnap);

		return res;
	}

	void ForEachThread(std::function<void(ThreadData& data)> func) {
		for (auto& th : ListThreads()) {
			func(th);
		}
	}
}