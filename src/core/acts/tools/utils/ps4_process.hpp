#pragma once

namespace utils::ps4 {
    enum PS4Notify : int32_t {
        PS4N_CONNECTED = 210,
        PS4N_MESSAGE = 222,
    };
	class PS4Process {
		libdebug::PS4DBG ps4;
        int32_t pid;
        uint64_t base{};

	public:
		PS4Process(const std::string& ipd, const char* process = "eboot.bin") : ps4(ipd) {
            ps4.Connect();

            auto procList = ps4.GetProcessList();
            auto proc = procList.FindProcess(process);

            if (!proc) {
                ps4.Disconnect();
                throw std::runtime_error("Can't connect to PS4 process");
            }
            pid = proc->pid;

            auto entries = ps4.GetProcessMaps(proc->pid);


            for (const auto& entry : entries.entries) {
                if (entry->prot == 5) {
                    base = entry->start;
                    break;
                }
            }

            if (!base) {
                ps4.Disconnect();
                throw std::runtime_error("Can't find executable base");
            }
		}
        ~PS4Process() {
            ps4.Disconnect();
        }

        uint64_t operator[](size_t delta) {
            return base + delta;
        }

        template<typename T>
        T Read(uint64_t location) {
            auto d = ps4.ReadMemory(pid, location, (int32_t)sizeof(T));
            return *reinterpret_cast<T*>(d.data());
        }

        template<typename T>
        std::unique_ptr<T> ReadObject(uint64_t location) {
            std::unique_ptr<T> ptr = std::make_unique<T>();
            auto d = ps4.ReadMemory(pid, location, (int32_t)sizeof(T));
            memcpy(ptr.get(), d.data(), sizeof(T));
            return ptr;
        }

        template<typename T>
        std::unique_ptr<T[]> ReadArray(uint64_t location, size_t len) {
            std::unique_ptr<T[]> ptr = std::make_unique<T[]>(len);
            auto d = ps4.ReadMemory(pid, location, (int32_t)(sizeof(T) * len));
            memcpy(ptr.get(), d.data(), sizeof(T) * len);
            return ptr;
        }

        std::string ReadString(uint64_t location, size_t buffLen = 1) {
            if (!location) return {};
            std::vector<char> buff{};

            while (true) {
                std::vector<byte> p = ps4.ReadMemory(pid, location, (int32_t)buffLen);

                for (byte b : p) {
                    buff.push_back((char)b);
                    if (!b) return buff.data();
                }
            }
        }
        
        std::vector<byte> ReadBuffer(uint64_t location, int32_t len) {
            return ps4.ReadMemory(pid, location, len);
        }

        void Write(uint64_t location, const void* ptr, size_t size) {
            std::vector<byte> data{};
            data.insert(data.begin(), (byte*)ptr, (byte*)ptr + size);
            ps4.WriteMemory(pid, location, data);
        }

        template<typename T>
        void Write(uint64_t location, const T& value) {
            Write(location, (const void*)&value, sizeof(value));
        }

        void Notify(int32_t type, const std::string& message) {
            ps4.Notify(type, message);
        }

        inline void Notify(const std::string& message) {
            Notify(utils::ps4::PS4N_MESSAGE, message);
        }

        libdebug::PS4DBG& Ps4Dbg() {
            return this->ps4;
        }

        constexpr int32_t Pid() const {
            return this->pid;
        }
	};


}