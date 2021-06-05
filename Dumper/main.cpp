#include "dumper.h"
#include <Windows.h>

int main(int argc, char* argv[]) {

  auto dumper = Dumper::GetInstance();
    
  switch (dumper->Init(argc, argv)) {
  case WINDOW_NOT_FOUND: { puts("Can't find UE4 window"); return FAILED; }
  case PROCESS_NOT_FOUND: { puts("Can't find process"); return FAILED; }
  case READER_ERROR: { puts("Can't init reader"); return FAILED; }
  case CANNOT_GET_PROCNAME: { puts("Can't get process name"); return FAILED; }
  case ENGINE_ERROR: { puts("Can't find offsets for this game"); return FAILED; }
  case MODULE_NOT_FOUND: { puts("Can't enumerate modules (protected process?)"); return FAILED; }
  case CANNOT_READ: { puts("Can't read process memory"); return FAILED; }
  case INVALID_IMAGE: { puts("Can't get executable sections"); return FAILED; }
  case OBJECTS_NOT_FOUND: { puts("Can't find objects array"); return FAILED; }
  case NAMES_NOT_FOUND: { puts("Can't find names array"); return FAILED; }
  case SUCCESS: { break; };
  default: { return FAILED; }
  }

  

  switch (dumper->Dump()) {
  case FILE_NOT_OPEN: { puts("Can't open file"); return FAILED; }
  case ZERO_PACKAGES: { puts("Size of packages is zero"); return FAILED; }
  case SUCCESS: { break; }
  default: { return FAILED; }
  }

  return SUCCESS;
}