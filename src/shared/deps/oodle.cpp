#include <includes_shared.hpp>
#include "oodle.hpp"


namespace deps::oodle {

	Oodle& GetInstance() {
		static Oodle oodle{};
		static std::once_flag of{};

		std::call_once(of, [] {
			if (!oodle.LoadAny()) {
				throw std::runtime_error("Can't load oodle");
			}
		});

		return oodle;
	}


}