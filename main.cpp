////////////////////////////////////////////////////////////////////////////////////
//
// File        : main.cpp
// Description : Entry point for Process Memory Inspector
// Author      : Pallavi Sable
// Date        : 2026
//
// Platform    : Linux only (requires /proc filesystem)
// Compile     : g++ -o memory_inspector memory_inspector.cpp main.cpp
// Run         : ./memory_inspector
//
////////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include "memory_inspector.h"
using namespace std;

int main()
{
    int pid = 0;

    cout << "-----------------------------------------" << endl;
    cout << "-------Process Memory Inspector----------" << endl;
    cout << "----------- C++ OOP Version -------------" << endl;
    cout << "-----------------------------------------" << endl;
    cout << "Enter the PID of process to inspect : ";
    cin  >> pid;

    if(pid <= 0)
    {
        cout << "Invalid PID entered." << endl;
        return -1;
    }

    // Load and display process information
    ProcessInfo Info;
    if(!Info.LoadInfo(pid))
        return -1;
    Info.DisplayInfo();

    // Load and display memory layout
    MemoryInspector Inspector;
    if(!Inspector.LoadLayout(pid))
        return -1;
    Inspector.DisplayLayout();
    Inspector.DisplaySummary();

    return 0;
}

/*
    SECTION TYPES REFERENCE:
    TEXT    : Executable code/instructions
    DATA    : Global and static variables (writable)
    HEAP    : Dynamically allocated memory (malloc/new)
    STACK   : Local variables and function call frames
    VDSO    : Virtual Dynamic Shared Object
    VVAR    : Virtual Variables (fast kernel data access)
    OTHER   : Shared libraries, mapped files, anonymous regions
*/
