#include "SoundSystem.h"
#include <assert.h>
#include <algorithm>

#define Assert(formula, message) assert(formula && message)
#define Assert(message) assert(0 && message)

SoundClass::SoundClass()
{
}

SoundClass::SoundClass(const SoundClass& soundClass)
{
}

SoundClass::~SoundClass()
{
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

SoundClass& SoundClass::GetInstance()
{
	static SoundClass* instance;

	if (instance == nullptr)
	{
		instance = new SoundClass();
	}
	return *instance;
}

bool SoundClass::Initialize(HWND hWnd)
{
	HRESULT hr;

	if (FAILED(hr = DirectSoundCreate8(NULL, &_directSound, NULL)))
	{
		return false;
	}

	if (FAILED(hr = _directSound->SetCooperativeLevel(hWnd, DSSCL_PRIORITY)))
	{
		return false;
	}

	DSBUFFERDESC bufferDesc;
	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME;

	return true;
}

void SoundClass::Terminate()
{
	ClearAllSound();

	if (_primaryBuffer)
	{
		_primaryBuffer->Release();
		_primaryBuffer = 0;
	}
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

void SoundClass::AddSound(std::string filename, std::string soundname, int volume)
{
	int soundvolume = 0;
	if (volume > 100 || volume < 0 )
	{
		Assert("volume setting failed. volume is set over 100 or under 0");
	}
	else
	{
		soundvolume = volume * -100;
	}

	Sound* soundstruct = new Sound;
	IDirectSoundBuffer8* tempsound;
	LoadWaveFile(filename, &tempsound);

	soundstruct->fileName = filename;
	soundstruct->soundName = soundname;
	soundstruct->soundBuffer = tempsound;
	soundstruct->volume = soundvolume;

	if (FAILED(tempsound->SetCurrentPosition(0)))
	{
		Assert("PlayWaveFile function -> SetCurrentPosition 01 is failed");
		return;
	}

	if (FAILED(tempsound->SetVolume(soundvolume)))
	{
		Assert("PlayWaveFile function -> SetVolume 01 is failed");
		return;
	}

	_soundBuffer.push_back(soundstruct);
}

void SoundClass::RemoveSound(int index)
{
	TerminateWaveFile(_soundBuffer[index]);
	_soundBuffer.erase(_soundBuffer.begin() + index);
}

void SoundClass::ClearAllSound()
{
	for (auto sound : _soundBuffer)
	{
		TerminateWaveFile(sound);
	}
	_soundBuffer.clear();
}

void SoundClass::Setvolume(int index, int volume)
{
	int soundvolume = 0;
	if (volume > 100 || volume < 0)
	{
		Assert("volume setting failed. volume is set over 100 or under 0");
	}
	else
	{
		soundvolume = (100 - volume) * -100;
	}

	_soundBuffer[index]->volume = soundvolume;

	if (FAILED(_soundBuffer[index]->soundBuffer->SetVolume(soundvolume)))
	{
		Assert("PlayWaveFile function -> SetVolume 01 is failed");
		return;
	}
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

int SoundClass::FindIndexOfSound(std::string soundname)
{
	int index = 0;

	for (auto sound : _soundBuffer)
	{
		if (sound->fileName.compare(soundname))
		{
			return index;
		}
		index++;

	}
}

Sound& SoundClass::FindSound(std::string soundname)
{
	for (auto sound : _soundBuffer)
	{
		if (sound->fileName.compare(soundname))
		{
			return *sound;
		}
	}
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

void SoundClass::Play(int index)
{
	if (FAILED(_soundBuffer[index]->soundBuffer->Play(0, 0, 0)))
	{
		Assert("PlayWaveFile function -> Play 01 is failed");
		return;
	}
}

void SoundClass::Loop(int index)
{
	if (FAILED(_soundBuffer[index]->soundBuffer->Play(0, 0, DSBPLAY_LOOPING)))
	{
		Assert("PlayWaveFile function -> Play 01 is failed");
		return;
	}
}

void SoundClass::Pause(int index)
{
	// 사운드 일시 중지
	_soundBuffer[index]->soundBuffer->Stop();
}

void SoundClass::Stop(int index)
{
	// 사운드 정지
	_soundBuffer[index]->soundBuffer->Stop();

	// 재생 위치 초기화
	_soundBuffer[index]->soundBuffer->SetCurrentPosition(0);
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

bool SoundClass::LoadWaveFile(std::string filename, IDirectSoundBuffer8** secondaryBuffer)
{
	// Open the wave file in binary.
	FILE* filePtr = nullptr;
	int error = fopen_s(&filePtr, filename.c_str(), "rb");
	if (error != 0)
	{
		Assert("LoadWaveFile function -> fopen_s is failed");
		return false;
	}

	// Read in the wave file header.
	WaveHeaderType waveFileHeader;
	unsigned int count = fread(&waveFileHeader, sizeof(waveFileHeader), 1, filePtr);
	if (count != 1)
	{
		Assert("LoadWaveFile function -> fread is failed");
		return false;
	}

	// LIST 청크가 있다면 건너뛰기
	if (strncmp(waveFileHeader.dataChunkId, "LIST", 4) == 0)
	{
		fseek(filePtr, waveFileHeader.dataSize, SEEK_CUR);

		// "data" 청크 정보 읽기
		count = fread(&waveFileHeader.dataChunkId, sizeof(waveFileHeader.dataChunkId), 1, filePtr);
		if (count != 1)
		{
			// fread 실패
			Assert("LoadWaveFile function -> fread is failed");
			fclose(filePtr);
			return false;
		}

		count = fread(&waveFileHeader.dataSize, sizeof(waveFileHeader.dataSize), 1, filePtr);
		if (count != 1)
		{
			// fread 실패
			Assert("LoadWaveFile function -> fread is failed");
			fclose(filePtr);
			return false;
		}
	}

	// Check that the chunk ID is the RIFF format.
	if ((waveFileHeader.chunkId[0] != 'R') || (waveFileHeader.chunkId[1] != 'I') ||
		(waveFileHeader.chunkId[2] != 'F') || (waveFileHeader.chunkId[3] != 'F'))
	{
		return false;
	}

	// Check that the file format is the WAVE format.
	if ((waveFileHeader.format[0] != 'W') || (waveFileHeader.format[1] != 'A') ||
		(waveFileHeader.format[2] != 'V') || (waveFileHeader.format[3] != 'E'))
	{
		return false;
	}

	// Check that the sub chunk ID is the fmt format.
	if ((waveFileHeader.subChunkId[0] != 'f') || (waveFileHeader.subChunkId[1] != 'm') ||
		(waveFileHeader.subChunkId[2] != 't') || (waveFileHeader.subChunkId[3] != ' '))
	{
		return false;
	}

	// Check that the audio format is WAVE_FORMAT_PCM.
	if (waveFileHeader.audioFormat != WAVE_FORMAT_PCM)
	{
		return false;
	}

	// Check that the wave file was recorded in stereo format.
	if (waveFileHeader.numChannels != 2)
	{
		return false;
	}

	// Check that the wave file was recorded at a sample rate of 44.1 KHz.
	if (waveFileHeader.sampleRate != 48000)
	{
		return false;
	}

	// Ensure that the wave file was recorded in 16 bit format.
	if (waveFileHeader.bitsPerSample != 16)
	{
		return false;
	}

	// Check for the data chunk header.
	if ((waveFileHeader.dataChunkId[0] != 'd') || (waveFileHeader.dataChunkId[1] != 'a') ||
		(waveFileHeader.dataChunkId[2] != 't') || (waveFileHeader.dataChunkId[3] != 'a'))
	{
		return false;
	}

	// Set the wave format of secondary buffer that this wave file will be loaded onto.
	WAVEFORMATEX waveFormat;
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nSamplesPerSec = 44100;
	waveFormat.wBitsPerSample = 16;
	waveFormat.nChannels = 2;
	waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;

	// Set the buffer description of the secondary sound buffer that the wave file will be loaded onto.
	DSBUFFERDESC bufferDesc;
	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_CTRLVOLUME;
	bufferDesc.dwBufferBytes = waveFileHeader.dataSize;
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = &waveFormat;
	bufferDesc.guid3DAlgorithm = GUID_NULL;

	// Create a temporary sound buffer with the specific buffer settings.
	IDirectSoundBuffer* tempBuffer = nullptr;
	if (FAILED(_directSound->CreateSoundBuffer(&bufferDesc, &tempBuffer, NULL)))
	{
		Assert("LoadWaveFile function -> CreateSoundBuffer is failed");
		return false;
	}

	// Test the buffer format against the direct sound 8 interface and create the secondary buffer.
	if (FAILED(tempBuffer->QueryInterface(IID_IDirectSoundBuffer8, (void**)&*secondaryBuffer)))
	{
		Assert("LoadWaveFile function -> QueryInterface is failed");
		return false;
	}

	// Release the temporary buffer.
	tempBuffer->Release();
	tempBuffer = 0;

	// Move to the beginning of the wave data which starts at the end of the data chunk header.
	fseek(filePtr, sizeof(WaveHeaderType), SEEK_SET);

	// Create a temporary buffer to hold the wave file data.
	unsigned char* waveData = new unsigned char[waveFileHeader.dataSize];
	if (!waveData)
	{
		Assert("LoadWaveFile function -> Create a temporary buffer to hold the wave file data is failed");
		return false;
	}

	// Read in the wave file data into the newly created buffer.
	count = fread(waveData, 1, waveFileHeader.dataSize, filePtr);
	if (count != waveFileHeader.dataSize)
	{
		Assert("LoadWaveFile function -> fread is failed");
		return false;
	}

	// Close the file once done reading.
	error = fclose(filePtr);
	if (error != 0)
	{
		Assert("LoadWaveFile function -> fclose is failed");
		return false;
	}

	// Lock the secondary buffer to write wave data into it.
	unsigned char* bufferPtr = nullptr;
	unsigned long bufferSize = 0;
	if (FAILED((*secondaryBuffer)->Lock(0, waveFileHeader.dataSize, (void**)&bufferPtr, (DWORD*)&bufferSize, NULL, 0, 0)))
	{
		Assert("LoadWaveFile function -> Lock is failed");
		return false;
	}

	// Copy the wave data into the buffer.
	memcpy(bufferPtr, waveData, waveFileHeader.dataSize);

	// Unlock the secondary buffer after the data has been written to it.
	if (FAILED((*secondaryBuffer)->Unlock((void*)bufferPtr, bufferSize, NULL, 0)))
	{
		Assert("LoadWaveFile function -> Unlock is failed");
		return false;
	}

	// Release the wave data since it was copied into the secondary buffer.
	delete[] waveData;
	waveData = 0;

	return true;
}

void SoundClass::TerminateWaveFile(Sound* index)
{
	if (index->soundBuffer)
	{
		(index->soundBuffer)->Release();
		index->soundBuffer = 0;
	}

	delete index;
}