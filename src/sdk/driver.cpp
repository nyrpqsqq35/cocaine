#include "driver.h"

#include <cocaine.h>
Driver* driver;

NTSTATUS Driver::SetSystemEnvironmentPrivilege(BOOLEAN Enable,
                                               PBOOLEAN WasEnabled) {
  if (WasEnabled != nullptr) {
    *WasEnabled = FALSE;
  }

  const NTSTATUS Status =
      RtlAdjustPrivilege(SE_SYSTEM_ENVIRONMENT_PRIVILEGE, Enable, FALSE,
                         &SeSystemEnvironmentWasEnabled);
  if (NT_SUCCESS(Status) && WasEnabled != nullptr) {
    *WasEnabled = SeSystemEnvironmentWasEnabled;
  }

  return Status;
}

void Driver::SendCommand(MemoryCommand* cmd) {
  wchar_t VarName[] = {'n', 'y', 'a', 'a', 'u', 'w', 'u', '\0'};
  UNICODE_STRING FVariableName = UNICODE_STRING();
  FVariableName.Buffer = VarName;
  FVariableName.Length = 28;
  FVariableName.MaximumLength = 30;
  // UNICODE_STRING VariableName2 = { sizeof(VARIABLE_NAME) -
  // sizeof(VARIABLE_NAME[0]), sizeof(VARIABLE_NAME), (PWSTR)VARIABLE_NAME };
  // UNICODE_STRING VariableName = RTL_CONSTANT_STRING(VARIABLE_NAME);
  NtSetSystemEnvironmentValueEx(&FVariableName, &DummyGuid, cmd,
                                sizeof(MemoryCommand), ATTRIBUTES);
  memset(VarName, 0, sizeof(VarName));
  // memset(VariableName.Buffer, 0, VariableName.Length);
  // VariableName.Length = 0;
}

NTSTATUS Driver::copy_memory(uintptr_t src_process_id, uintptr_t src_address,
                             uintptr_t dest_process_id, uintptr_t dest_address,
                             size_t size) {
  uintptr_t result = 0;
  MemoryCommand cmd = MemoryCommand();
  cmd.operation = BASE_OPERATION * 0x45 * 0x3;
  cmd.magic = COMMAND_MAGIC;
  cmd.data[0] = (uintptr_t)src_process_id;
  cmd.data[1] = (uintptr_t)src_address;
  cmd.data[2] = (uintptr_t)dest_process_id;
  cmd.data[3] = (uintptr_t)dest_address;
  cmd.data[4] = (uintptr_t)size;
  cmd.data[5] = (uintptr_t)&result;
  SendCommand(&cmd);
  return (NTSTATUS)result;
}

NTSTATUS Driver::read_memory(uintptr_t process_id, uintptr_t address,
                             uintptr_t buffer, size_t size) {
  if (address < 0x1000000) {
    printf("bad address: %llx\n", address);
    // DebugBreak();
    // throw std::runtime_error("Are you sure?");
    return NTE_INTERNAL_ERROR;
  }
  if (is_guarded(address) && GuardedVA == 0)
    throw std::runtime_error("Please supply GuardVA");
  return copy_memory(process_id, address, currentProcessId, buffer, size);
}

NTSTATUS Driver::write_memory(uintptr_t process_id, uintptr_t address,
                              uintptr_t buffer, size_t size) {
  return copy_memory(currentProcessId, buffer, process_id, address, size);
}

NTSTATUS enumTable(PVOID* Buffer) {
  NTSTATUS status;
  PVOID buffer;
  ULONG bufferSize = 0x100;
  buffer = malloc(bufferSize);

  status = NtQuerySystemInformation(SystemBigPoolInformation, buffer,
                                    bufferSize, &bufferSize);

  while (status == STATUS_INFO_LENGTH_MISMATCH) {
    free(buffer);
    buffer = malloc(bufferSize);

    status = NtQuerySystemInformation(SystemBigPoolInformation, buffer,
                                      bufferSize, &bufferSize);
  }
  if (NT_SUCCESS(status)) {
    *Buffer = buffer;
  } else {
    free(buffer);
  }
  return status;
}

bool getGuardedVA2(uintptr_t* addr) {
  PSYSTEM_BIGPOOL_INFORMATION bigPoolTable;
  ULONG i;
  if (!NT_SUCCESS(enumTable((void**)&bigPoolTable))) {
    printf(" ~> Fail#4\n");
    return false;
  }

  for (i = 0; i < bigPoolTable->Count; ++i) {
    INT itemIndex;
    SYSTEM_BIGPOOL_ENTRY poolTagInfo;
    poolTagInfo = bigPoolTable->AllocatedInfo[i];
    if (!poolTagInfo.NonPaged || poolTagInfo.TagUlong != 'TnoC' ||
        poolTagInfo.SizeInBytes != 0x200000)
      continue;
    auto va = ((uintptr_t)poolTagInfo.VirtualAddress & ~1ull);
    auto size = poolTagInfo.SizeInBytes;
    // if (va == 0xffffaa7a68200000) continue;
    // if (va == 0xffffaa7a66000000) continue;
    printf("Found pool entry VA, 0x%llx, Size = %llx\n", va, size);
    driver->grs.push_back(va);
    *addr = va;
    // free(bigPoolTable);
    // return true;
  }
  free(bigPoolTable);
  if (*addr != 0) return true;
  return false;
}

bool getGuardedVA(uintptr_t* addr) {
  auto fuck = driver->GetKernelModuleAddress((char*)"vgk.sys");
  printf("VGK.sys @ %llx\n", fuck);

  auto poolAddr = driver->read<uintptr_t>(4, fuck + 0x7FCE0);
  printf("raw pool @ %llx\n", poolAddr);
  // printf(" pool bitwsed @ %llx\n", poolAddr & 0xFFFFFF);
  *addr = poolAddr;

  // uintptr_t old = 0;
  // getGuardedVA2(&old);
  // printf(" our addr @ %llx\n", old);

  return true;
}

bool Driver::get_guarded_va(uintptr_t* address) {
  return getGuardedVA(address);
}

uintptr_t Driver::GetBaseAddress(uintptr_t pid) {
  uintptr_t result = 0;
  MemoryCommand cmd = MemoryCommand();
  cmd.operation = BASE_OPERATION * 0x45 * 0x4;
  cmd.magic = COMMAND_MAGIC;
  cmd.data[0] = pid;
  cmd.data[1] = (uintptr_t)&result;
  SendCommand(&cmd);
  return result;
}

uintptr_t Driver::GetKernelModuleAddress(char* module_name) {
  void* buffer = nullptr;
  DWORD buffer_size = 0;

  NTSTATUS status = NtQuerySystemInformation(SystemModuleInformation, buffer,
                                             buffer_size, &buffer_size);
  while (status == STATUS_INFO_LENGTH_MISMATCH) {
    VirtualFree(buffer, 0, MEM_RELEASE);
    buffer = VirtualAlloc(nullptr, buffer_size, MEM_COMMIT | MEM_RESERVE,
                          PAGE_READWRITE);
    if (buffer == 0) {
      return 0;
    }
    status = NtQuerySystemInformation(SystemModuleInformation, buffer,
                                      buffer_size, &buffer_size);
  }

  if (!NT_SUCCESS(status)) {
    VirtualFree(buffer, 0, MEM_RELEASE);
    return 0;
  }

  const PRTL_PROCESS_MODULES modules = (PRTL_PROCESS_MODULES)buffer;
  if (modules == nullptr) {
    VirtualFree(buffer, 0, MEM_RELEASE);
    return 0;
  }
  for (auto i = 0u; i < modules->NumberOfModules; ++i) {
    char* current_module_name = (char*)(modules->Modules[i].FullPathName +
                                        modules->Modules[i].OffsetToFileName);
    if (!_stricmp(current_module_name, module_name)) {
      const uintptr_t result = (uintptr_t)(modules->Modules[i].ImageBase);
      VirtualFree(buffer, 0, MEM_RELEASE);
      return result;
    }
  }

  VirtualFree(buffer, 0, MEM_RELEASE);
  return 0;
}

uintptr_t Driver::GetKernelModuleExport(uintptr_t kernel_module_base,
                                        char* function_name) {
  if (!kernel_module_base) {
    return 0;
  }

  IMAGE_DOS_HEADER dos_header = {0};
  IMAGE_NT_HEADERS64 nt_headers = {0};

  read_memory(4, kernel_module_base, (uintptr_t)&dos_header,
              sizeof(dos_header));
  if (dos_header.e_magic != IMAGE_DOS_SIGNATURE) {
    return 0;
  }

  read_memory(4, kernel_module_base + dos_header.e_lfanew,
              (uintptr_t)&nt_headers, sizeof(nt_headers));
  if (nt_headers.Signature != IMAGE_NT_SIGNATURE) {
    return 0;
  }

  const auto export_base =
      nt_headers.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT]
          .VirtualAddress;
  const auto export_base_size =
      nt_headers.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT]
          .Size;
  if (!export_base || !export_base_size) {
    return 0;
  }

  const auto export_data =
      reinterpret_cast<PIMAGE_EXPORT_DIRECTORY>(VirtualAlloc(
          nullptr, export_base_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE));

  read_memory(4, kernel_module_base + export_base, (uintptr_t)export_data,
              export_base_size);
  // if (!ReadMemory(device_handle, kernel_module_base + export_base,
  // export_data, export_base_size))
  // {
  //     VirtualFree(export_data, 0, MEM_RELEASE);
  //     return 0;
  // }

  const auto delta = reinterpret_cast<uintptr_t>(export_data) - export_base;
  const auto name_table =
      reinterpret_cast<UINT32*>(export_data->AddressOfNames + delta);
  const auto ordinal_table =
      reinterpret_cast<UINT16*>(export_data->AddressOfNameOrdinals + delta);
  const auto function_table =
      reinterpret_cast<UINT32*>(export_data->AddressOfFunctions + delta);

  for (auto i = 0u; i < export_data->NumberOfNames; ++i) {
    char* current_function_name = (char*)(name_table[i] + delta);
    if (!_stricmp(current_function_name, function_name)) {
      const auto function_ordinal = ordinal_table[i];
      const auto function_address =
          kernel_module_base + function_table[function_ordinal];
      if (function_address >= kernel_module_base + export_base &&
          function_address <=
              kernel_module_base + export_base + export_base_size) {
        VirtualFree(export_data, 0, MEM_RELEASE);
        return 0;  // No forwarded exports on 64bit?
      }
      VirtualFree(export_data, 0, MEM_RELEASE);
      return function_address;
    }
  }

  VirtualFree(export_data, 0, MEM_RELEASE);
  return 0;
}

uintptr_t Driver::PatternScan(uintptr_t pid, uintptr_t base,
                              const char* signature) {
  static auto pattern_to_byte = [](const char* pattern) {
    auto bytes = std::vector<int>{};
    auto start = const_cast<char*>(pattern);
    auto end = const_cast<char*>(pattern) + strlen(pattern);

    for (auto current = start; current < end; ++current) {
      if (*current == '?') {
        ++current;
        if (*current == '?') ++current;
        bytes.push_back(-1);
      } else {
        bytes.push_back(strtoul(current, &current, 16));
      }
    }
    return bytes;
  };

  IMAGE_DOS_HEADER dosHeader = {0};
  IMAGE_NT_HEADERS64 ntHeaders = {0};
  read_memory(pid, base, (uintptr_t)&dosHeader, sizeof(dosHeader));
  if (dosHeader.e_magic != IMAGE_DOS_SIGNATURE) {
    return 0x50;
  }

  read_memory(pid, base + dosHeader.e_lfanew, (uintptr_t)&ntHeaders,
              sizeof(ntHeaders));
  if (ntHeaders.Signature != IMAGE_NT_SIGNATURE) {
    return 0x51;
  }

  auto sizeOfImage = ntHeaders.OptionalHeader.SizeOfImage;
  auto patternBytes = pattern_to_byte(signature);
  uintptr_t* scanBytes = new uintptr_t[sizeOfImage + 1];
  copy_memory(pid, base, currentProcessId, (uintptr_t)&scanBytes, sizeOfImage);

  auto s = patternBytes.size();
  auto d = patternBytes.data();

  for (auto i = 0ul; i < sizeOfImage; ++i) {
    bool found = true;
    for (auto j = 0ul; j < s; ++j) {
      if (scanBytes[i + j] != d[j] && d[j] != -1) {
        found = false;
        break;
      }
    }
    if (found) {
      return base + i;
    }
  }
  return 0;
}

bool Driver::initialize() {
  DbgLog("=> initializing Driver . . .\n");

  currentProcessId = GetCurrentProcessId();
  DbgLog("-> felix pid = %llx\n", currentProcessId);

  ntModule = LoadLibraryW(L"ntdll.dll");
  DbgLog("-> Ntdll.dll @ 0x%p\n", ntModule);

  NTSTATUS status =
      SetSystemEnvironmentPrivilege(true, &SeSystemEnvironmentWasEnabled);
  if (!NT_SUCCESS(status)) {
    DbgLog("~> please run as Administrator\n");
    return false;
  }

  BYTE nstosname[] = {'n', 't', 'o', 's', 'k', 'r', 'n',
                      'l', '.', 'e', 'x', 'e', 0};
  kernelModuleAddress = GetKernelModuleAddress((char*)nstosname);
  memset(nstosname, 0, sizeof(nstosname));
  DbgLog("-> Ntoskrnl.exe @ 0x%p\n", nstosname);

  BYTE pbid[] = {'P', 's', 'L', 'o', 'o', 'k', 'u', 'p', 'P',
                 'r', 'o', 'c', 'e', 's', 's', 'B', 'y', 'P',
                 'r', 'o', 'c', 'e', 's', 's', 'I', 'd', 0};
  kernel_PsLookupProcessByProcessId =
      GetKernelModuleExport(kernelModuleAddress, (char*)pbid);
  DbgLog("  -> PsLookupProcessByProcessId = 0x%llx\n",
         kernel_PsLookupProcessByProcessId);
  memset(pbid, 0, sizeof(pbid));

  BYTE gba[] = {'P', 's', 'G', 'e', 't', 'P', 'r', 'o', 'c', 'e', 's',
                's', 'S', 'e', 'c', 't', 'i', 'o', 'n', 'B', 'a', 's',
                'e', 'A', 'd', 'd', 'r', 'e', 's', 's', 0};
  kernel_PsGetProcessSectionBaseAddress =
      GetKernelModuleExport(kernelModuleAddress, (char*)gba);
  DbgLog("  -> kernel_PsGetProcessSectionBaseAddress = 0x%llx\n",
         kernel_PsGetProcessSectionBaseAddress);
  memset(gba, 0, sizeof(gba));

  BYTE mmcp[] = {'M', 'm', 'C', 'o', 'p', 'y', 'V', 'i', 'r', 't',
                 'u', 'a', 'l', 'M', 'e', 'm', 'o', 'r', 'y', 0};
  kernel_MmCopyVirtualMemory =
      GetKernelModuleExport(kernelModuleAddress, (char*)mmcp);
  DbgLog("  -> kernel_MmCopyVirtualMemory = 0x%llx\n",
         kernel_MmCopyVirtualMemory);
  memset(mmcp, 0, sizeof(mmcp));

  kernel_ZwQuerySystemInformation = GetKernelModuleExport(
      kernelModuleAddress, (char*)"ZwQuerySystemInformation");
  DbgLog("  -> kernel_ZwQuerySystemInformation = 0x%llx\n",
         kernel_ZwQuerySystemInformation);
  kernel_ExAllocatePool =
      GetKernelModuleExport(kernelModuleAddress, (char*)"ExAllocatePool");
  DbgLog("  -> kernel_ExAllocatePool = 0x%llx\n", kernel_ExAllocatePool);
  kernel_ExFreePool =
      GetKernelModuleExport(kernelModuleAddress, (char*)"ExFreePool");
  DbgLog("  -> kernel_ExFreePool = 0x%llx\n", kernel_ExFreePool);

  uintptr_t result = 0;
  MemoryCommand cmd = MemoryCommand();
  cmd.operation = BASE_OPERATION * 0x45 * 0x2;
  cmd.magic = COMMAND_MAGIC;
  cmd.data[0] = kernel_PsLookupProcessByProcessId;
  cmd.data[1] = kernel_PsGetProcessSectionBaseAddress;
  cmd.data[2] = kernel_MmCopyVirtualMemory;
  cmd.data[3] = (uintptr_t)&result;
  SendCommand(&cmd);

  DbgLog("  => #1 Result = %llx\n", result);
  cmd.operation = BASE_OPERATION * 0x45 * 0xf;
  cmd.data[0] = kernel_ZwQuerySystemInformation;
  cmd.data[1] = kernel_ExAllocatePool;
  cmd.data[2] = kernel_ExFreePool;
  // result & magic set above
  SendCommand(&cmd);
  DbgLog("  => #2 Result = %llx\n", result);

  return result;
}