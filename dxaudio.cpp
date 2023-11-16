// Beginning Game Programming, Second Edition
// dxaudio.cpp - DirectSound framework header file

#include "dxaudio.h"

CSoundManager *dsound;

int Init_DirectSound(HWND hwnd)
{
    HRESULT result;

    //create DirectSound manager object
    dsound = new CSoundManager();

    //initialize DirectSound
    result = dsound->Initialize(hwnd, DSSCL_PRIORITY);
    if (result != DS_OK)
        return 0;

    //set the primary buffer format
    result = dsound->SetPrimaryBufferFormat(2, 22050, 16);
    if (result != DS_OK)
        return 0;

    //return success
    return 1;
}

CSound *LoadSound(char *filename)
{
    HRESULT result;

    //create local reference to wave data
    CSound *wave;

    //attempt to load the wave file
    result = dsound->Create(&wave, filename);
    if (result != DS_OK)
        return NULL;

    //return the wave
    return wave;
}

void PlaySound(CSound *sound)
{
    sound->Play();
}

void LoopSound(CSound *sound)
{
    sound->Play(0, DSBPLAY_LOOPING);
}

void StopSound(CSound *sound)
{
    sound->Stop();
}
