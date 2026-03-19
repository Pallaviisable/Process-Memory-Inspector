////////////////////////////////////////////////////////////////////////////////////
//
//  File:              memory_inspector.c
//  Description:       Implementation of Process Memory Inspector functions
//  Author:            Pallavi Sable
//  Date:              06/01/2026
//
//  Platform:          Linux only (requires /proc filesystem)
//
////////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "memory_inspector.h"

////////////////////////////////////////////////////////////////////////////////////
//
//  Function Name:     GetSectionType
//  Input:             details — mapping name from /proc/<pid>/maps (may be empty)
//                     perms   — 4-char permission string e.g. "rwxp", "r--p"
//                     section — caller-allocated output buffer (min 10 bytes)
//  Output:            Writes section label into 'section'
//  Description:       Priority order:
//                       1. Named kernel regions ([heap], [stack], [vdso], [vvar])
//                       2. Executable permission  → TEXT
//                       3. Writable permission    → DATA
//                       4. Everything else        → OTHER
//
////////////////////////////////////////////////////////////////////////////////////
void GetSectionType(char *details, char *perms, char *section)
{
    if(strcmp(details, "[heap]") == 0)
    {
        strcpy(section, "HEAP");
    }
    else if(strcmp(details, "[stack]") == 0)
    {
        strcpy(section, "STACK");
    }
    else if(strcmp(details, "[vdso]") == 0)
    {
        strcpy(section, "VDSO");
    }
    else if(strcmp(details, "[vvar]") == 0)
    {
        strcpy(section, "VVAR");
    }
    
    else if(strlen(perms) >= 3 && perms[2] == 'x')  
    {
        strcpy(section, "TEXT");
    }
    else if(strlen(perms) >= 2 && perms[1] == 'w')  
    {
        strcpy(section, "DATA");
    }
    else
    {
        strcpy(section, "OTHER");
    }
}

////////////////////////////////////////////////////////////////////////////////////
//
//  Function Name:     ShowMemoryLayout
//  Input:             pid — process ID to inspect (must be > 0)
//  Output:            Formatted memory map printed to stdout
//  Description:       Opens /proc/<pid>/maps and parses each line.
//                     Prints: start, end, size(KB), permissions, section, name.
//                     Anonymous mappings are labelled "Anonymous".
//
////////////////////////////////////////////////////////////////////////////////////
void ShowMemoryLayout(int pid)
{
    FILE *fp    = NULL;
    char  line[256];            
    char  Filename[100];

    snprintf(Filename, sizeof(Filename), "/proc/%d/maps", pid);  
    printf("Accessing the File : %s\n", Filename);

    fp = fopen(Filename, "r");
    if(fp == NULL)
    {
        printf("Unable to access system file: %s\n", Filename);  
        printf("Check that PID %d exists and you have permission to read it.\n", pid);
        return;
    }

    printf("-----------------------------------------\n");
    printf("--------------MEMORY LAYOUT--------------\n");
    printf("-----------------------------------------\n");
    printf("%-18s %-18s %-10s %-6s %-8s %s\n",
           "START", "END", "SIZE(KB)", "PERMS", "SECTION", "MAPPING");  
    printf("-----------------------------------------\n");

    while(fgets(line, sizeof(line), fp) != NULL)
    {
        char          address[100];
        char          perms[10];
        char          offset[20];
        char          dev[20];
        char          details[300] = "";
        unsigned long inode;
        unsigned long start, end;
        unsigned long sizekb;
        char          section[50];
        int           Ret = 0;

        Ret = sscanf(line, "%s %s %s %s %lu %[^\n]",
                     address, perms, offset, dev, &inode, details);

        if(Ret >= 5)
        {
            sscanf(address, "%lx-%lx", &start, &end);

            // Fixed: guard against end < start (malformed line)
            sizekb = (end > start) ? (end - start) / 1024 : 0;

            GetSectionType(details, perms, section);

            if(strlen(details) == 0)
            {
                strcpy(details, "Anonymous");
            }

            printf("%-18lx %-18lx %-10lu %-6s %-8s %s\n",
                   start, end, sizekb, perms, section, details);  // Fixed: aligned with headers
        }
    }

    fclose(fp);
}

////////////////////////////////////////////////////////////////////////////////////
//
//  Function Name:     ShowProcessInformation
//  Input:             pid — process ID to inspect (must be > 0)
//  Output:            Process name, PID, state, and thread count printed to stdout
//  Description:       Opens /proc/<pid>/status and filters for key fields:
//                     Name, Pid, State, Threads.
//                     Prints a separator after the block for clean formatting.
//
////////////////////////////////////////////////////////////////////////////////////
void ShowProcessInformation(int pid)
{
    FILE *fp    = NULL;
    char  line[256];            
    char  Filename[100];

    snprintf(Filename, sizeof(Filename), "/proc/%d/status", pid);  
    printf("Accessing the File : %s\n", Filename);

    fp = fopen(Filename, "r");
    if(fp == NULL)
    {
        printf("Unable to access system file: %s\n", Filename);    
        printf("Check that PID %d exists and you have permission to read it.\n", pid);
        return;
    }

    printf("-----------------------------------------\n");
    printf("----------PROCESS INFORMATION------------\n");
    printf("-----------------------------------------\n");

    while(fgets(line, sizeof(line), fp) != NULL)
    {
        if(strncmp(line, "Name:",    5) == 0 ||
           strncmp(line, "Pid:",     4) == 0 ||
           strncmp(line, "State:",   6) == 0 ||
           strncmp(line, "Threads:", 8) == 0)
        {
            printf("%s", line);
        }
    }

    printf("-----------------------------------------\n");
    fclose(fp);
}
