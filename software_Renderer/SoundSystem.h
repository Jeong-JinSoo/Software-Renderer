#pragma once
#include <windows.h>
#include <mmsystem.h>
#include <dsound.h>
#include <stdio.h>
#include <assert.h>

#include <vector>
#include <string>

#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dxguid")
#pragma comment(lib, "winmm.lib")

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
	unsigned long bytesPerSample;
	unsigned short blockAlign;
	unsigned short bitsPerSample;
	char dataChunkId[4];
	unsigned long dataSize;
};

struct Sound
{
	std::string soundName;
	std::string fileName;
	IDirectSoundBuffer8* soundBuffer;
	int volume; // 0~100
};

class SoundClass
{
public:
	SoundClass();
	SoundClass(const SoundClass& soundClass);
	~SoundClass();
	
	// 싱글턴 형태의 반환
	static SoundClass& GetInstance();

	//사운드 초기화
	bool Initialize(HWND hWnd);
	void Terminate();

	//사운드 관리
	void AddSound(std::string filename, std::string soundname, int volume);
	void RemoveSound(int index);
	void ClearAllSound();
	void Setvolume(int index, int volume);

	//사운드 벡터에서 찾는게 몇번째 인덱스인지 찾기
	int FindIndexOfSound(std::string soundname);
	//그냥 사운드 자체를 가져오기
	Sound& FindSound(std::string soundname);

	// 사운드 재생 관련
	void Play(int index);
	void Loop(int index);
	void Pause(int index);
	void Stop(int index);

private:
	bool LoadWaveFile(std::string filename, IDirectSoundBuffer8** secondaryBuffer);
	void TerminateWaveFile(Sound* index);

private:
	IDirectSound8* _directSound = nullptr;
	IDirectSoundBuffer* _primaryBuffer = nullptr;
	std::vector<Sound*> _soundBuffer;
};

