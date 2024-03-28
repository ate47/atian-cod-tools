#pragma once

namespace clisync {


	enum CliSyncDataFeature : uint64_t{
		FEATURE_LOG = 0x1
	};

	struct CliSyncData {
		// location of the exec workspace
		char workDir[MAX_PATH + 1];
		// location of the exec
		char execDir[MAX_PATH + 1];
		// feature field, described in CliSyncDataFeature
		uint64_t features;


		inline bool HasFeature(CliSyncDataFeature feature) {
			return (features & feature) != 0;
		}
	};

}