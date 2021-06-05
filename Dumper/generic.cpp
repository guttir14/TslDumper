#include "wrappers.h"
#include "memory.h"

uint8* TNameEntryArray::GetEntry(uint32 id) const {
	uint32 ChunkIndex = id / 16836;
	uint32 WithinChunkIndex = id % 16836;
	uint8** Chunk = Chunks[ChunkIndex];
	return Read<uint8*>(Chunk + WithinChunkIndex);
}

void TNameEntryArray::Dump(std::function<void(std::string_view name, uint32 id)> callback) const {
	for (uint32 i = 0; i < NumElements; i++)
	{
		auto entry = UE_FNameEntry(GetEntry(i));
		if (!entry) { continue; }
		auto name = entry.String();
		callback(name, i);
	}
}

uint8* TUObjectArray::GetObjectPtr(uint32 id) const
{
	if (id >= NumElements) { return nullptr; }
	return Read<uint8*>(ObjObjects.Objects + id * 48 + 8);
}

void TUObjectArray::Dump(std::function<void(uint8*)> callback) const
{
	for (uint32 i = 0; i < NumElements; i++)
	{
		uint8* object = GetObjectPtr(i);
		if (!object) { continue; }
		callback(object);
	}
}

UE_UClass TUObjectArray::FindObject(const std::string& name) const
{
	for (uint32 i = 0; i < NumElements; i++)
	{
		UE_UClass object = GetObjectPtr(i);
		if (object && object.GetFullName() == name) { return object; }
	}
	return nullptr;
}

Decrypt32 DecryptNameIndex = nullptr;
Decrypt32 DecryptNameNumber = nullptr;
Decrypt32 DecryptInternalIndex = nullptr;
Decrypt64 DecryptClass = nullptr;
Decrypt64 DecryptOuter = nullptr;

TNameEntryArray GlobalNames;
TUObjectArray ObjObjects;
