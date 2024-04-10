#ifndef SDK_DRIVER_H
#define SDK_DRIVER_H

#include <cocaine.h>

#define BASE_OPERATION 0x81
#define COMMAND_MAGIC BASE_OPERATION * 0x45 * 0x01

#define EFI_VARIABLE_NON_VOLATILE 0x00000001
#define EFI_VARIABLE_BOOTSERVICE_ACCESS 0x00000002
#define EFI_VARIABLE_RUNTIME_ACCESS 0x00000004
#define EFI_VARIABLE_HARDWARE_ERROR_RECORD 0x00000008
#define EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS 0x00000010
#define EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS 0x00000020
#define EFI_VARIABLE_APPEND_WRITE 0x00000040

#define SE_SYSTEM_ENVIRONMENT_PRIVILEGE (22L)

#define ATTRIBUTES                                                    \
  (EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS |      \
   EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_HARDWARE_ERROR_RECORD | \
   EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS |                          \
   EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS |               \
   EFI_VARIABLE_APPEND_WRITE)

typedef struct _MemoryCommand {
  int magic;
  int operation;
  unsigned long long data[6];
} MemoryCommand;

class Driver {
 public:
  HMODULE ntModule;
  GUID DummyGuid = {0xdb934910,
                    0xc2ca,
                    0x4156,
                    {0xa4, 0x00, 0xc5, 0xe0, 0xa0, 0x64, 0xcc, 0x3b}};

  BOOLEAN SeSystemEnvironmentWasEnabled;

  uintptr_t currentProcessId = 0;
  uintptr_t kernelModuleAddress;
  uintptr_t kernel_PsLookupProcessByProcessId;
  uintptr_t kernel_PsGetProcessSectionBaseAddress;
  uintptr_t kernel_MmCopyVirtualMemory;
  uintptr_t kernel_ZwQuerySystemInformation;
  uintptr_t kernel_ExAllocatePool;
  uintptr_t kernel_ExFreePool;

  // int SystemModuleInformation = 11;
  // int SystemHandleInformation = 16;
  // int SystemExtendedHandleInformation = 64;
  // int SystemBigPoolInformation = 0x42;

  NTSTATUS SetSystemEnvironmentPrivilege(BOOLEAN Enable, PBOOLEAN WasEnabled);

  void SendCommand(MemoryCommand* cmd);

  NTSTATUS copy_memory(uintptr_t src_process_id, uintptr_t src_address,
                       uintptr_t dest_process_id, uintptr_t dest_address,
                       size_t size);

  NTSTATUS read_memory(uintptr_t process_id, uintptr_t address,
                       uintptr_t buffer, size_t size);

  NTSTATUS write_memory(uintptr_t process_id, uintptr_t address,
                        uintptr_t buffer, size_t size);

  bool get_guarded_va(uintptr_t* address);

  uintptr_t GetBaseAddress(uintptr_t pid);

  uintptr_t GetKernelModuleAddress(char* module_name);

  uintptr_t GetKernelModuleExport(uintptr_t kernel_module_base,
                                  char* function_name);

  uintptr_t PatternScan(uintptr_t pid, uintptr_t base, const char* signature);

  bool initialize();

  uintptr_t GuardedVA = 0x0;
  std::vector<uintptr_t> grs;

  inline bool is_guarded(uintptr_t address) {
    constexpr uintptr_t filter = 0xFFFFFFFF00000000;
    uintptr_t result = address & filter;
    return result == 0x8000000000 || result == 0x10000000000;
  }

  template <typename T>
  T read(const uintptr_t process_id, const uintptr_t address,
         PNTSTATUS out_status = 0) {
    T buffer{};
    uintptr_t pid = process_id;
    uintptr_t addy = address;

    if (is_guarded(addy)) {
      pid = 4;
      addy = GuardedVA + (address & 0xffffff);
    }
    NTSTATUS status = read_memory(pid, addy, (uintptr_t)&buffer, sizeof(T));
    if (out_status) *out_status = status;
    return buffer;
  }
  // template <>
  // uintptr_t read(const uintptr_t process_id, const uintptr_t address,
  //                PNTSTATUS out_status) {
  //   uintptr_t buffer{};
  //   uintptr_t pid = process_id;
  //   uintptr_t addy = address;
  //   if (is_guarded(addy)) {
  //     pid = 4;
  //     // addy = GuardedVA + (address & 0xffffff);
  //     for (const auto& e : grs) {
  //       NTSTATUS status = read_memory(pid, e + (address & 0xffffff),
  //                                     (uintptr_t)&buffer, sizeof(uintptr_t));
  //       if (out_status) *out_status = status;
  //       if (buffer != 0) return buffer;
  //     }
  //     return buffer;
  //   } else {
  //     NTSTATUS status =
  //         read_memory(pid, addy, (uintptr_t)&buffer, sizeof(uintptr_t));
  //     if (out_status) *out_status = status;
  //     return buffer;
  //   }
  // }

  template <typename T>
  void write(const uintptr_t process_id, const uintptr_t address,
             const T& buffer, PNTSTATUS out_status) {
    NTSTATUS status =
        write_memory(process_id, address, (uintptr_t)&buffer, sizeof(T));

    if (out_status) *out_status = status;
  }
};

extern Driver* driver;

#endif /* SDK_DRIVER_H */
