#pragma once

#include "InputRecord.h"
#include <stdio.h>
#include <sstream>
#include <vector>
#include <string>



extern bool g_bPaused;

class PlaybackManager
{
public:

	PlaybackManager(const char*);

	InputRecord * GetCurrentInput();

	InputRecord * GetCurrentInputIndexBased();

	unsigned long GetCurrentInputIndex();

	unsigned long ReloadPlayback();

	bool ReadInputFile();

	void InitPlayback(bool);

	bool IsPlayingBack();

	unsigned long GetTotalFrameCount();

	void DoPlayback(bool, XINPUT_STATE*);

	XINPUT_STATE * GetXInputState();

protected:
private:

	InputRecord * m_pCurrentInput;

	std::vector<InputRecord*> m_Inputs;

	unsigned long m_InputIndex;

	FILE * m_Fp;

	unsigned long m_nTotalFrameCount;

	unsigned long m_RuntoLineNo;

	unsigned long m_WalktoLineNo;

	unsigned long m_TotalFrameCountOfInputFile;

	unsigned long m_CurrentFrame;

	unsigned long m_FrameToNext;

	bool m_bPlayingBack;

	XINPUT_STATE * m_pGamePadState;

};

extern PlaybackManager * g_pPlaybackManager;
