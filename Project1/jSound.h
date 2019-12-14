#ifndef __JSOUND_H__
#define __JSOUND_H__

#include "junks.h"
#include "jDirectXheader.h"

class jSound
{
public:
	static jSound& GetInst() { static jSound inst; return inst; }

private:
	jSound();
	~jSound();

	struct WaveHeaderType
	{
		char chunkId[4];
		unsigned long chunkSize;
		char format[4];
		char subChunkId[4];
		unsigned long subChunkSize;
		unsigned short audioFormat;
		unsigned short numChannels;
		unsigned long sampleRate;
		unsigned long bytesPerSecond;
		unsigned short blockAlign;
		unsigned short bitsPerSample;
		char dataChunkId[4];
		unsigned long dataSize;
	};

public:
	bool Initialize(HWND);
	void Release();
	bool PlayWaveFile();
	bool LoadWaveFile(string _filename);

private:
	IDirectSound8 * m_DirectSound = nullptr;
	IDirectSoundBuffer* m_primaryBuffer = nullptr;
	IDirectSoundBuffer8* m_secondaryBuffer = nullptr;
};

#endif