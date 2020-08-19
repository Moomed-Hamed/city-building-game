//-----------------------------------------------------------------
//  Name        : Directory Utilities
//  Purpose     : Provide directory handling library
//  Last Updated: 9/8/2020
//-----------------------------------------------------------------

#pragma once

#include "types.h"
#include "debug.h"
#include "memory.h"

#define DIRECTORY_ERROR(str) std::cout << "DIRECTORY ERROR: " << str << '\n';

//WARNING: harcoded file name limit here
#define MAX_DIRECTORY_FILES 256
struct Directory
{
	uint num_files;
	char* names[MAX_DIRECTORY_FILES];
};

// allocates memory & fills directory struct. free the memory with free_directory
void parse_directory(Directory* dir, const char* path)
{
	char filepath[256] = {};
	snprintf(filepath, 256, "%s\\*.*", path); // file mask: *.* = get everything

	WIN32_FIND_DATA FoundFile;
	HANDLE Find = FindFirstFile(filepath, &FoundFile);
	if (Find == INVALID_HANDLE_VALUE) { print("Path not found: [%s]\n", path); return; }

	//FindFirstFile always returns "." & ".." as first two directories
	while (!strcmp(FoundFile.cFileName, ".") || !strcmp(FoundFile.cFileName, "..")) FindNextFile(Find, &FoundFile);

	uint num_files = 0; // for readability
	do
	{
		uint length = strlen(FoundFile.cFileName);

		dir->names[num_files] = Alloc(char, length + 1);
		dir->names[num_files][length] = 0;
		memcpy(dir->names[num_files], FoundFile.cFileName, length);

		++num_files;

	} while (FindNextFile(Find, &FoundFile));

	FindClose(Find);

	dir->num_files = num_files;

	//	out("Finished parsing " << path << "/ and found " << num_files << " files");
}

// prints file count, names, and extensions to std output
void print_directory(Directory dir)
{
	print("directory contains %d files", dir.num_files);
	for (uint i = 0; i < dir.num_files; ++i)
		print(" %d: %s\n", i + 1, dir.names[i]);
}

// frees all memory used by this directory struct
void free_directory(Directory* dir)
{
	for (uint i = 0; i < dir->num_files; ++i) Free(dir->names[i]);
	*dir = {};
}

/*
char* get_file_names_without_extension(Directory dir)
{
	char* buf;

	for (uint i = 0; i < dir.num_files; i++)
	{
		uint length = strlen(dir.names[i]);

		for (uint j = 0; i < length; ++i)
		{
			if (dir.names[i][j] == '.')
				buf[num_files].extension = dir->file_names[num_files].name + i + 1;
			else
				buf[num_files].name[i] = FoundFile.cFileName[i];
		}
	}
}

char* get_file_extension(Directory dir)
{
	char* buf;

	for (uint i = 0; i < dir.num_files; i++)
	{
		uint length = strlen(dir.names[i]);

		for (uint j = 0; i < length; ++i)
		{
			if (dir.names[i][j] == '.')
				buf[num_files].extension = dir->file_names[num_files].name + i + 1;
			else
				buf[num_files].name[i] = FoundFile.cFileName[i];
		}
	}
}
*/