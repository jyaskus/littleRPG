// Beginning Game Programming, Second Edition
// dxaudio.h - DirectSound framework header file


#ifndef _DXAUDIO_H
#define _DXAUDIO_H

#include "dsutil.h"

//primary DirectSound object
extern CSoundManager *dsound;

//function prototypes
int Init_DirectSound(HWND);
CSound *LoadSound(char *);
void PlaySound(CSound *);
void LoopSound(CSound *);
void StopSound(CSound *);


#endif

