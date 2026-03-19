////////////////////////////////////////////////////////////////////////////////////
//
//  File:              main.c
//  Description:       Entry point for Process Memory Inspector
//  Author:            Pallavi Sable
//  Date:              06/01/2026
//
//  Platform:          Linux only (requires /proc filesystem)
//  Compile:           gcc -o memory_inspector main.c memory_inspector.c
//  Usage:             ./memory_inspector
//                     Then enter a valid PID when prompted.
//
//  Tip:               Run `ps aux` or `pgrep <name>` in another terminal
//                     to find a live PID to inspect.
//
////////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include "memory_inspector.h"

////////////////////////////////////////////////////////////////////////////////////
//
//  Function Name:     main
//  Input:             None (PID entered interactively)
//  Output:            Returns 0 on success, -1 on invalid PID
//  Description:       Prompts user for a PID, validates it, then calls
//                     ShowProcessInformation() and ShowMemoryLayout()
//
////////////////////////////////////////////////////////////////////////////////////
int main()
{
    int pid = 0;

    printf("-----------------------------------------\n");
    printf("-------Memory_Process_Inspector----------\n");
    printf("-----------------------------------------\n");
    printf("Enter the PID of a process to inspect\n");
    printf("(Tip: use `ps aux` or `pgrep <name>` to find a PID)\n");

    if(scanf("%d", &pid) != 1)  
    {
        printf("Invalid input: please enter a numeric PID.\n");
        return -1;
    }

    if(pid <= 0)
    {
        printf("Invalid PID: PID must be a positive integer.\n"); 
        return -1;
    }

    ShowProcessInformation(pid);
    ShowMemoryLayout(pid);

    return 0;
}
