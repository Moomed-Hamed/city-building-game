//-----------------------------------------------------------------
//  Name        : File Utilities
//  Purpose     : Make file handling simple
//  Last Updated: 12/8/2020
//-----------------------------------------------------------------

#pragma once

#include "types.h"
#include "memory.h"
#include "debug.h"

#include <fileapi.h>

void read_entire_file_into_memory(byte* memory, const char* path)
{
	DWORD BytesRead;
	HANDLE os_file = CreateFile(path, GENERIC_READ | GENERIC_WRITE, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	LARGE_INTEGER size;
	GetFileSizeEx(os_file, &size);
	ReadFile(os_file, memory, size.QuadPart, &BytesRead, NULL);
}
void write_entire_file_to_disk(const char* path, void* buf, uint size)
{
}

void create_file(const char* path)
{
	CreateFile(path, GENERIC_READ | GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
}
void delete_file(const char* path)
{
	DeleteFile(path);
}

int get_file_size(const char* path)
{
	HANDLE file = CreateFile(path, GENERIC_READ | GENERIC_WRITE, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	LARGE_INTEGER size;
	GetFileSizeEx(file, &size);
	return (int)size.QuadPart;
}

// skip chars until a newline is found
int file_get_next_line_size(char* memory, int file_size, int offset = 0)
{
	int i = 0;

	while (offset++ < file_size)
	{
		if (memory[i++] == '\n') return i;
	}

	return -1;
}
int file_store_next_line(char* memory, int file_size, char* buf, int offset = 0)
{
	char c = 69;
	int line_size = 0;

	while (offset < file_size)
	{
		c = memory[line_size];

		if (c == ' ' || c == '\t') continue;

		buf[line_size++] = c;

		if (c == '\n') return line_size;
	}

	return -1;
}