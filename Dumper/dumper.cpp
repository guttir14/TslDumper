#include "dumper.h"
#include "wrappers.h"
#include <fmt/core.h>
#include "memory.h"
#include "utils.h"
#include "engine.h"
#include "types.h"

Dumper* Dumper::GetInstance() {
	static Dumper dumper;
	return &dumper;
}

int Dumper::Init(int argc, char* argv[]) {
	for (auto i = 1; i < argc; i++) {
		auto arg = *(uint16*)argv[i];
		if (arg == 'h-') { printf("'-p' - dump only names and objects\n'-w' - wait for input"); return FAILED; }
		else if (arg == 'p-') { Full = false; }
		else if (arg == 'w-') { Wait = true; }
	}

	if (Wait) { system("pause"); }

	uint32 pid = 0;

	HWND hWnd = FindWindowA("UnrealWindow", nullptr);
	if (!hWnd) { return WINDOW_NOT_FOUND; };
	GetWindowThreadProcessId(hWnd, (DWORD*)(&pid));
	if (!pid) { return PROCESS_NOT_FOUND; };

	if (!ReaderInit(pid)) { return READER_ERROR; };

	fs::path processName;

	wchar_t processPath[MAX_PATH]{};
	if (!GetProccessPath(pid, processPath, MAX_PATH)) { return CANNOT_GET_PROCNAME; };
	processName = fs::path(processPath).filename();
	printf("Found UE4 game: %ls\n", processName.c_str());

	auto root = fs::path(argv[0]); root.remove_filename();
	auto game = processName.stem();
	Directory = root / "Games" / game;
	fs::create_directories(Directory);
	auto [base, size] = GetModuleInfo(pid, processName);
	if (!(base && size)) { return MODULE_NOT_FOUND; }
	std::vector<uint8> image(size);
	if (!Read(base, image.data(), size)) { return CANNOT_READ; }
	auto sections = GetExSections(image.data());
	if (!sections.size()) { return INVALID_IMAGE; }
	Base = (uint64)(base);
	if (!EngineInit(game.string(), &sections)) { return ENGINE_ERROR; };

	return SUCCESS;
}

int Dumper::Dump() {
	{
		File file(Directory / "NamesDump.txt", "w");
		if (!file) { return FILE_NOT_OPEN; }
		uint32 size = 0;
		GlobalNames.Dump([&file, &size](std::string_view name, uint32 id) { fmt::print(file, "[{:0>6}] {}\n", id, name); size++; });
		fmt::print("Names: {}\n", size);
	}

	std::unordered_map<uint8*, std::vector<UE_UObject>> packages;
	{
		File file(Directory / "ObjectsDump.txt", "w");
		if (!file) { return FILE_NOT_OPEN; }
		uint32 size = 0;
		if (Full) {
			ObjObjects.Dump(
				[&file, &size, &packages](UE_UObject object)
				{
					fmt::print(file, "[{:0>6}] <{}> {}\n", object.GetIndex(), object.GetAddress(), object.GetFullName()); size++;
					if (object.IsA<UE_UStruct>() || object.IsA<UE_UEnum>())
					{
						auto packageObj = object.GetPackageObject();
						packages[packageObj].push_back(object);
					}
				}
			);
		}
		else
		{
			ObjObjects.Dump([&file, &size](UE_UObject object) { fmt::print(file, "[{:0>6}] <{}> {}\n", object.GetIndex(), object.GetAddress(), object.GetFullName()); size++; });
		}

		fmt::print("Objects: {}\n", size);
	}

	if (!Full) { return SUCCESS; }

	// Checking if we have any package after clearing.
	if (!packages.size()) { return ZERO_PACKAGES; }

	fmt::print("Packages: {}\n", packages.size());

	auto path = Directory / "DUMP";
	fs::create_directories(path);

	uint32 i = 1; uint32 saved = 0;

	std::string unsaved{};
	for (UE_UPackage package : packages) {
		fmt::print("\rProcessing: {}/{}", i++, packages.size());

		package.Process();
		if (package.Save(path)) { saved++; }
		else { unsaved += (package.GetObject().GetName() + ", "); };
	}

	fmt::print("\nSaved packages: {}", saved);

	if (unsaved.size()) {
		unsaved.erase(unsaved.size() - 2);
		fmt::print("\nUnsaved packages (empty): [ {} ]", unsaved);
	}

	return SUCCESS;
}