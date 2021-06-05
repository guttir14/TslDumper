#pragma once
#include <functional>
#include <windows.h>

struct Offsets {
	struct {
		uint16_t HeaderSize = 0x10;
	} FNameEntry;
	struct {
		uint16_t Index = 0x8; // +
		uint16_t Class = 0x30; // +
		uint16_t Name = 0x1C; // +
		uint16_t Outer = 0x28; // 
	} UObject;
	struct {
		uint16_t Next = 0x38; // 
	} UField;
	struct {
		uint16_t SuperStruct = 0x70; // 
		uint16_t Children = 0x100; // 
		uint16_t PropertiesSize = 0xE0; // 
	} UStruct;
	struct {
		uint16_t Names = 0x50; // 
	} UEnum;
	struct {
		uint16_t FunctionFlags = 0x128;
		uint16_t Func = 0x118;
	} UFunction;
	struct {
		uint16_t ArrayDim = 0x40; // 
		uint16_t ElementSize = 0x44; // 
		uint16_t PropertyFlags = 0x48; // 
		uint16_t Offset = 0x60; // 
		uint16_t Size = 0x88; //
	} UProperty;
};

extern Offsets defs;

bool EngineInit(std::string game, std::vector<std::pair<byte*, byte*>>*);