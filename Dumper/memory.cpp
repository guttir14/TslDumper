#include "memory.h"

HANDLE hProcess;

uint64_t Base;

bool Read(void* address, void* buffer, size_t size) {
	return ReadProcessMemory(hProcess, address, buffer, size, nullptr);
}

bool SetDebugPrivilege() {

  HANDLE hToken;
  LUID luid;
  bool err = false;
  if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken)) {
    if (LookupPrivilegeValueA(0, SE_DEBUG_NAME, &luid)) {
      TOKEN_PRIVILEGES priv;
      priv.PrivilegeCount = 1;
      priv.Privileges[0].Luid = luid;
      priv.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
      err = AdjustTokenPrivileges(hToken, FALSE, &priv, sizeof(TOKEN_PRIVILEGES), NULL, NULL);
    }
    CloseHandle(hToken);
  }
  return err;
}

bool ReaderInit(uint32_t pid) {
  SetDebugPrivilege();
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	return hProcess != nullptr;
}