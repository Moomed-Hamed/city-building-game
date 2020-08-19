#pragma once
#include <Windows.h>

#define Alloc(type, size) (type*)VirtualAlloc(0, (size) * sizeof(type), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE)
#define Free(ptr) VirtualFree(ptr, 0, MEM_RELEASE)