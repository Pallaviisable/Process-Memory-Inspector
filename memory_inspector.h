////////////////////////////////////////////////////////////////////////////////////
//
//  File:              memory_inspector.h
//  Description:       Declarations for Process Memory Inspector
//                     Reads /proc/<pid>/maps and /proc/<pid>/status on Linux
//                     to display process memory layout and basic process info
//  Author:            Pallavi Sable
//  Date:              06/01/2026
//
//  Platform:          Linux only (requires /proc filesystem)
//  Compile:           gcc -o memory_inspector main.c memory_inspector.c
//
////////////////////////////////////////////////////////////////////////////////////

#ifndef MEMORY_INSPECTOR_H
#define MEMORY_INSPECTOR_H

////////////////////////////////////////////////////////////////////////////////////
//  Memory Segment Reference
//
//  Section     Description
//  ---------   ---------------------------------------------------------------
//  TEXT        Code/instructions segment — executable, read-only
//              Contains compiled binary instructions of the program
//
//  DATA        Data segment — readable and writable, non-executable
//              Contains global and static variables (initialised and uninitialised)
//
//  HEAP        Dynamic memory region
//              Grows upward; managed via malloc()/free()
//
//  STACK       Call stack for the main thread
//              Grows downward; holds local variables, return addresses, stack frames
//
//  VDSO        Virtual Dynamic Shared Object
//              Kernel-mapped shared library to speed up certain syscalls (e.g. gettimeofday)
//              Avoids full context switch into kernel space
//
//  VVAR        Virtual Variables
//              Read-only kernel data mapped into user space (e.g. time variables for VDSO)
//
//  VSYSCALL    Virtual System Call Page (legacy)
//              Fixed-address page for fast syscalls on older kernels (largely replaced by VDSO)
//
//  OTHER       Any region that does not match the above categories
//              Includes shared libraries (.so), memory-mapped files, anonymous mappings
//
////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////
//
//  Function Name:     GetSectionType
//  Input:             details — mapping name/path from /proc/<pid>/maps
//                     perms   — permission string (e.g. "rwxp")
//                     section — output buffer, receives the section label
//  Output:            Fills section with one of:
//                     "HEAP" | "STACK" | "VDSO" | "VVAR" | "TEXT" | "DATA" | "OTHER"
//  Description:       Classifies a memory region based on its name and permissions
//
////////////////////////////////////////////////////////////////////////////////////
void GetSectionType(char *details, char *perms, char *section);

////////////////////////////////////////////////////////////////////////////////////
//
//  Function Name:     ShowMemoryLayout
//  Input:             pid — process ID to inspect
//  Output:            Prints formatted memory map to stdout
//  Description:       Reads /proc/<pid>/maps and prints each memory region with:
//                     start address, end address, size (KB), permissions,
//                     section type, and mapping name
//
////////////////////////////////////////////////////////////////////////////////////
void ShowMemoryLayout(int pid);

////////////////////////////////////////////////////////////////////////////////////
//
//  Function Name:     ShowProcessInformation
//  Input:             pid — process ID to inspect
//  Output:            Prints Name, PID, State, and Thread count to stdout
//  Description:       Reads /proc/<pid>/status and extracts key process fields
//
////////////////////////////////////////////////////////////////////////////////////
void ShowProcessInformation(int pid);

#endif /* MEMORY_INSPECTOR_H */
