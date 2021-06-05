#pragma once
#include <filesystem>

enum {
  SUCCESS,
  FAILED,
  WINDOW_NOT_FOUND,
  PROCESS_NOT_FOUND,
  READER_ERROR,
  CANNOT_GET_PROCNAME,
  ENGINE_ERROR,
  MODULE_NOT_FOUND,
  CANNOT_READ,
  INVALID_IMAGE,
  NAMES_NOT_FOUND,
  OBJECTS_NOT_FOUND,
  FILE_NOT_OPEN,
  ZERO_PACKAGES
};

namespace fs = std::filesystem;

class Dumper {
protected:
  bool Full = true;
  bool Wait = false;
  fs::path Directory;
private:
  Dumper() {};
public:

  static Dumper* GetInstance();

  int Init(int argc, char* argv[]);
  int Dump();
};