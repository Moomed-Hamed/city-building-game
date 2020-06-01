#pragma once

//-----------------------------------------------------------------
//  Author:         Mohamed Hamed
//  Last Updated:   Date: 10/5/2020
//  Description :   This is *WINDOWS ONLY* file handling code
//-----------------------------------------------------------------

#include <Windows.h>
#include <iostream>
#include <GLM/glm.hpp>

#define out(val) std::cout << ' ' << val << '\n'

#define Alloc(type, size) new type[size]
#define Free(ptr) delete[] ptr

typedef unsigned int uint;

// read from file into buf until newline or EOF is found
int get_line(FILE* file, char* buf)
{
	int i = 0;
	char c = ' ';

	while (1)
	{
		c = fgetc(file);

		if (!i && (c == ' ' || c == '\t')) continue;

		if (c == EOF) return EOF;

		buf[i++] = c;

		if (c == '\n') return 0;
	}
}

// skip chars in file until a newline or EOF is found
void skip_line(FILE* file)
{
	char c = ' ';

	while (1)
	{
		c = fgetc(file);
		if (c == '\n' || c == EOF) return;
	}
}

// write from buf into file until newline is found
void write_line(FILE* file, char* buf)
{
	int i = 0;
	do fputc(buf[i], file); while (buf[i++] != '\n');
}

struct File_Name
{
	char* Name;
	char* Extension; // txt, jpg, wav, etc.
};

//WARNING: there is a harcoded limit to the number of files here
#define MAX_DIRECTORY_FILES 256
struct Directory
{
	uint NumFiles;
	File_Name Files[MAX_DIRECTORY_FILES];
	//char* Path; // not sure if this should be supported
};

// fills a Directory struct (involves allocating memory)
// free this structure's memory with the free_directory function
void parse_directory(Directory* dir, const char* path)
{
	WIN32_FIND_DATA FoundFile;
	HANDLE Find = NULL;

	char filepath[256] = {};
	sprintf(filepath, "%s\\*.*", path); // file mask: *.* = get everything

	Find = FindFirstFile(filepath, &FoundFile);
	if (Find == INVALID_HANDLE_VALUE) { printf("Path not found: [%s]\n", path); return; }

	//FindFirstFile always returns "." & ".." as first two directories
	while (!strcmp(FoundFile.cFileName, ".") || !strcmp(FoundFile.cFileName, "..")) FindNextFile(Find, &FoundFile);

	uint num_files = 0; // for readability
	do
	{
		uint length = strlen(FoundFile.cFileName);

		dir->Files[num_files].Name = Alloc(char, length + 1);
		memset(dir->Files[num_files].Name, 0, length + 1);

		for (uint i = 0, j = 0; i < length; ++i)
		{
			if (FoundFile.cFileName[i] == '.')
				dir->Files[num_files].Extension = dir->Files[num_files].Name + i + 1;
			else
				dir->Files[num_files].Name[i] = FoundFile.cFileName[i];
		}

		++num_files;

	} while (FindNextFile(Find, &FoundFile));

	FindClose(Find);

	dir->NumFiles = num_files;

	//	out("Finished parsing " << path << "/ and found " << num_files << " files");
}

// prints the file count, names, and extensions to std output
void print_directory(Directory dir)
{
	out("directory contains " << dir.NumFiles << " files");
	for (uint i = 0; i < dir.NumFiles; ++i) 
		printf(" %d: %s.%s\n", i + 1, dir.Files[i].Name, dir.Files[i].Extension);
}

// frees all memory used by this directory struct
void free_directory(Directory* dir)
{
	for (uint i = 0; i < dir->NumFiles; ++i) Free(dir->Files[i].Name);
	*dir = {};
}

#undef Alloc
#undef Free