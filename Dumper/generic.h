#pragma once
#include <functional>
#include <Windows.h>
#include "types.h"

struct TArray {
	uint8* Data;
	uint32 Count;
	uint32 Max;
};

struct TNameEntryArray
{
	uint8** Chunks[128];
	uint32 NumElements;
	uint32 NumChunks;

	uint8* GetEntry(uint32 id) const;
	void Dump(std::function<void(std::string_view name, uint32 id)> callback) const;
};

struct TUObjectArray
{
	uint8* Objects;
	uint32 MaxElements;
	uint32 NumElements;

	uint8* GetObjectPtr(uint32 id) const;
	void Dump(std::function<void(byte*)> callback) const;
	class UE_UClass FindObject(const std::string& name) const;
};

typedef uint8* (__fastcall* Decrypt64)(uint64);
extern Decrypt64 DecryptClass;
extern Decrypt64 DecryptOuter;

typedef uint32(__fastcall* Decrypt32)(uint32);
extern Decrypt32 DecryptNameIndex;
extern Decrypt32 DecryptNameNumber;
extern Decrypt32 DecryptInternalIndex;

extern TNameEntryArray GlobalNames;
extern TUObjectArray ObjObjects;
