#include <includes.hpp>
#include <tools/gsc/gsc_injector.hpp>

// injectors
#include <tools/gsc/injector/gsc_injector_t8_36_pc.hpp>

namespace tool::gsc::injector {
	namespace {
		class GscInjectors {
			std::unordered_map<tool::gsc::opcode::VMId, std::unordered_map<tool::gsc::opcode::Platform, std::shared_ptr<GscInjector>>> injectors{};
		public:
			GscInjectors() {
				Register<GscInjectorT836PC>();
			}

			template<typename T>
			void Register() {
				std::shared_ptr<GscInjector> inj{ std::make_shared<T>() };
				injectors[inj->vm][inj->platform] = inj;
			}

			GscInjector* FindInjector(tool::gsc::opcode::VMId vm, tool::gsc::opcode::Platform platform) {
				auto it1{ injectors.find(vm) };
				if (it1 == injectors.end()) {
					return nullptr;
				}
				auto it2{ it1->second.find(platform) };
				if (it2 == it1->second.end()) {
					return nullptr;
				}

				return it2->second.get();
			}
		};

		GscInjectors injectors{};
	}

	GscInjector* FindInjector(tool::gsc::opcode::VMId vm, tool::gsc::opcode::Platform platform) {
		return injectors.FindInjector(vm, platform);
	}
}