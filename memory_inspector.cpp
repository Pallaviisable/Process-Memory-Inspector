////////////////////////////////////////////////////////////////////////////////////
//
// File        : memory_inspector.cpp
// Description : Implementation of MemorySegment, ProcessInfo, MemoryInspector
// Author      : Pallavi Sable
// Date        : 2026
//
// Platform    : Linux only (requires /proc filesystem)
// Compile     : g++ -o memory_inspector memory_inspector.cpp main.cpp
//
////////////////////////////////////////////////////////////////////////////////////

#include <cstring>
#include <cstdio>
#include "memory_inspector.h"

////////////////////////////////////////////////////////////////////////////////////
//
// Class       : MemorySegment
// Function    : MemorySegment (Constructor)
// Description : Initialises all fields to zero/empty
//
////////////////////////////////////////////////////////////////////////////////////
MemorySegment :: MemorySegment()
{
    StartAddr = 0;
    EndAddr   = 0;
    SizeKB    = 0;
    memset(Permissions, 0, sizeof(Permissions));
    memset(SectionType, 0, sizeof(SectionType));
    memset(Details,     0, sizeof(Details));
}

////////////////////////////////////////////////////////////////////////////////////
//
// Class       : MemorySegment
// Function    : SetData
// Input       : start   - start address of memory region
//               end     - end address of memory region
//               perms   - 4-char permission string (e.g. "rwxp")
//               details - mapping name or label from /proc/<pid>/maps
// Output      : None
// Description : Sets all member fields and calls ClassifySection internally
//
////////////////////////////////////////////////////////////////////////////////////
void MemorySegment :: SetData(unsigned long start, unsigned long end, char *perms, char *details)
{
    StartAddr = start;
    EndAddr   = end;
    SizeKB    = (end > start) ? (end - start) / 1024 : 0;

    strncpy(Permissions, perms,   sizeof(Permissions) - 1);
    strncpy(Details,     details, sizeof(Details)     - 1);

    ClassifySection();

    if(strlen(Details) == 0)
    {
        strcpy(Details, "Anonymous");
    }
}

////////////////////////////////////////////////////////////////////////////////////
//
// Class       : MemorySegment
// Function    : ClassifySection
// Input       : None (uses member fields Permissions and Details)
// Output      : Fills SectionType with label
// Description : Priority order:
//               1. Named kernel regions ([heap],[stack],[vdso],[vvar])
//               2. Executable permission (perms[2] == 'x') -> TEXT
//               3. Writable permission   (perms[1] == 'w') -> DATA
//               4. Everything else       -> OTHER
//
////////////////////////////////////////////////////////////////////////////////////
void MemorySegment :: ClassifySection()
{
    if(strcmp(Details, "[heap]") == 0)
    {
        strcpy(SectionType, "HEAP");
    }
    else if(strcmp(Details, "[stack]") == 0)
    {
        strcpy(SectionType, "STACK");
    }
    else if(strcmp(Details, "[vdso]") == 0)
    {
        strcpy(SectionType, "VDSO");
    }
    else if(strcmp(Details, "[vvar]") == 0)
    {
        strcpy(SectionType, "VVAR");
    }
    else if(strlen(Permissions) >= 3 && Permissions[2] == 'x')
    {
        strcpy(SectionType, "TEXT");
    }
    else if(strlen(Permissions) >= 2 && Permissions[1] == 'w')
    {
        strcpy(SectionType, "DATA");
    }
    else
    {
        strcpy(SectionType, "OTHER");
    }
}

////////////////////////////////////////////////////////////////////////////////////
//
// Class       : MemorySegment
// Function    : DisplaySegment
// Input       : None
// Output      : Prints one formatted row to stdout
// Description : Prints start, end, size, permissions, section type, and mapping name
//
////////////////////////////////////////////////////////////////////////////////////
void MemorySegment :: DisplaySegment()
{
    printf("%-18lx %-18lx %-10lu %-6s %-8s %s\n",
            StartAddr, EndAddr, SizeKB, Permissions, SectionType, Details);
}

////////////////////////////////////////////////////////////////////////////////////
//
// Class       : ProcessInfo
// Function    : ProcessInfo (Constructor)
// Description : Initialises PID to 0 and clears string buffers
//
////////////////////////////////////////////////////////////////////////////////////
ProcessInfo :: ProcessInfo()
{
    PID = 0;
    memset(Name,    0, sizeof(Name));
    memset(State,   0, sizeof(State));
    memset(Threads, 0, sizeof(Threads));
}

////////////////////////////////////////////////////////////////////////////////////
//
// Class       : ProcessInfo
// Function    : LoadInfo
// Input       : pid - process ID to inspect
// Output      : Returns true on success, false if file cannot be opened
// Description : Opens /proc/<pid>/status and reads Name, State, Threads fields
//
////////////////////////////////////////////////////////////////////////////////////
bool ProcessInfo :: LoadInfo(int pid)
{
    FILE *fp   = NULL;
    char  line[256];
    char  Filename[100];

    PID = pid;
    snprintf(Filename, sizeof(Filename), "/proc/%d/status", pid);

    printf("Accessing the File : %s\n", Filename);

    fp = fopen(Filename, "r");
    if(fp == NULL)
    {
        printf("Unable to access system file : %s\n", Filename);
        printf("Check that PID %d exists.\n", pid);
        return false;
    }

    while(fgets(line, sizeof(line), fp) != NULL)
    {
        if(strncmp(line, "Name:",    5) == 0)  strncpy(Name,    line, sizeof(Name)    - 1);
        if(strncmp(line, "State:",   6) == 0)  strncpy(State,   line, sizeof(State)   - 1);
        if(strncmp(line, "Threads:", 8) == 0)  strncpy(Threads, line, sizeof(Threads) - 1);
    }

    fclose(fp);
    return true;
}

////////////////////////////////////////////////////////////////////////////////////
//
// Class       : ProcessInfo
// Function    : DisplayInfo
// Input       : None
// Output      : Prints process information block to stdout
// Description : Displays PID, Name, State, and Thread count with separators
//
////////////////////////////////////////////////////////////////////////////////////
void ProcessInfo :: DisplayInfo()
{
    printf("-----------------------------------------\n");
    printf("----------PROCESS INFORMATION------------\n");
    printf("-----------------------------------------\n");
    printf("PID     : %d\n", PID);
    printf("%s", Name);
    printf("%s", State);
    printf("%s", Threads);
    printf("-----------------------------------------\n");
}

////////////////////////////////////////////////////////////////////////////////////
//
// Class       : MemoryInspector
// Function    : MemoryInspector (Constructor)
// Description : Initialises PID and SegmentCount to zero
//
////////////////////////////////////////////////////////////////////////////////////
MemoryInspector :: MemoryInspector()
{
    PID          = 0;
    SegmentCount = 0;
}

////////////////////////////////////////////////////////////////////////////////////
//
// Class       : MemoryInspector
// Function    : LoadLayout
// Input       : pid - process ID to inspect
// Output      : Returns true on success, false if file cannot be opened
// Description : Opens /proc/<pid>/maps, parses each line into a MemorySegment
//               and stores it in the Segments array
//
////////////////////////////////////////////////////////////////////////////////////
bool MemoryInspector :: LoadLayout(int pid)
{
    FILE *fp = NULL;
    char  line[512];
    char  Filename[100];

    PID = pid;
    snprintf(Filename, sizeof(Filename), "/proc/%d/maps", pid);

    printf("Accessing the File : %s\n", Filename);

    fp = fopen(Filename, "r");
    if(fp == NULL)
    {
        printf("Unable to access system file : %s\n", Filename);
        return false;
    }

    while(fgets(line, sizeof(line), fp) != NULL)
    {
        if(SegmentCount >= MAX_SEGMENTS) break;

        char          address[100];
        char          perms[10];
        char          offset[20];
        char          dev[20];
        char          details[300] = "";
        unsigned long inode;
        unsigned long start, end;
        int           Ret = 0;

        Ret = sscanf(line, "%s %s %s %s %lu %[^\n]",
                     address, perms, offset, dev, &inode, details);

        if(Ret >= 5)
        {
            sscanf(address, "%lx-%lx", &start, &end);
            Segments[SegmentCount].SetData(start, end, perms, details);
            SegmentCount++;
        }
    }

    fclose(fp);
    return true;
}

////////////////////////////////////////////////////////////////////////////////////
//
// Class       : MemoryInspector
// Function    : DisplayLayout
// Input       : None
// Output      : Prints full memory map table to stdout
// Description : Prints header row then calls DisplaySegment for each entry
//
////////////////////////////////////////////////////////////////////////////////////
void MemoryInspector :: DisplayLayout()
{
    printf("-----------------------------------------\n");
    printf("-----------MEMORY LAYOUT-----------------\n");
    printf("-----------------------------------------\n");
    printf("%-18s %-18s %-10s %-6s %-8s %s\n",
           "START", "END", "SIZE(KB)", "PERMS", "SECTION", "MAPPING");
    printf("-----------------------------------------\n");

    for(int i = 0; i < SegmentCount; i++)
    {
        Segments[i].DisplaySegment();
    }
}

////////////////////////////////////////////////////////////////////////////////////
//
// Class       : MemoryInspector
// Function    : DisplaySummary
// Input       : None
// Output      : Prints section-wise count and total KB to stdout
// Description : Groups all segments by SectionType and totals their sizes
//
////////////////////////////////////////////////////////////////////////////////////
void MemoryInspector :: DisplaySummary()
{
    const char *Types[] = {"TEXT", "DATA", "HEAP", "STACK", "VDSO", "VVAR", "OTHER"};
    int         TypeCount = 7;

    printf("-----------------------------------------\n");
    printf("----------MEMORY SUMMARY-----------------\n");
    printf("-----------------------------------------\n");

    for(int t = 0; t < TypeCount; t++)
    {
        int           count   = 0;
        unsigned long totalKB = 0;

        for(int i = 0; i < SegmentCount; i++)
        {
            if(strcmp(Segments[i].SectionType, Types[t]) == 0)
            {
                count++;
                totalKB += Segments[i].SizeKB;
            }
        }

        if(count > 0)
        {
            printf("%-8s : %d region(s), %lu KB total\n", Types[t], count, totalKB);
        }
    }

    printf("-----------------------------------------\n");
}
