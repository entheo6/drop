#include "sound.h"

void Sound::playSound(SOUND sound)
{
	if (*pSoundEnabled)
		PlaySound(MAKEINTRESOURCE(soundMap[sound]), GetModuleHandle(NULL), SND_RESOURCE | SND_SYNC);
}