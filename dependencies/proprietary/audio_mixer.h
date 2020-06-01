#pragma once
#include "intermediary.h"

typedef ALuint Audio;

#define MAX_AUDIO_SOURCES 16

struct Audio_Player
{
	ALuint Sources[MAX_AUDIO_SOURCES];
};

Audio LoadAudio(const char* path)
{
	uint Format;
	uint SampleRate;
	uint Size;
	char* Data;

	FILE* file = fopen(path, "rb");
	fread(&Format    , sizeof(uint), 1, file);
	fread(&SampleRate, sizeof(uint), 1, file);
	fread(&Size      , sizeof(uint), 1, file);
	Data = Alloc(char, Size);
	fread(Data, sizeof(char), Size, file);
	fclose(file);

	ALuint bufferid = 0;
	alGenBuffers(1, &bufferid);
	alBufferData(bufferid, Format, Data, Size, SampleRate);

	ALuint SourceID;
	alGenSources(1, &SourceID);
	alSourcei(SourceID, AL_BUFFER, bufferid);

	//out("Audio: " << SourceID);

	Free(Data);
	return SourceID;
}

force_inline void play_audio(ALuint source_id) { alSourcePlay(source_id); }

struct Audio_Manager
{
	ALuint music;
	ALuint user_interface;
	ALuint player;
	ALuint world;
	ALuint enemies;
};

struct Audio_Data
{

};

struct Sound
{
	int size;
	char* data;
};

struct Audio_Source
{
	Sound sounds[16];
};

void mix_that_shit(Sound s1, Sound s2, float mix);
void crank_it(Audio_Source src, float volume_level);