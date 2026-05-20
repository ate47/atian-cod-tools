#include <includes.hpp>
#if __has_include(<CL/cl.h>)
#include <CL/cl.h>
#include <core/config.hpp>
#include <tools/hashes/hash_scanner.hpp>
#include <tools/hashes/text_expand.hpp>
#include <utils/data_utils.hpp>
#include <actslib/profiler.hpp>


namespace tool::hash::scanner {
	namespace {
        std::filesystem::path GetBaseDir() {
            std::string scanpath{ core::config::GetString("data.dir", "") };
            if (scanpath.empty()) {
                return utils::GetProgDir() / "data" / "opencl";
            }
            return std::filesystem::path{ scanpath } / "opencl";
        }

        void ReadData(const char* id, std::string& progData) {
            std::filesystem::path progFile{ GetBaseDir() / std::format("{}.cl", id) };

            if (!progData.empty()) {
                progData.append("\n");
            }
            if (!utils::ReadFile(progFile, progData, true)) {
                throw std::runtime_error(std::format("Can't read {}", progFile.string()));
            }
        }

        using CLProg = utils::TClosable<cl_program, cl_int, clReleaseProgram>;
        using CLKernel = utils::TClosable<cl_kernel, cl_int, clReleaseKernel>;
        using CLMem = utils::TClosable<cl_mem, cl_int, clReleaseMemObject>;

        class GPUData {
        public:
            cl_platform_id m_platform{};
            cl_device_id m_device{};
            cl_context m_context{};
            cl_command_queue m_queue{};
            GPUData() {
                cl_int err;

                cl_uint platformCount{};
                err = clGetPlatformIDs(0, nullptr, &platformCount);
                OpenCLAssert(err == CL_SUCCESS, "clGetPlatformIDs(count)");
                OpenCLAssert(platformCount != 0, "No OpenCL platforms found");

                std::unique_ptr<cl_platform_id[]> platforms{ std::make_unique<cl_platform_id[]>(platformCount) };
                err = clGetPlatformIDs(platformCount, platforms.get(), nullptr);
                OpenCLAssert(err == CL_SUCCESS, "clGetPlatformIDs(list)");

                m_platform = platforms[0];

                cl_uint deviceCount{};
                err = clGetDeviceIDs(m_platform, CL_DEVICE_TYPE_GPU, 0, nullptr, &deviceCount);
                OpenCLAssert(err == CL_SUCCESS, "clGetDeviceIDs(count)");
                OpenCLAssert(deviceCount != 0, "No OpenCL devices found");

                std::unique_ptr<cl_device_id[]> devices{ std::make_unique<cl_device_id[]>(deviceCount) };
                err = clGetDeviceIDs(m_platform, CL_DEVICE_TYPE_GPU, deviceCount, devices.get(), nullptr);
                OpenCLAssert(err == CL_SUCCESS, "clGetDeviceIDs(list)");

                m_device = devices[0];

                m_context = clCreateContext(nullptr, 1, &m_device, nullptr, nullptr, &err);
                OpenCLAssert(err == CL_SUCCESS, "clCreateContext");
                m_queue = clCreateCommandQueueWithProperties(m_context, m_device, nullptr, &err);
                OpenCLAssert(err == CL_SUCCESS, "clCreateCommandQueue");
            }

            GPUData(GPUData&& other) = delete;
            GPUData(const GPUData& other) = delete;
            ~GPUData() {
                if (m_queue) {
                    clReleaseCommandQueue(m_queue);
                }
                if (m_context) {
                    clReleaseContext(m_context);
                }
            }

            CLProg CreateProgramWithSource(std::string& progData) {
                cl_int err;

                const char* src{ progData.data() };
                size_t len{ progData.size() };

                cl_program program = clCreateProgramWithSource(m_context, 1, &src, &len, &err);
                OpenCLAssert(err == CL_SUCCESS, "building program");

                err = clBuildProgram(program, 1, &m_device, nullptr, nullptr, nullptr);
                if (err != CL_SUCCESS) {
                    size_t logSize{};
                    clGetProgramBuildInfo(program, m_device, CL_PROGRAM_BUILD_LOG, 0, nullptr, &logSize);
                    std::string log{};
                    log.resize(logSize);
                    clGetProgramBuildInfo(program, m_device, CL_PROGRAM_BUILD_LOG, logSize, log.data(), nullptr);

                    clReleaseProgram(program);
                    throw std::runtime_error(std::format("OpenCL build error: {} ({})", log, err));
                }
                return { program };
            }

            CLMem CreateBuffer(cl_mem_flags flags, size_t len) {
                cl_int err;
                cl_mem mem{ clCreateBuffer(m_context, flags, len, nullptr, &err) };
                OpenCLAssert(err == CL_SUCCESS, "failed to allocate buffer");
                return { mem };
            }

            CLMem CreateBuffer(cl_mem_flags flags, const void* data, size_t len) {
                CLMem mem{ CreateBuffer(flags, len)};
                cl_int err;
                err = clEnqueueWriteBuffer(m_queue, *mem, CL_TRUE, 0, len, data, 0, nullptr, nullptr);
                OpenCLAssert(err == CL_SUCCESS, "failed to write buffer");
                return mem;
            }

            CLMem CreateBuffer(cl_mem_flags flags, std::vector<byte>& data) {
                return CreateBuffer(flags, data.data(), data.size());
            }

            CLMem CreateBuffer(cl_mem_flags flags, std::string& data) {
                return CreateBuffer(flags, data.data(), data.size() + 1);
            }

            CLMem CreateBuffer(cl_mem_flags flags, const char* str) {
                return CreateBuffer(flags, str, std::strlen(str) + 1);
            }

            CLKernel CreateKernel(CLProg& prog, const char* name) {
                cl_int err;
                cl_kernel kernel{ clCreateKernel(*prog, name, &err) };
                OpenCLAssert(err == CL_SUCCESS, "failed to create kernel '%s'", name);
                return { kernel };
            }

            void SetKernelArg(CLKernel& kernel, cl_int argIndex, size_t argSize, const void* argValue) {
                cl_int err;
                err = clSetKernelArg(*kernel, argIndex, argSize, argValue);
                OpenCLAssert(err == CL_SUCCESS, "failed set kernel arg %d", argIndex);
            }

            void EnqueueNDRangeKernel(CLKernel& kernel, size_t workDim, size_t* globalWorkOffset, size_t* globalWork) {
                EnqueueNDRangeKernelEvent(kernel, workDim, globalWorkOffset, globalWork, 0, nullptr, nullptr);
            }

            void EnqueueNDRangeKernelEvent(CLKernel& kernel, size_t workDim, size_t* globalWorkOffset, size_t* globalWork, size_t waitEventsCount, const cl_event* waitEvents, cl_event* event) {
                cl_int err;
                err = clEnqueueNDRangeKernel(m_queue, *kernel, workDim, globalWorkOffset, globalWork, nullptr, waitEventsCount, waitEvents, event);
                OpenCLAssert(err == CL_SUCCESS, "failed to enqueue nd range kernel");
            }

            void EnqueueReadBuffer(CLMem& buffer, bool block, void* data, size_t len) {
                EnqueueReadBufferEvent(buffer, block, data, len, 0, nullptr, nullptr);
            }
            void EnqueueReadBufferEvent(CLMem& buffer, bool block, void* data, size_t len, size_t waitEventsCount, const cl_event* waitEvents, cl_event* event) {
                cl_int err;
                err = clEnqueueReadBuffer(m_queue, *buffer, block ? CL_TRUE : CL_FALSE, 0, len, data, waitEventsCount, waitEvents, event);
                OpenCLAssert(err == CL_SUCCESS, "failed to read buffer");
            }

            void OpenCLAssert(bool val, const char* msg, ...) {
                va_list va;
                va_start(va, msg);
                if (!val) {
                    throw std::runtime_error(std::format("OpenCL error: {}", utils::vap(msg, va)));
                }
                va_end(va);
            }
        };

        void PackMap(std::filesystem::path path, std::vector<byte>& map, size_t indexSize, std::unordered_set<uint64_t>& hashes) {
            std::vector<std::filesystem::path> files{ GetHashFiles(path) };
            LOG_TRACE("{} file(s) loaded...", files.size());

            ScanHashes(files, hashes, ::hash::MASK60);

            LOG_INFO("Find {} hash(es)", hashes.size());

            std::unique_ptr<std::vector<uint64_t>[]> base{ std::make_unique<std::vector<uint64_t>[]>(indexSize) };
            for (uint64_t hash : hashes) {
                base[hash & (indexSize - 1)].push_back(hash);
            }

            // buckets
            utils::Allocate(map, sizeof(uint64_t) * indexSize * 2);

            size_t count{};
            for (size_t i = 0; i < indexSize; i++) {
                std::vector<uint64_t>& v{ base[i] };
                std::sort(v.begin(), v.end());

                uint64_t* idx{ (uint64_t*)map.data() + i * 2 };
                idx[0] = count; // start
                idx[1] = count += v.size(); // end
                utils::WriteValue(map, v.data(), v.size() * sizeof(v[0]));
            }
        }
        void PackDict(std::filesystem::path path, std::vector<byte>& packedDictIndex, std::string& packedDictData, std::vector<const char*>& dictVec) {
            dictVec = ReadDict(path, packedDictData);

            // offsets
            uint64_t* arr{ utils::AllocateArray<uint64_t>(packedDictIndex, dictVec.size()) };
            for (const char* s : dictVec) {
                *arr = s - packedDictData.data();
                arr++;
            }
        }

        class GPUHashData : public AbstractHashData {
        public:
            std::unordered_set<uint64_t> hashes{};

            using AbstractHashData::AbstractHashData;

            int TestHash(uint64_t val, const char* str) {
                if (hashes.contains(val & ::hash::MASK60)) {
                    output << std::hex << val << "," << str << "\n";
                    LOG_INFO("{:x},{}", val, str);
                    return 1;
                }
                return 0;
            }

            size_t TestHash(const char* str) {
                size_t t{};
                if (UseFunc(HASH_FNVA)) t += TestHash(::hash::Hash64A(str), str);
                if (UseFunc(HASH_FNVA32)) t += TestHash(::hash::HashX32(str), str);
                if (UseFunc(HASH_PRIME)) t += TestHash(::hash::HashPrime(str), str);
                if (UseFunc(HASH_DJB2)) t += TestHash(::hash::HashDJB2(str), str);
                if (UseFunc(HASH_SCR_T10)) t += TestHash(::hash::HashT10Scr(str), str);
                if (UseFunc(HASH_SCR_T10_SP)) t += TestHash(::hash::HashT10ScrSP(str), str);
                if (UseFunc(HASH_OMNVAR)) t += TestHash(::hash::HashT10OmnVar(str), str);
                if (UseFunc(HASH_SCR_T89)) t += TestHash(::hash::HashT89Scr(str), str);
                if (UseFunc(HASH_SCR_JUP)) t += TestHash(::hash::HashJupScr(str), str);
                if (UseFunc(HASH_RES)) t += TestHash(::hash::HashIWAsset(str), str);
                if (UseFunc(HASH_DVAR)) t += TestHash(::hash::HashIWDVar(str), str);
                if (UseFunc(HASH_T7)) t += TestHash(::hash::HashT7(str), str);
                if (UseFunc(HASH_KVP)) t += TestHash(::hash::HashKVP(str), str);
                return t;
            }
        };

        size_t RebuildWord(
            uint64_t index,
            const std::vector<const char*>& dictVec,
            const char* prefix,
            const char* middle,
            const char* suffix,
            char* outBuf,
            size_t outBufSize) {
            size_t pos = 0;
            const size_t wordsCount = dictVec.size();

            // 1. Prefix
            if (prefix) {
                for (const char* p = prefix; *p && pos < outBufSize - 1; ++p) {
                    outBuf[pos++] = *p;
                }
            }

            // 2. Combinator expansion (base-N)
            bool first = true;
            uint64_t x = index;

            while (x > 0) {
                uint64_t j = x - 1;
                uint64_t idx = j % wordsCount;
                x = j / wordsCount;

                // Insert middle between parts (but not before first)
                if (!first && middle) {
                    for (const char* m = middle; *m && pos < outBufSize - 1; ++m) {
                        outBuf[pos++] = *m;
                    }
                }
                first = false;

                // Append dictionary word
                const char* w = dictVec[idx];
                while (*w && pos < outBufSize - 1) {
                    outBuf[pos++] = *w++;
                }
            }

            // 3. Suffix
            if (suffix) {
                for (const char* s = suffix; *s && pos < outBufSize - 1; ++s) {
                    outBuf[pos++] = *s;
                }
            }

            // Null-terminate
            outBuf[pos] = '\0';
            return pos;
        }


		int hashbrutedictgpu(int argc, const char* argv[]) {
			if (tool::NotEnoughParam(argc, 4)) {
				return tool::BAD_USAGE;
			}


            const char* map{ argv[2] };
            GPUHashData hashData{ argv[3], argv[4] };
            const char* dict{ argv[5] };
            const char* prefix{ nullptr };
            if (!tool::NotEnoughParam(argc, 5)) {
                prefix = argv[6];
                LOG_INFO("prefix: {}", prefix);
            }
            const char* suffix{ nullptr };
            if (!tool::NotEnoughParam(argc, 6)) {
                suffix = argv[7];
                LOG_INFO("suffix: {}", suffix);
            }
            const char* mid{ "_" };
            if (!tool::NotEnoughParam(argc, 7)) {
                mid = argv[8];
                LOG_INFO("middle: {}", mid);
            }
            size_t count{ tool::NotEnoughParam(argc, 8) ? std::string::npos : utils::ParseFormatInt(argv[9]) };

            std::vector<byte> packedMap{};
            std::vector<byte> packedDictIndex{};
            std::string packedDictData{};
            std::vector<const char*> dictVec{};
            cl_uint indexSize{ 0x1000 };
            constexpr size_t hashesPerWork = 0x800000;
            PackMap(map, packedMap, indexSize, hashData.hashes);
            PackDict(dict, packedDictIndex, packedDictData, dictVec);
            cl_uint wordsCount{(cl_uint) dictVec.size()};

            size_t maxLen{ std::string::npos };
            if (count != std::string::npos && count) {
                maxLen = 1;
                for (size_t i = 0; i < count; i++) {
                    maxLen *= dictVec.size();
                }
                maxLen++;
                LOG_INFO("count: {}, (max {})", count, maxLen);
            }

            GPUData gpu{};

            CLMem gpuMap{ gpu.CreateBuffer(CL_MEM_READ_ONLY, packedMap) };
            CLMem gpuDictIndex{ gpu.CreateBuffer(CL_MEM_READ_ONLY, packedDictIndex) };
            CLMem gpuDictData{ gpu.CreateBuffer(CL_MEM_READ_ONLY, packedDictData) };

            CLMem gpuOutBufferA{ gpu.CreateBuffer(CL_MEM_WRITE_ONLY, hashesPerWork * sizeof(cl_ulong)) };
            CLMem gpuOutBufferB{ gpu.CreateBuffer(CL_MEM_WRITE_ONLY, hashesPerWork * sizeof(cl_ulong)) };
            std::unique_ptr<cl_ulong[]> outBufferHost{ std::make_unique<cl_ulong[]>(hashesPerWork) };

            CLMem gpuPrefix{ prefix && *prefix ? gpu.CreateBuffer(CL_MEM_READ_ONLY, prefix) : nullptr };
            CLMem gpuMiddle{ mid && *mid ? gpu.CreateBuffer(CL_MEM_READ_ONLY, mid) : nullptr };
            CLMem gpuSuffix{ suffix && *suffix ? gpu.CreateBuffer(CL_MEM_READ_ONLY, suffix) : nullptr };


            std::string progData{};
            ReadData("hashbrutegpu", progData);
            CLProg prog{ gpu.CreateProgramWithSource(progData) };
            CLKernel bruteKernel{ gpu.CreateKernel(prog, "hash_brute_dict") };

            cl_ulong algoMask{ (cl_ulong)hashData.funcs };

            // 0 hashes to compute            
            gpu.SetKernelArg(bruteKernel, 0, sizeof(*gpuMap), &*gpuMap);
            // 1 dictionary offsets in dictData            
            gpu.SetKernelArg(bruteKernel, 1, sizeof(*gpuDictIndex), &*gpuDictIndex);
            // 2 dictionary data            
            gpu.SetKernelArg(bruteKernel, 2, sizeof(*gpuDictData), &*gpuDictData);
            // 3 prefix string            
            gpu.SetKernelArg(bruteKernel, 3, sizeof(*gpuPrefix), &*gpuPrefix);
            // 4 middle string            
            gpu.SetKernelArg(bruteKernel, 4, sizeof(*gpuMiddle), &*gpuMiddle);
            // 5 suffix string            
            gpu.SetKernelArg(bruteKernel, 5, sizeof(*gpuSuffix), &*gpuSuffix);
            // 8 wordsCount
            gpu.SetKernelArg(bruteKernel, 8, sizeof(wordsCount), &wordsCount);
            // 9 indexSize
            gpu.SetKernelArg(bruteKernel, 9, sizeof(indexSize), &indexSize);
            // 10 algoMask
            gpu.SetKernelArg(bruteKernel, 10, sizeof(algoMask), &algoMask);

            cl_ulong startIndex{};
            bool useOutA{};
            size_t global{ hashesPerWork };
            constexpr size_t outBufferHostSize = hashesPerWork * sizeof(outBufferHost[0]);
            cl_event kernelEvent{};
            cl_event readEvent{};
            cl_event prevReadEvent{};

            char wordBuff[0x200];
            actslib::profiler::Profiler profiler{ "hashbrutegpu" };
            profiler.Reset();
            size_t total{};

            while (true) {
                if (HAS_LOG_LEVEL(core::logs::LVL_TRACE)) {
                    wordBuff[0] = 0;
                    RebuildWord(startIndex, dictVec, prefix, mid, suffix, wordBuff, sizeof(wordBuff));
                    LOG_TRACE("{} (0x{:x}+0x{:x}) {}", startIndex, startIndex, global, wordBuff);
                }
                if (startIndex < maxLen) {
                    // pick buffer
                    cl_mem outBuffer = useOutA ? *gpuOutBufferA : *gpuOutBufferB;

                    // 6 out data
                    gpu.SetKernelArg(bruteKernel, 6, sizeof(outBuffer), &outBuffer);
                    // 7 startIndex
                    gpu.SetKernelArg(bruteKernel, 7, sizeof(startIndex), &startIndex);

                    // enqueue
                    gpu.EnqueueNDRangeKernelEvent(bruteKernel, 1, nullptr, &global, 0, nullptr, &kernelEvent);

                }
                if (startIndex) {
                    CLMem& prevBuf = useOutA ? gpuOutBufferB : gpuOutBufferA;
                    // this was done during the last iteration, so we need to remove global to get the previous start index

                    gpu.EnqueueReadBufferEvent(prevBuf, false, outBufferHost.get(), outBufferHostSize, 1, &prevReadEvent, &readEvent);

                    clWaitForEvents(1, &readEvent);

                    for (size_t i = 0; i < hashesPerWork; i++) {
                        if (!outBufferHost[i]) {
                            continue; // nothing
                        }
                        // -1 because 0 is bad index
                        cl_ulong idx{ outBufferHost[i] - 1 };
                        RebuildWord(idx, dictVec, prefix, mid, suffix, wordBuff, sizeof(wordBuff));
                        // we know that wordBuff is a string of a known hash, we need to find which one(s)

                        size_t found{ hashData.TestHash(wordBuff) };
                        if (!found) {
                            LOG_ERROR("CAN'T FIND RETURNED '{}', index={}:{}/{}", wordBuff, idx, i, hashesPerWork);
                        }
                        total += found;
                    }
                }
                if (prevReadEvent) {
                    clReleaseEvent(prevReadEvent);
                }
                prevReadEvent = kernelEvent;

                if (startIndex >= maxLen) {
                    break;
                }
                // goto next blocks
                startIndex += global;
                // swap buffers
                useOutA = !useOutA;
            }
            profiler.Stop();
            LOG_INFO("found {} string(s) with {} hashes in {}ms {:.2}ns/hash", total, startIndex, profiler.GetMainSection().GetMillis(),
                profiler.GetMainSection().GetMillis() * 1000000.0 / startIndex);

			return tool::OK;
		}


        size_t RebuildWordLetters(
            uint64_t index,
            const char* dictVec,
            const char* prefix,
            const char* suffix,
            size_t lettersCount,
            char* outBuf,
            size_t outBufSize) {
            size_t pos = 0;

            // 1. Prefix
            if (prefix) {
                for (const char* p = prefix; *p && pos < outBufSize - 1; ++p) {
                    outBuf[pos++] = *p;
                }
            }

            // 2. Combinator expansion (base-N)
            uint64_t x = index;

            while (x > 0) {
                uint64_t j = x - 1;
                uint64_t idx = j % lettersCount;
                x = j / lettersCount;

                // Append dictionary word
                if (pos < outBufSize - 1) {
                    outBuf[pos++] = dictVec[idx];
                }
            }

            // 3. Suffix
            if (suffix) {
                for (const char* s = suffix; *s && pos < outBufSize - 1; ++s) {
                    outBuf[pos++] = *s;
                }
            }

            // Null-terminate
            outBuf[pos] = '\0';
            return pos;
        }


        int hashbrutegpu(int argc, const char* argv[]) {
            if (tool::NotEnoughParam(argc, 3)) {
                return tool::BAD_USAGE;
            }


            const char* map{ argv[2] };
            GPUHashData hashData{ argv[3], argv[4] };
            const char* prefix{ nullptr };
            if (!tool::NotEnoughParam(argc, 4)) {
                prefix = argv[5];
                LOG_INFO("prefix: {}", prefix);
            }
            const char* suffix{ nullptr };
            if (!tool::NotEnoughParam(argc, 5)) {
                suffix = argv[6];
                LOG_INFO("suffix: {}", suffix);
            }
            size_t count{ tool::NotEnoughParam(argc, 6) ? std::string::npos : utils::ParseFormatInt(argv[7]) };
            const char* dict{ tool::NotEnoughParam(argc, 7) ? hash::text_expand::dict_alpha_number : argv[8] };

            std::vector<byte> packedMap{};
            std::vector<byte> packedDictIndex{};
            cl_uint indexSize{ 0x1000 };
            constexpr size_t hashesPerWork = 0x1000000;
            PackMap(map, packedMap, indexSize, hashData.hashes);
            cl_uint lettersCount{ (cl_uint)std::strlen(dict) };

            size_t maxLen{ std::string::npos };
            if (count != std::string::npos && count) {
                maxLen = 1;
                for (size_t i = 0; i < count; i++) {
                    maxLen *= lettersCount;
                }
                maxLen++;
                LOG_INFO("count: {}, (max {})", count, maxLen);
            }

            GPUData gpu{};

            CLMem gpuMap{ gpu.CreateBuffer(CL_MEM_READ_ONLY, packedMap) };
            CLMem gpuDict{ gpu.CreateBuffer(CL_MEM_READ_ONLY, dict) };

            CLMem gpuOutBufferA{ gpu.CreateBuffer(CL_MEM_WRITE_ONLY, hashesPerWork * sizeof(cl_ulong)) };
            CLMem gpuOutBufferB{ gpu.CreateBuffer(CL_MEM_WRITE_ONLY, hashesPerWork * sizeof(cl_ulong)) };
            std::unique_ptr<cl_ulong[]> outBufferHost{ std::make_unique<cl_ulong[]>(hashesPerWork) };

            CLMem gpuPrefix{ prefix && *prefix ? gpu.CreateBuffer(CL_MEM_READ_ONLY, prefix) : nullptr };
            CLMem gpuSuffix{ suffix && *suffix ? gpu.CreateBuffer(CL_MEM_READ_ONLY, suffix) : nullptr };


            std::string progData{};
            ReadData("hashbrutegpu", progData);
            CLProg prog{ gpu.CreateProgramWithSource(progData) };
            CLKernel bruteKernel{ gpu.CreateKernel(prog, "hash_brute") };

            cl_ulong algoMask{ (cl_ulong)hashData.funcs };

            // 0 hashes to compute
            gpu.SetKernelArg(bruteKernel, 0, sizeof(*gpuMap), &*gpuMap);
            // 1 dictionary data
            gpu.SetKernelArg(bruteKernel, 1, sizeof(*gpuDict), &*gpuDict);
            // 2 prefix string
            gpu.SetKernelArg(bruteKernel, 2, sizeof(*gpuPrefix), &*gpuPrefix);
            // 3 suffix string
            gpu.SetKernelArg(bruteKernel, 3, sizeof(*gpuSuffix), &*gpuSuffix);
            // 4 out data
            // 5 startIndex
            // 6 lettersCount
            gpu.SetKernelArg(bruteKernel, 6, sizeof(lettersCount), &lettersCount);
            // 7 indexSize
            gpu.SetKernelArg(bruteKernel, 7, sizeof(indexSize), &indexSize);
            // 8 algoMask
            gpu.SetKernelArg(bruteKernel, 8, sizeof(algoMask), &algoMask);

            cl_ulong startIndex{};
            bool useOutA{};
            size_t global{ hashesPerWork };
            constexpr size_t outBufferHostSize = hashesPerWork * sizeof(outBufferHost[0]);
            cl_event kernelEvent{};
            cl_event readEvent{};
            cl_event prevReadEvent{};

            char wordBuff[0x200];
            actslib::profiler::Profiler profiler{ "hashbrutegpu" };
            profiler.Reset();
            size_t total{};

            while (true) {
                if (HAS_LOG_LEVEL(core::logs::LVL_TRACE)) {
                    wordBuff[0] = 0;
                    RebuildWordLetters(startIndex, dict, prefix, suffix, lettersCount, wordBuff, sizeof(wordBuff));
                    LOG_TRACE("{} (0x{:x}+0x{:x}) {}", startIndex, startIndex, global, wordBuff);
                }
                if (startIndex < maxLen) {
                    // pick buffer
                    cl_mem outBuffer = useOutA ? *gpuOutBufferA : *gpuOutBufferB;

                    // 4 out data
                    gpu.SetKernelArg(bruteKernel, 4, sizeof(outBuffer), &outBuffer);
                    // 5 startIndex
                    gpu.SetKernelArg(bruteKernel, 5, sizeof(startIndex), &startIndex);

                    // enqueue
                    gpu.EnqueueNDRangeKernelEvent(bruteKernel, 1, nullptr, &global, 0, nullptr, &kernelEvent);

                }
                if (startIndex) {
                    CLMem& prevBuf = useOutA ? gpuOutBufferB : gpuOutBufferA;
                    // this was done during the last iteration, so we need to remove global to get the previous start index

                    gpu.EnqueueReadBufferEvent(prevBuf, false, outBufferHost.get(), outBufferHostSize, 1, &prevReadEvent, &readEvent);

                    clWaitForEvents(1, &readEvent);

                    for (size_t i = 0; i < hashesPerWork; i++) {
                        if (!outBufferHost[i]) {
                            continue; // nothing
                        }
                        // -1 because 0 is bad index
                        cl_ulong idx{ outBufferHost[i] - 1 };
                        RebuildWordLetters(idx, dict, prefix, suffix, lettersCount, wordBuff, sizeof(wordBuff));
                        // we know that wordBuff is a string of a known hash, we need to find which one(s)

                        size_t found{ hashData.TestHash(wordBuff) };
                        if (!found) {
                            LOG_ERROR("CAN'T FIND RETURNED '{}', index={}:{}/{}", wordBuff, idx, i, hashesPerWork);
                        }
                        total += found;
                    }
                }
                if (prevReadEvent) {
                    clReleaseEvent(prevReadEvent);
                }
                prevReadEvent = kernelEvent;

                if (startIndex >= maxLen) {
                    break;
                }
                // goto next blocks
                startIndex += global;
                // swap buffers
                useOutA = !useOutA;
            }
            profiler.Stop();
            LOG_INFO("found {} string(s) with {} hashes in {}ms {:.2}ns/hash", total, startIndex, profiler.GetMainSection().GetMillis(),
                profiler.GetMainSection().GetMillis() * 1000000.0 / startIndex);

            return tool::OK;
        }

        int gputest(int argc, const char* argv[]) {
            GPUData gpu{};

            std::string progData{};
            ReadData("test", progData);
            CLProg testProg{ gpu.CreateProgramWithSource(progData) };
            CLKernel kernel{ gpu.CreateKernel(testProg, "test") };
            CLMem buffer{ gpu.CreateBuffer(CL_MEM_WRITE_ONLY, sizeof(int)) };

            // Arg
            gpu.SetKernelArg(kernel, 0, sizeof(cl_mem), &*buffer);

            // Launch
            size_t global{ 1 };
            gpu.EnqueueNDRangeKernel(kernel, 1, nullptr, &global);

            // Read back
            int result{};
            gpu.EnqueueReadBuffer(buffer, true, &result, sizeof(result));

            LOG_INFO("Kernel returned value: {}", result);
            return tool::OK;
        }


        // acts hashbrutegpu .\output_ff\bo7\source\scripts\ .\output_bo7\brute.txt bo6 .\output_bo6\dict.txt "" ""
		ADD_TOOL(hashbrutedictgpu, "hash", " [dir] [output] [algorithm] [dict] (prefix=) (suffix=) (middle=_) (count=infinity)", "brute search hashes in a directory with dictionary with GPU", hashbrutedictgpu);
		ADD_TOOL(hashbrutegpu, "hash", " [dir] [output] [algorithm] (prefix=) (suffix=) (count=infinity) (dict=all)", "brute search hashes in a directory with dictionary with GPU", hashbrutegpu);
		ADD_TOOL(gputest, "dev", "", "test gpu", gputest);
	}
}
#endif // __has_include(<CL/cl.h>)
