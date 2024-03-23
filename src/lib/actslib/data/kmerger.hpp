#pragma once
#include "../actslib.hpp"


namespace actslib::data {

	struct KMergerChunk {
		size_t layer;
		std::filesystem::path file;
	};

	class KMergerConfig {
	public:
		virtual void CreateDefaultChunk(const std::filesystem::path& chunkLocation) = 0;
		virtual bool CreateChunk(const std::filesystem::path& chunkLocation) = 0;
		virtual void MergeChunks(const std::vector<KMergerChunk>& chunks, const std::filesystem::path& chunkLocation) = 0;
	};

	struct KMergerTask {
		std::function<void()> func;
		const char* type;
		bool end{};
	};

	class KMerger {
		KMergerConfig& cfg;
		const size_t units;
		const size_t workersCount;
		const std::filesystem::path workdir;
		bool completed{};
		std::vector<std::thread> workers{};

		std::vector<KMergerChunk> chunks{};
		size_t maxLayer{};
		std::mutex chunksMutex;

		std::vector<KMergerTask> tasks{};
		std::condition_variable taskCondVar;
		std::mutex taskCondMutex;

		size_t idGen{};
		std::mutex idMutex{};

		const char* exception{};
		std::mutex exceptionMutex{};
	public:
		KMerger(const std::filesystem::path workdir, size_t units, size_t workersCount, KMergerConfig& cfg)
			: workdir(workdir), units(units), workersCount(workersCount), cfg(cfg) {
			if (!workersCount) {
				throw std::invalid_argument("No workers");
			}
			if (units < 2) {
				throw std::invalid_argument("units should be at least 2");
			}
		}

		size_t GetNewIdSync() {
			std::lock_guard lg{ idMutex };
			return ++idGen;
		}

		inline std::filesystem::path GetNewIdSyncPath() {
			return workdir / va("c_%lld", GetNewIdSync());
		}

		void PushTask(const char* type, std::function<void()> task, bool sync) {
			if (sync) {
				std::lock_guard lg{ taskCondMutex };

				tasks.emplace_back(task, type);

				taskCondVar.notify_one();
			}
			else {
				tasks.emplace_back(task, type);

				taskCondVar.notify_one();
			}
		}

	private:

		KMergerTask PopTask() {
			std::unique_lock ul{ taskCondMutex };

			auto& that = *this;

			taskCondVar.wait(ul, [&that] {return !that.tasks.empty() || that.completed;  });

			if (tasks.empty() || exception) {
				return { [] {}, "empty", true };
			}

			auto f = tasks[tasks.size() - 1];

			tasks.pop_back();

			ul.unlock();

			return f;
		}

		bool FindChunksToMerge(std::vector<KMergerChunk>& ch) {
			if (completed) {
				ALOG_TRACE("completed size: {}", chunks.size());
				if (chunks.size() > 1) {
					ch.reserve(chunks.size() > units ? units : chunks.size());
					if (units > chunks.size()) {
						ch.insert(ch.begin(), chunks.begin(), chunks.end());
						chunks.clear();
					}
					else {
						auto start = chunks.begin() + (chunks.size() - units);
						ch.insert(ch.begin(), start, chunks.end());
						chunks.erase(start, chunks.end());
					}
					return true;
				}
				ALOG_TRACE("done size: {}", chunks.size());
				return false; // done
			}

			// not completed, we need to search for the same layers
			
			// clear chunks
			for (size_t i = 0; i <= maxLayer; i++) {
				size_t count{};

				for (const auto& chunk : chunks) {
					if (chunk.layer == i) {
						count++;
					}
				}

				if (count >= units) {
					ch.reserve(units);
					auto it = chunks.begin();

					// insert the nodes at the right level into ch
					while (ch.size() < units) {
						if (it->layer == i) {
							ch.push_back(*it);
							it = chunks.erase(it);
							continue;
						}
						it++;
					}

					return true;
				}
			}

			return false;
		}

		void PushMergeTaskIfRequired(bool sync) {
			std::unique_lock ul{ chunksMutex };


			std::vector<KMergerChunk> ch{};
			if (!FindChunksToMerge(ch)) {
				return;
			}
			ul.unlock();

			auto& that = *this;

			PushTask("merge", [&that, ch] {
				size_t layer{};

				// compute max layer
				for (const auto& chk : ch) {
					if (chk.layer > layer) {
						layer = chk.layer;
					}
				}

				std::filesystem::path chunkLoc = that.GetNewIdSyncPath();
				// merge chunks into new chunk
				that.cfg.MergeChunks(ch, chunkLoc);

				// insert the new chunk
				{
					std::lock_guard lg{ that.chunksMutex };
					that.chunks.emplace_back(layer + 1, chunkLoc);
					if (layer + 1 > that.maxLayer) {
						that.maxLayer = layer + 1;
					}
				}

				// create a new merge task if this one is required
				that.PushMergeTaskIfRequired(true);
			}, sync);
		}

		void PushCreateChunkTask(bool sync) {
			auto& that = *this;
			PushTask("create", [&that] {
				std::filesystem::path chunkLoc = that.GetNewIdSyncPath();

				if (!that.cfg.CreateChunk(chunkLoc)) {
					std::lock_guard lg{ that.taskCondMutex };

					that.completed = true;
					// call the merge to force the full completion
					that.PushMergeTaskIfRequired(false);
					// chunk completed, we don't have to handle more than required. We close all the ones not working
					that.taskCondVar.notify_all();
					return;
				}

				// insert the new chunk
				{
					std::lock_guard lg{ that.chunksMutex };
					that.chunks.emplace_back(0, chunkLoc);
				}
				// create new chunk after
				that.PushCreateChunkTask(true);
				// maybe we need to merge the new chunk
				that.PushMergeTaskIfRequired(true);
			}, sync);


		}

		void HandleWorker() {
			KMergerTask task{ [] {}, "none" };

			try {
				while (!task.end) {
					task = PopTask();
					task.func();
				}
			}
			catch (std::exception& e) {
				{
					std::lock_guard lg{ exceptionMutex };
					exception = actslib::va("KMerger error in %s : %s", task.type, e.what());
				}
			
				std::lock_guard lg{ taskCondMutex };
				completed = true;
			
				tasks.clear(); // clear all tasks
			
				taskCondVar.notify_all();
			}
		}
	public:
		void Init() {
			if (!workers.empty()) {
				return; // already init, nothing to do
			}

			std::filesystem::create_directories(workdir);

			workers.reserve(workersCount);

			auto& that = *this;

			for (size_t i = 0; i < workersCount; i++) {
				workers.emplace_back([&that] { that.HandleWorker(); });
			}
		}

		std::filesystem::path PushAndJoin() {
			Init(); // Just in case

			PushCreateChunkTask(true);

			for (std::thread& th : workers) {
				th.join();
			}

			if (exception) {
				throw std::runtime_error(actslib::va("%s", exception));
			}

			if (chunks.size() > 1) {
				throw std::runtime_error("More than 1 chunk after a KMerger run");
			}

			if (chunks.empty()) {
				std::filesystem::path dp = GetNewIdSyncPath();
				cfg.CreateDefaultChunk(dp);
				return dp;
			}

			return chunks[0].file;
		}
	};


}