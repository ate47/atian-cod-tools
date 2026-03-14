#pragma once
#include <core/memory_allocator.hpp>

namespace deps::idc_builder {
	struct CDeclDefinition {
		const char* value;
		const char* flags;
	};

	struct StructDefinition {
		const char* name;
		bool force;
	};

	struct EnumValueDefinition {
		const char* name;
		int64_t val;
	};

	struct EnumDefinition {
		const char* name;
		bool force;
		std::vector<EnumValueDefinition> values{};
	};

	struct AddressDefinition {
		size_t rva;
		const char* name;
		const char* type;
		const char* flags;
	};

	typedef size_t IdcEnumId;

	/*
	 * Get the rva inside a lib for a ptr
	 * @param loc loc
	 * @return rva
	 */
	size_t GetRva(void* loc);

	// builder to create .idc file (ida script file)
	class IdcBuilder {
		// allocator
		core::memory_allocator::MemoryAllocator alloc{};
		// member configuration
		std::vector<EnumDefinition> enums{};
		std::vector<StructDefinition> structs{};
		std::vector<CDeclDefinition> cdecls{};
		std::vector<AddressDefinition> addresses{};
		std::vector<const char*> headers{};
	public:
		IdcBuilder();
		IdcBuilder(const IdcBuilder& other) = delete;
		IdcBuilder(IdcBuilder&& other) = delete;

		// add a name to a rva, might have a typep
		void AddAddress(size_t rva, const char* name, const char* type = nullptr);
		// add a c declaration
		void AddCDecl(const char* decl, const char* flags = nullptr);
		// add a name to a rva, might have a type, the rva will obtained from deps::idc_builder::GetRva
		inline void AddAddress(void* loc, const char* name, const char* type = nullptr) { AddAddress(GetRva(loc), name, type); }
		// add a name to a rva, might have a type
		void AddAddressEx(size_t rva, const char* name, const char* flags, const char* type = nullptr);
		// add a name to a rva, might have a type, the rva will obtained from deps::idc_builder::GetRva
		inline void AddAddressEx(void* loc, const char* name, const char* flags, const char* type = nullptr) { AddAddressEx(GetRva(loc), name, flags, type); }
		// create an enum type
		IdcEnumId AddEnum(const char* name, bool force = false);
		// add member to an enum type
		void AddEnumMember(IdcEnumId id, const char* name, int64_t val);
		// add member to an enum type, the value will be its index in the members
		void AddEnumMember(IdcEnumId id, const char* name);
		// add header added before the main, useful for #define
		void AddHeader(const char* header);
		// add a struct name
		void AddStruct(const char* name, bool force = false);
		// get an enum def from an id
		EnumDefinition& GetEnumDef(IdcEnumId id);
		// write an idc from the configured information into a file
		void WriteIdcFile(std::filesystem::path out);
	};
}