#include "engine.h"
#include "wrappers.h"
#include "utils.h"
#include "memory.h"

Offsets defs;

bool TlsInit(std::vector<std::pair<byte*, byte*>>* sections) {

	void* decryptPtrAddr = nullptr;
	void* globalNamesPtrPtrAddr = nullptr;
	void* objObjectsAddr = nullptr;
	

	{
		uint8 decryptPtrSig[] = { 0xFF, 0x15, 0x00, 0x00, 0x00, 0x00, 0xEB, 0x3F };
		uint8 namesSig[] = { 0x75, 0x1E, 0x48, 0x8B, 0x15, 0x00, 0x00, 0x00, 0x00, 0xB9 };
		uint8 objSig[] = { 0x85, 0xED, 0x7E, 0x10, 0x48, 0x8B, 0xD6, 0x48, 0x8D, 0x0D, 0x00, 0x00, 0x00, 0x00, 0xE8, 0x00, 0x00, 0x00, 0x00, 0x90 };

		char flags = 0;
		for (int i = 0; i < sections->size(); i++) {
			auto& info = sections->at(i);
			if (!(flags & 1)) { decryptPtrAddr = FindPointer(info.first, info.second, decryptPtrSig, sizeof(decryptPtrSig)); flags |= 1;}
			if (!(flags & 2)) { globalNamesPtrPtrAddr = FindPointer(info.first, info.second, namesSig, sizeof(namesSig)); flags |= 2; }
			if (!(flags & 4)) { objObjectsAddr = FindPointer(info.first, info.second, objSig, sizeof(objSig), 0x18); flags |= 4; }
			if (flags & 7) { goto success; }
		}

		return false;
	}

	success:
	
	DecryptNameIndex = [](uint32_t v25) {
		v25 = _rotr(v25 ^ 0x1276F7AB, 4);
		return v25 ^ (v25 << 16) ^ 0xAD932E4;
	};

	DecryptNameNumber = [](uint32_t v26) {
		auto v39 = _rotr(v26 ^ 0x1F38C2DD, 12);
		return v39 ^ (v39 << 16) ^ 0xD90A4EE8;
	};


	DecryptInternalIndex = [](uint32 a1) {
		auto v37 = _rotl(a1 ^ 0xF7AB1276, 12);
		return v37 ^ (v37 << 16) ^ 0xAD9383D;
	};
	
	DecryptClass = [](uint64_t address) {
		auto v120 = _rotr64(address ^ 0xB2E43838241BE450, 14);
		return (byte*)(v120 ^ (v120 << 32) ^ 0xAD550BA6BAFE19D1);
	};

	DecryptOuter = [](uint64_t address) {
		auto v13 = _rotl64(address ^ 0x5C97626CEC6E8C9E, 24);
		return (uint8*)(v13 ^ (v13 << 32) ^ 0x590861ED1C7E099F);
	};

	// Building Decrypt from game memory
	typedef uint64(__fastcall* decrypt_t)(uint64);
	decrypt_t decrypt;
	{
		uint8 buffer[50]{};
		auto decryptPtr = *(uint64_t*)(decryptPtrAddr);
		Read((void*)decryptPtr, buffer, 50);
		auto decryptVar = decryptPtr + *(int32_t*)(buffer + 3) + 7;
		decrypt = (decrypt_t)VirtualAlloc(NULL, 50, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
		if (!decrypt) return false;
		((uint16_t*)decrypt)[0] = 0xb848;
		*(uint64_t*)((uint8*)(decrypt)+2) = decryptVar;
		memcpy(&((uint8*)decrypt)[0xA], &buffer[0xA], 40);
	}

	// Initialization for GlobalNames
	{
		auto v1 = *(uint64_t*)globalNamesPtrPtrAddr;
		auto v2 = decrypt(v1);
		struct {
			uint64_t Ptr = 0;
			uint64_t NumElements = 0;
			uint64_t NumChunks = 0;
		} data;


		auto v3 = decrypt(Read<uint64>((void*)v2));
		auto v4 = decrypt(Read<uint64>((void*)v3));
		Read((void*)(v4), &data, 0x18);

		// Getting nums
		GlobalNames.NumElements = decrypt(data.NumElements);
		GlobalNames.NumChunks = decrypt(data.NumChunks);

		// Filling chunks
		auto v5 = decrypt(Read<uint64>((void*)v4));
		Read((void*)v5, &GlobalNames, sizeof(GlobalNames) - 8);
	}

	{
		struct _t {
			uint64_t ObjObjects = 0; // 0x18
			uint64_t MaxElements = 0; // 0x20
			uint32_t NumElements = 0; // 0x28
		};

		auto data = (_t*)objObjectsAddr;
		ObjObjects.Objects = (uint8*)decrypt(data->ObjObjects);
		ObjObjects.MaxElements = decrypt(data->MaxElements);
		ObjObjects.NumElements = data->NumElements;
	}

	VirtualFree(decrypt, 0, MEM_RELEASE);

	return true;
}

bool EngineInit(std::string game, std::vector<std::pair<byte*, byte*>>* sections) {
	union
	{
		uint64 val64;
		struct {
			int a;
			int b;
		} val32;
	} name;
	name.val32 = {'GlsT', '\0ema' };

	if (*(uint64*)game.data() != name.val64 || !TlsInit(sections)) return false;
	return true;
}
