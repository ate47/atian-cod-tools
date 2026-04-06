#pragma once
#include <acts_api.hpp>

namespace acts::api {

	class ActsAPIVersion {
	public:
        virtual unsigned int GetBuildVersion() = 0;
        virtual const char* GetVersion() = 0;
        virtual unsigned int GetVersionId() = 0;
	};

}

ACTS_COMMON_API acts::api::ActsAPIVersion& ActsAPIVersion();