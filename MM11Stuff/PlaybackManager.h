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

	bool ReadMutilLevelInputFile(const char*, unsigned long, unsigned long*, unsigned long*);

	bool ReadInputFile();

	void InitPlayback(bool);

	bool IsPlayingBack();

	unsigned long GetTotalFrameCount();

	void DoPlayback(bool, XINPUT_STATE*);

	XINPUT_STATE * GetXInputState();

#pragma region PublicForExternalOSD
	InputRecord * m_pCurrentInput;

	unsigned long m_CurrentFrame;

	unsigned long m_nTotalFrameCount;

	bool m_bPlayingBack;

	char m_szCurrentManagerState[120];

	bool *m_bLoading;
#pragma endregion


protected:
private:

	const char * m_szDefaultFileName = "megaman.rec";

	std::vector<InputRecord*> m_Inputs;

	unsigned long m_InputIndex;

	FILE * m_Fp;

	FILE * m_pSegmentedFile;

	unsigned long long m_RuntoLineNo;

	unsigned long long m_WalktoLineNo;

	unsigned long m_FrameToNext;

	XINPUT_STATE * m_pGamePadState;

};

extern PlaybackManager * g_pPlaybackManager;