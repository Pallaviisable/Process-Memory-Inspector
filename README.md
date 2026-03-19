# Process Memory Inspector

> A Linux systems programming tool written in C that reads the `/proc` filesystem to inspect the live memory layout of any running process — displaying its stack, heap, text, data segments, and more in real time.

This project demonstrates hands-on knowledge of **Linux internals**, **C systems programming**, **process memory architecture**, and the **`/proc` virtual filesystem** — skills directly relevant to systems, embedded, OS, and low-level software engineering roles.

---

## Why This Project

Understanding how a process is laid out in memory is fundamental to systems programming — it underlies debugging, security research, performance profiling, and OS development. This tool inspects that layout live, from userspace, using only the Linux `/proc` filesystem and standard C — no external libraries, no frameworks.

---

## What It Does

Given a PID, the tool:

1. **Reads `/proc/<pid>/status`** — extracts process name, PID, state, and thread count
2. **Reads `/proc/<pid>/maps`** — parses every memory region mapped to the process and classifies it by type

Output includes, for each memory region:

| Column | Description |
|---|---|
| `START` | Start virtual address (hex) |
| `END` | End virtual address (hex) |
| `SIZE(KB)` | Region size in kilobytes |
| `PERMS` | Permissions — `r`ead / `w`rite / e`x`ecute / `p`rivate |
| `SECTION` | Classified segment type (see below) |
| `MAPPING` | File path, kernel region name, or `Anonymous` |

---

## Memory Segment Classification

| Section | Description |
|---|---|
| `TEXT` | Executable code segment — read-only, contains compiled binary instructions |
| `DATA` | Data segment — writable, holds global and static variables |
| `HEAP` | Dynamic memory region — grows upward, managed via `malloc()`/`free()` |
| `STACK` | Main thread call stack — grows downward, holds local variables and return addresses |
| `VDSO` | Virtual Dynamic Shared Object — kernel-mapped library for fast syscalls (avoids full context switch) |
| `VVAR` | Virtual Variables — read-only kernel data mapped into userspace (used by VDSO) |
| `OTHER` | Shared libraries (`.so`), memory-mapped files, or unclassified anonymous mappings |

Classification priority: named kernel regions (`[heap]`, `[stack]`, etc.) → executable permission → writable permission → OTHER.

---

## Technical Highlights

- **`/proc` filesystem parsing** — directly reads kernel-exposed virtual files; no system call wrappers needed
- **`sscanf` for structured line parsing** — robust extraction of address ranges, permissions, inode, and mapping names from `/proc/<pid>/maps` format
- **`snprintf` over `sprintf`** — all filename construction uses bounded formatting to prevent buffer overflows
- **Column-aligned output with headers** — formatted with `%-Ns` width specifiers for readable terminal output
- **Defensive input handling** — `scanf` return value checked, malformed address ranges guarded, descriptive error messages on file access failure
- **Double `printf("\n")` bug fixed** — `fgets` already captures the newline; the original `printf("%s\n")` was adding a blank line after every status field
- **Line buffer sized correctly** — increased from 80 to 256 bytes; `/proc/maps` lines with long file paths exceed 80 characters
- **Clean separation of concerns** — classification logic, display logic, and entry point are fully separated across `.h`, `.c`, and `main.c`

---

## File Structure

```
MemoryInspector/
├── memory_inspector.h   ← Function declarations + full segment reference docs
├── memory_inspector.c   ← GetSectionType(), ShowMemoryLayout(), ShowProcessInformation()
├── main.c               ← Entry point — input, validation, function calls
└── README.md
```

---

## How to Compile & Run

```bash
gcc -o memory_inspector main.c memory_inspector.c
./memory_inspector
```

When prompted, enter any valid PID. To find one:

```bash
ps aux              # list all processes
pgrep firefox       # find PID by process name
echo $$             # PID of your current shell
```

> **Linux only.** The `/proc` filesystem is a Linux kernel feature. This tool will not work on macOS or Windows.

> **Permissions.** You can inspect your own processes freely. Inspecting processes owned by other users requires `sudo`.

---

## Example Output

```
-----------------------------------------
-------Memory_Process_Inspector----------
-----------------------------------------
Enter the PID of a process to inspect
1234
Accessing the File : /proc/1234/status
-----------------------------------------
----------PROCESS INFORMATION------------
-----------------------------------------
Name:   bash
Pid:    1234
State:  S (sleeping)
Threads:        1
-----------------------------------------
Accessing the File : /proc/1234/maps
-----------------------------------------
--------------MEMORY LAYOUT--------------
-----------------------------------------
START              END                SIZE(KB)   PERMS  SECTION  MAPPING
-----------------------------------------
55a3e2100000       55a3e2128000       160        r--p   TEXT     /usr/bin/bash
55a3e2128000       55a3e2232000       1064       r-xp   TEXT     /usr/bin/bash
...
7ffd3c200000       7ffd3c221000       132        rwxp   STACK    [stack]
```

---

## Skills Demonstrated

`C` &nbsp;|&nbsp; `Linux Systems Programming` &nbsp;|&nbsp; `/proc Filesystem` &nbsp;|&nbsp; `Process Memory Architecture` &nbsp;|&nbsp; `File I/O` &nbsp;|&nbsp; `String Parsing` &nbsp;|&nbsp; `Memory Segments` &nbsp;|&nbsp; `Defensive Programming` &nbsp;|&nbsp; `Buffer Safety`

---

## Author

**Pallavi Sable** — 06/01/2026
