#pragma once

namespace clisync {


	enum CliSyncDataFeature : UINT64{
		FEATURE_LOG = 0x1
	};

	struct CliSyncData {
		// location of the exec workspace
		CHAR workDir[MAX_PATH + 1];
		// location of the exec
		CHAR execDir[MAX_PATH + 1];
		// feature field, described in CliSyncDataFeature
		UINT64 features;


		inline bool HasFeature(CliSyncDataFeature feature) {
			return (features & feature) != 0;
		}
	};

}