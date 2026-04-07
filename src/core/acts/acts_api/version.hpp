#pragma once
#include "api.hpp"
/*
 * Atian Tools Version API
 */

namespace acts::api {

	class ActsAPIVersion {
	public:
		// @return build version
        virtual unsigned int GetBuildVersion() = 0;
		// @return version string
        virtual const char* GetVersion() = 0;
		// @return version id
        virtual unsigned int GetVersionId() = 0;
	};

}

// @return version api
ACTS_COMMON_API acts::api::ActsAPIVersion& ActsAPIVersion();