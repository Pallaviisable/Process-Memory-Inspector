# Process Memory Inspector

> A Linux-based process inspection tool built in C++ using Object-Oriented Programming.  
> Reads `/proc/<pid>/maps` and `/proc/<pid>/status` to display a process's full memory layout, segment classification, and a summarised memory report.

---

## Why This Project

Every running process on Linux has a virtual memory map maintained by the kernel and exposed through the `/proc` filesystem. Most developers never look at it. This tool reads that raw data directly — no libraries, no abstractions — and presents it in a clean, structured format showing exactly how a process's memory is organised into TEXT, DATA, HEAP, STACK, and other regions.

The project was rebuilt from a working C implementation into a fully OOP C++ design to demonstrate class design, separation of concerns, and modular code structure.

---

## What It Does

- Reads `/proc/<pid>/status` → displays process name, state, and thread count
- Reads `/proc/<pid>/maps` → parses every memory region into a structured object
- Classifies each region as TEXT, DATA, HEAP, STACK, VDSO, VVAR, or OTHER based on permissions and kernel labels
- Displays a formatted memory layout table with start/end addresses, size in KB, permissions, and mapping name
- Displays a summary report grouped by section type with total KB per section

---

## Sample Output

```
-----------------------------------------
-------Process Memory Inspector----------
----------- C++ OOP Version -------------
-----------------------------------------
Enter the PID of process to inspect : 1234

Accessing the File : /proc/1234/status
-----------------------------------------
----------PROCESS INFORMATION------------
-----------------------------------------
PID     : 1234
Name:   bash
State:  S (sleeping)
Threads:    1
-----------------------------------------

Accessing the File : /proc/1234/maps
-----------------------------------------
-----------MEMORY LAYOUT-----------------
-----------------------------------------
START              END                SIZE(KB)   PERMS  SECTION  MAPPING
-----------------------------------------
55a3c4200000       55a3c4201000       4          r--p   OTHER    /usr/bin/bash
55a3c4201000       55a3c42f1000       960        r-xp   TEXT     /usr/bin/bash
55a3c42f1000       55a3c4330000       252        r--p   OTHER    /usr/bin/bash
55a3c4331000       55a3c433a000       36         rw-p   DATA     /usr/bin/bash
55a3c5b72000       55a3c5b95000       140        rw-p   HEAP     [heap]
7ffd3a200000       7ffd3a221000       132        rw-p   STACK    [stack]
...

-----------------------------------------
----------MEMORY SUMMARY-----------------
-----------------------------------------
TEXT     : 3 region(s), 972 KB total
DATA     : 5 region(s), 88 KB total
HEAP     : 1 region(s), 140 KB total
STACK    : 1 region(s), 132 KB total
VVAR     : 1 region(s), 4 KB total
OTHER    : 9 region(s), 412 KB total
-----------------------------------------
```

---

## Class Design

| Class | File | Responsibility |
|---|---|---|
| `MemorySegment` | `memory_inspector.h/.cpp` | Stores one memory region — address, size, permissions, section type. Classifies itself via `ClassifySection()` |
| `ProcessInfo` | `memory_inspector.h/.cpp` | Reads `/proc/<pid>/status` and extracts Name, State, Threads |
| `MemoryInspector` | `memory_inspector.h/.cpp` | Reads `/proc/<pid>/maps`, stores all segments in array, provides layout and summary display |

---

## Section Type Classification Logic

| Section | Condition |
|---|---|
| `HEAP` | Details label is `[heap]` |
| `STACK` | Details label is `[stack]` |
| `VDSO` | Details label is `[vdso]` |
| `VVAR` | Details label is `[vvar]` |
| `TEXT` | Permission bit 2 is `x` (executable) |
| `DATA` | Permission bit 1 is `w` (writable) |
| `OTHER` | Everything else (shared libs, mapped files) |

---

## File Structure

```
Process-Memory-Inspector/
├── memory_inspector.h      ← Class declarations for all 3 classes
├── memory_inspector.cpp    ← Full implementation with documented function headers
└── main.cpp                ← Entry point — creates objects and drives the flow
```

---

## How to Compile and Run

```bash
# Compile
g++ -o memory_inspector memory_inspector.cpp main.cpp

# Run
./memory_inspector

# Find a PID to inspect
ps aux
```

> **Linux only** — this tool reads the `/proc` filesystem which exists only on Linux.  
> Run with appropriate permissions. To inspect system processes use `sudo`.

---

## Technical Highlights

- **Pure OOP design** — 3 classes each with a single clear responsibility
- **No external libraries** — only standard C++ headers (`iostream`, `fstream`, `cstdio`, `cstring`)
- **Separation of concerns** — declarations in `.h`, full implementation in `.cpp`, driver in `main.cpp`
- **Documented function headers** — every method has Input / Output / Description block
- **Defensive file handling** — checks for failed `fopen` with meaningful error messages
- **Bonus summary report** — groups all segments by type and totals KB per section (not in original C version)
- **Safe string handling** — uses `strncpy` with size guards, `memset` for buffer initialisation

---

## Concepts Demonstrated

`C++` | `OOP` | `Classes & Objects` | `Linux /proc Filesystem` | `Memory Management` | `File I/O` | `System Programming` | `Process Inspection` | `Header & Implementation Separation`

---

## Author

**Pallavi Sable** — 2026  
B.E. Electronics and Computer Engineering, Savitribai Phule Pune University
