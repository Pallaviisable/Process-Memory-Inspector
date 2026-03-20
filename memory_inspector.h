////////////////////////////////////////////////////////////////////////////////////
//
// File        : memory_inspector.h
// Description : Class declarations for Process Memory Inspector
// Author      : Pallavi Sable
// Date        : 2026
//
// Platform    : Linux only (requires /proc filesystem)
//
////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
using namespace std;

////////////////////////////////////////////////////////////////////////////////////
//
// Class Name  : MemorySegment
// Description : Stores information about one memory region from /proc/<pid>/maps
//               Classifies the region type based on permissions and label
//
////////////////////////////////////////////////////////////////////////////////////
class MemorySegment
{
public:
    unsigned long StartAddr;
    unsigned long EndAddr;
    unsigned long SizeKB;
    char          Permissions[10];
    char          SectionType[20];
    char          Details[300];

    // Constructor
    MemorySegment();

    // Member Functions
    void SetData(unsigned long start, unsigned long end, char *perms, char *details);
    void ClassifySection();
    void DisplaySegment();
};

////////////////////////////////////////////////////////////////////////////////////
//
// Class Name  : ProcessInfo
// Description : Reads and displays process metadata from /proc/<pid>/status
//               Fields: Name, State, Threads
//
////////////////////////////////////////////////////////////////////////////////////
class ProcessInfo
{
public:
    int  PID;
    char Name[100];
    char State[100];
    char Threads[100];

    // Constructor
    ProcessInfo();

    // Member Functions
    bool LoadInfo(int pid);
    void DisplayInfo();
};

////////////////////////////////////////////////////////////////////////////////////
//
// Class Name  : MemoryInspector
// Description : Reads /proc/<pid>/maps and stores all memory segments
//               Provides display and summary of the full memory layout
//
////////////////////////////////////////////////////////////////////////////////////

#define MAX_SEGMENTS 500

class MemoryInspector
{
public:
    int           PID;
    MemorySegment Segments[MAX_SEGMENTS];
    int           SegmentCount;

    // Constructor
    MemoryInspector();

    // Member Functions
    bool LoadLayout(int pid);
    void DisplayLayout();
    void DisplaySummary();
};
