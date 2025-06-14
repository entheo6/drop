#pragma once

#include <unordered_map>

#include <Windows.h>

#include "..\include\resource.h"

class Sound
{
public:
	enum SOUND
	{
		OPEN,
		OPEN_2,
		CLOSE,
		SELECT,
		SELECT_2,
		CANCEL,
		START,
		STOP
	};

private:
	bool* pSoundEnabled;

	std::unordered_map<SOUND, unsigned int> soundMap =
	{
		{ OPEN, IDR_WAVE1 },
		{ OPEN_2, IDR_WAVE2 },
		{ CLOSE, IDR_WAVE3 },
		{ SELECT, IDR_WAVE4 },
		{ SELECT_2, IDR_WAVE5 },
		{ START, IDR_WAVE6 },
		{ STOP, IDR_WAVE7 },
		{ CANCEL, IDR_WAVE8 }
	};

public:
	Sound()
		:pSoundEnabled(nullptr){}
	Sound(bool* enabled)
		:pSoundEnabled(enabled) {}
	void playSound(SOUND);
};