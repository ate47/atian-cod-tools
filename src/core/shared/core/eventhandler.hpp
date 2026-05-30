#pragma once
#include <utils/hash_mini.hpp>

namespace core::eventhandler {

	typedef std::function<void(void* data)> EventCallback;
	void RegisterEventCallback(uint64_t name, EventCallback callback);

	inline void RegisterEventCallback(uint64_t name, std::function<void()> callback) {
		RegisterEventCallback(name, [callback](void* data) { callback(); });
	}
	void RunEvent(uint64_t name, void* data = nullptr);


	template<typename EventData = void>
	class EventInfo {
		uint64_t name;
	public:
		constexpr EventInfo(uint64_t name) : name(name) {}
		constexpr EventInfo(const char* name) : name(hash::Hash64(name)) {}

		constexpr uint64_t GetName() const { return name; }

		template<typename CallbackType>
			requires std::is_same_v<EventData, void>
		inline void Callback(CallbackType&& callback) const {
			core::eventhandler::RegisterEventCallback(name, [cb = std::forward<CallbackType>(callback)](void* data) { cb(); });
		}

		template<typename CallbackType>
			requires (!std::is_same_v<EventData, void> && requires(CallbackType cb, EventData* e) { cb(e); })
		inline void Callback(CallbackType&& callback) const {
			core::eventhandler::RegisterEventCallback(name, [cb = std::forward<CallbackType>(callback)](void* data) { cb(reinterpret_cast<EventData*>(data)); });
		}

		template<typename CallbackType>
			requires (!std::is_same_v<EventData, void> && requires(CallbackType cb, EventData& e) { cb(e); })
		inline void Callback(CallbackType&& callback) const {
			core::eventhandler::RegisterEventCallback(name, [cb = std::forward<CallbackType>(callback)](void* data) { cb(*reinterpret_cast<EventData*>(data)); });
		}

		template<typename CallDataType, typename CDTD = std::decay_t<CallDataType>>
			requires (!std::is_same_v<EventData, void>)
		inline void Call(CallDataType&& data) const{
			if constexpr (std::is_same_v<EventData, CDTD>) {
				core::eventhandler::RunEvent(name, (void*)&data);
			}
			else if constexpr (std::is_same_v<EventData, std::remove_pointer_t<CDTD>>) {
				core::eventhandler::RunEvent(name, (void*)data);
			}
			else {
				static_assert(false, "Invalid CallDataType, not ref, data or pointer");
			}
		}

		inline void Call() const
			requires std::is_same_v<EventData, void> {
			core::eventhandler::RunEvent(name);
		}
	};
}