#pragma once

#include "InputRecord.h"
#include <stdio.h>
#include <sstream>
#include <vector>
#include <string>



extern bool g_bPaused;


enum eBreakType
{
	BREAKTYPE_DEFAULT_NONE=0x0,
	BREAKTYPE_FAST=1<<0,
	BREAKTYPE_NORMAL=1<<1
};
// We need more than just line no, otherwise it cannot distinguish between input files.
typedef struct t_BreakState
{
	// The file the break is in.
	char m_szCurrentFile[56];

	// The line number the break is in.
	unsigned int m_nLineNo;

	// The breakpoint type.
	eBreakType m_BreakType;
} BreakState;

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

	// TODO: Vectorize break states for multi-break
	BreakState m_BreakState;

};

extern PlaybackManager * g_pPlaybackManager;