#pragma once
#include <fileapi.h>

typedef unsigned int uint;
typedef char byte;

struct File
{
	uint offset;
	byte* contents;
};

void create_file(const char* name)
{
	CreateFile(name, GENERIC_READ | GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
}

void delete_file(const char* name)
{
	DeleteFile(name);
}

int get_file_size(const char* name)
{
	HANDLE file = CreateFile(name, GENERIC_READ | GENERIC_WRITE, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	LARGE_INTEGER size;
	GetFileSizeEx(file, &size);
	return (int)size.QuadPart;
}

void read_entire_file(const char* name, void* buf)
{
	DWORD BytesRead;
	HANDLE file = CreateFile(name, GENERIC_READ | GENERIC_WRITE, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	LARGE_INTEGER size;
	GetFileSizeEx(file, &size);
	ReadFile(file, buf, size.QuadPart, &BytesRead, NULL);
}

void read_file(const char* name, void* buf, uint size)
{
	DWORD BytesRead;
	HANDLE file = CreateFile(name, GENERIC_READ | GENERIC_WRITE, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	ReadFile(file, buf, size, &BytesRead, NULL);
}

void write_file(const char* name, void* buf, uint size)
{
}

void create_directory(const char* name)
{
	//__mkdir(name);
}
void delete_directory(const char* name);
int get_directory_file_count(const char* directory);
int get_directory_file_names(const char** names);
