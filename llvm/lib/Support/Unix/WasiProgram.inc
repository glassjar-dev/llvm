//===- llvm/Support/Unix/WasiProgram.inc - WASI Program stubs ---*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// Subprocess and signal-based process control are unavailable on WASI.
//
//===----------------------------------------------------------------------===//

#include "llvm/ADT/SmallString.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/Support/Errc.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/raw_ostream.h"
#include <cstdlib>
#include <optional>
#include <vector>

using namespace llvm;
using namespace sys;

ProcessInfo::ProcessInfo() : Pid(0), ReturnCode(0) {}

ErrorOr<std::string> sys::findProgramByName(StringRef Name,
                                            ArrayRef<StringRef> Paths) {
  assert(!Name.empty() && "Must have a name!");
  if (Name.contains('/'))
    return std::string(Name);

  SmallVector<StringRef, 16> EnvironmentPaths;
  if (Paths.empty())
    if (const char *PathEnv = std::getenv("PATH")) {
      SplitString(PathEnv, EnvironmentPaths, ":");
      Paths = EnvironmentPaths;
    }

  for (auto Path : Paths) {
    if (Path.empty())
      continue;
    SmallString<128> FilePath(Path);
    sys::path::append(FilePath, Name);
    if (sys::fs::can_execute(FilePath.c_str()))
      return std::string(FilePath);
  }
  return errc::no_such_file_or_directory;
}

static bool Execute(ProcessInfo &PI, StringRef Program,
                    ArrayRef<StringRef> Args,
                    std::optional<ArrayRef<StringRef>> Env,
                    ArrayRef<std::optional<StringRef>> Redirects,
                    unsigned MemoryLimit, std::string *ErrMsg,
                    BitVector *AffinityMask, bool DetachProcess) {
  (void)PI;
  (void)Program;
  (void)Args;
  (void)Env;
  (void)Redirects;
  (void)MemoryLimit;
  (void)AffinityMask;
  (void)DetachProcess;
  if (ErrMsg)
    *ErrMsg = "subprocess execution is not supported on WASI";
  return false;
}

ProcessInfo llvm::sys::Wait(const ProcessInfo &PI,
                            std::optional<unsigned> SecondsToWait,
                            std::string *ErrMsg,
                            std::optional<ProcessStatistics> *ProcStat,
                            bool Polling) {
  (void)SecondsToWait;
  (void)ProcStat;
  (void)Polling;
  ProcessInfo Result = PI;
  if (ErrMsg)
    *ErrMsg = "subprocess wait is not supported on WASI";
  return Result;
}

std::error_code llvm::sys::ChangeStdinMode(fs::OpenFlags Flags) {
  if (!(Flags & fs::OF_Text))
    return ChangeStdinToBinary();
  return std::error_code();
}

std::error_code llvm::sys::ChangeStdoutMode(fs::OpenFlags Flags) {
  if (!(Flags & fs::OF_Text))
    return ChangeStdoutToBinary();
  return std::error_code();
}

std::error_code llvm::sys::ChangeStdinToBinary() { return std::error_code(); }

std::error_code llvm::sys::ChangeStdoutToBinary() { return std::error_code(); }

std::error_code
llvm::sys::writeFileWithEncoding(StringRef FileName, StringRef Contents,
                                 WindowsEncodingMethod Encoding) {
  (void)Encoding;
  std::error_code EC;
  llvm::raw_fd_ostream OS(FileName, EC,
                          llvm::sys::fs::OpenFlags::OF_TextWithCRLF);
  if (EC)
    return EC;
  OS << Contents;
  if (OS.has_error())
    return make_error_code(errc::io_error);
  return EC;
}

bool llvm::sys::commandLineFitsWithinSystemLimits(StringRef Program,
                                                  ArrayRef<StringRef> Args) {
  (void)Program;
  (void)Args;
  return true;
}
