#include "stdafx.h"
#include "MM11.h"
#include "PlaybackManager.h"

bool g_bPaused = false;

PlaybackManager * g_pPlaybackManager = nullptr;

// From MM11.h
oXInputGetState original_XInputGetState = (oXInputGetState)(0x0);////(oXInputGetState)(*(unsigned long long*)XINPUT_IAT_ADDRESS);

// From MM11.h
unsigned long WINAPI XInputGetState_Hook(unsigned long dwUserIndex, XINPUT_STATE* pInputState)
{
	static bool bOnce = false;
	//static unsigned long dwCalledCount = 0;
	static unsigned long packetNum2 = 0;

	if (!bOnce) {
		DebugOutput("XInputGetState_Hook once.");
		bOnce = true;
	}

	if (!pInputState)
		return ERROR_SUCCESS;

	auto * p = &pInputState->Gamepad;

	if (g_pPlaybackManager)
	{
		if (g_pPlaybackManager->IsPlayingBack() && g_bPlaybackSync)
		{
			g_pPlaybackManager->DoPlayback(g_bDidFrameStep, pInputState);
			g_bPlaybackSync = false;
			g_bDidFrameStep = false;

			//DebugOutput("DoPlayback success, returning.");
			return ERROR_SUCCESS;
		}
		else
		{
			if (!pInputState || dwUserIndex > 0)
				return ERROR_DEVICE_NOT_CONNECTED;

			if (!pInputState)
				return ERROR_SUCCESS;

			DWORD dwOldProt;
			VirtualProtect((LPVOID)&original_XInputGetState, 0x8, PAGE_EXECUTE_READWRITE, &dwOldProt);

			if (*(unsigned long long*)(original_XInputGetState) == 0x0)
				return ERROR_SUCCESS;

			//DebugOutput("Returning original XInputGetState, not playing back or playbackSync is false.");
			return original_XInputGetState(dwUserIndex, pInputState);
		}
	}

	//DebugOutput("No playback at all..");
	return original_XInputGetState(dwUserIndex, pInputState);
}


// From MM11.h
unsigned long WINAPI XInputGetCapabilities_Hook(unsigned long dwUserIndex, unsigned long dwFlags, XINPUT_CAPABILITIES* pCaps)
{
	static bool bOnce = false;
	if (!bOnce) {
		DebugOutput("XInputGetCaps hook once.");
		bOnce = true;

	}

	if (!pCaps)
		return ERROR_SUCCESS;

	// Null flags
	pCaps->Flags ^= pCaps->Flags;

	// Set type and sub-type
	pCaps->Type = XINPUT_DEVTYPE_GAMEPAD;
	pCaps->SubType = XINPUT_DEVSUBTYPE_GAMEPAD;

	// buttons capable
	pCaps->Gamepad.wButtons |= XINPUT_GAMEPAD_A;
	pCaps->Gamepad.wButtons |= XINPUT_GAMEPAD_B;
	pCaps->Gamepad.wButtons |= XINPUT_GAMEPAD_Y;
	pCaps->Gamepad.wButtons |= XINPUT_GAMEPAD_X;
	pCaps->Gamepad.wButtons |= XINPUT_GAMEPAD_DPAD_UP;
	pCaps->Gamepad.wButtons |= XINPUT_GAMEPAD_DPAD_DOWN;
	pCaps->Gamepad.wButtons |= XINPUT_GAMEPAD_DPAD_LEFT;
	pCaps->Gamepad.wButtons |= XINPUT_GAMEPAD_DPAD_RIGHT;
	pCaps->Gamepad.wButtons |= XINPUT_GAMEPAD_START;
	pCaps->Gamepad.wButtons |= XINPUT_GAMEPAD_BACK;
	pCaps->Gamepad.wButtons |= XINPUT_GAMEPAD_LEFT_THUMB;
	pCaps->Gamepad.wButtons |= XINPUT_GAMEPAD_RIGHT_THUMB;
	pCaps->Gamepad.wButtons |= XINPUT_GAMEPAD_LEFT_SHOULDER;
	pCaps->Gamepad.wButtons |= XINPUT_GAMEPAD_RIGHT_SHOULDER;

	// set vib
	pCaps->Vibration.wLeftMotorSpeed = SHRT_MAX;
	pCaps->Vibration.wRightMotorSpeed = SHRT_MAX;

	// set triggers\thumbs
	pCaps->Gamepad.bLeftTrigger = 255;
	pCaps->Gamepad.bRightTrigger = 255;
	pCaps->Gamepad.sThumbLX = SHRT_MAX;
	pCaps->Gamepad.sThumbLY = SHRT_MAX;
	pCaps->Gamepad.sThumbRX = SHRT_MAX;
	pCaps->Gamepad.sThumbRY = SHRT_MAX;

	return ERROR_SUCCESS;
}

#pragma optimize("", off)
XINPUT_STATE * PlaybackManager::GetXInputState()
{
	return this->m_pGamePadState;
}

// shut up c4996
#pragma warning(disable : 4996)
PlaybackManager::PlaybackManager(const char *pcszFileName)
{
	this->m_bLoading = false;;

	// ADDED - Init break state
	this->m_BreakState.m_nLineNo = -1;
	strcpy(this->m_BreakState.m_szCurrentFile, this->m_szDefaultFileName);
	// Clear this so it doesn't have junk in it
	memset(&this->m_szCurrentManagerState[0], 0, 120);

	this->m_pGamePadState = nullptr;
	this->m_nTotalFrameCount = 0;
	this->m_Fp = NULL;
	this->m_pSegmentedFile = NULL;
	this->m_bPlayingBack = false;

	auto nLen = GetModuleFileNameA(nullptr, this->m_szCurrentDirectory, 256); 
	this->m_CWD.assign(this->m_szCurrentDirectory);
	auto indexOfLastBackSlash = this->m_CWD.rfind("\\");
	this->m_CWD.erase(indexOfLastBackSlash + 1, this->m_CWD.length() - indexOfLastBackSlash + 1);
	this->m_CWD += "\\Includes\\";

	if (pcszFileName)
	{
		this->m_Fp = _fsopen(pcszFileName, "r", _SH_DENYNO);
	}
	else
	{
		DebugOutput("null filename in PlaybackManager ctor.");
	}
}

// also need to return the line count here no?
bool PlaybackManager::ReadMutilLevelInputFile(const char * _szFileName, unsigned long otherFileCount, unsigned long* pOutRecordsRead, unsigned long *pOutLinesRead)
{
	char LineBuffer[2048] = { 0 };
	unsigned int linecount = otherFileCount;

	unsigned int otherLineCount = 0;

	if (this->m_pSegmentedFile)
	{
		fclose(this->m_pSegmentedFile);
		this->m_pSegmentedFile = nullptr;
	}

	std::string pathToCurrentFile = this->m_CWD;
	pathToCurrentFile += _szFileName;

	DebugOutput("PathToCurrentFile = %s", pathToCurrentFile.c_str());

	this->m_pSegmentedFile = _fsopen(pathToCurrentFile.c_str(), "r", _SH_DENYNO);

	if (this->m_pSegmentedFile == nullptr)
	{
		//DebugOutput("!this->m_pSegmentedFile, null pointer from _fsopen , errno=%u", errno);
		return false;
	}

	rewind(this->m_pSegmentedFile);

	while (true)
	{
		if (fgets(LineBuffer, 2048, this->m_pSegmentedFile) == NULL)
			break;

		LineBuffer[strcspn(LineBuffer, "\n")] = 0;

		if (strlen(LineBuffer) == 0)
		{
			++linecount;
			++otherLineCount;
			memset(LineBuffer, 0, sizeof(LineBuffer) / sizeof(LineBuffer[0]));
			continue;
		}

		if (LineBuffer[0] == '#')
		{
			++linecount;
			++otherLineCount;
			memset(LineBuffer, 0, sizeof(LineBuffer) / sizeof(LineBuffer[0]));
			continue;
		}

		std::string stringBuffer(LineBuffer);

		unsigned long long indexRunto = stringBuffer.find("Runto");
		unsigned long long  indexWalkto = stringBuffer.find("Walkto");


		if (indexRunto != std::string::npos)
		{
			// ADDED - Setting break state variables
			strcpy(this->m_BreakState.m_szCurrentFile, _szFileName);
			this->m_BreakState.m_nLineNo = otherLineCount;
			this->m_BreakState.m_BreakType = eBreakType::BREAKTYPE_FAST;

			this->m_RuntoLineNo = otherLineCount;
			// still increase linecount
			linecount++;
			otherLineCount++;


			continue;
		}
		else if (indexWalkto != std::string::npos)
		{
			// ADDED - Setting break state variables
			strcpy(this->m_BreakState.m_szCurrentFile, _szFileName);
			this->m_BreakState.m_nLineNo = otherLineCount;
			this->m_BreakState.m_BreakType = eBreakType::BREAKTYPE_NORMAL;

			this->m_WalktoLineNo = otherLineCount;
			// still increase linecount
			linecount++;
			otherLineCount++;


			continue;
		}

		// Ok, we know he is multi leveled because we are inside ReadMultiLevelInputFile.
		// why is otherLineCount always == 0?

		InputRecord * p = new InputRecord(std::string(LineBuffer), ++linecount, _szFileName, ++otherLineCount);

		if (p->m_Frames == -1)
		{
			delete p;
			memset(LineBuffer, 0, sizeof(LineBuffer) / sizeof(LineBuffer[0]));
			continue;
		}

		this->m_nTotalFrameCount += p->m_Frames;
		this->m_Inputs.push_back(p);
		++*pOutRecordsRead;
		memset(LineBuffer, 0, sizeof(LineBuffer) / sizeof(LineBuffer[0]));
	}

	fclose(this->m_pSegmentedFile);
	this->m_pSegmentedFile = nullptr;

	return true;
}

bool PlaybackManager::ReadInputFile()
{
	char LineBuffer[2048] = { 0 };
	unsigned int linecount = 0;

	DebugOutput("Hi, from ReadInputFile.");

	if (this->m_Fp == nullptr)
	{
		DebugOutput("No input file handle.");
		return false;
	}


	rewind(this->m_Fp);

	this->m_Inputs.clear();


	while (true)
	{
		if (fgets(LineBuffer, 2048, this->m_Fp) == NULL)
			break;

		LineBuffer[strcspn(LineBuffer, "\n")] = 0;

		if (strlen(LineBuffer) == 0)
		{
			++linecount;
			memset(LineBuffer, 0, sizeof(LineBuffer) / sizeof(LineBuffer[0]));
			continue;
		}


		if (LineBuffer[0] == '#')
		{
			// still increase our linecount
			++linecount;
			memset(LineBuffer, 0, sizeof(LineBuffer) / sizeof(LineBuffer[0]));
			continue;
		}

		
		std::string stringBuffer(LineBuffer);
		unsigned long long indexRunto = stringBuffer.find("Runto");
		unsigned long long  indexWalkto = stringBuffer.find("Walkto");

		unsigned long long indexRead = stringBuffer.find("Read");

		if (indexRunto != std::string::npos)
		{
			strcpy(this->m_BreakState.m_szCurrentFile, this->m_szDefaultFileName);
			this->m_BreakState.m_nLineNo = linecount;
			this->m_BreakState.m_BreakType = eBreakType::BREAKTYPE_FAST;

			this->m_RuntoLineNo = linecount;
			// still increase linecount
			linecount++;
			continue;
		}
		else if (indexWalkto != std::string::npos)
		{
			strcpy(this->m_BreakState.m_szCurrentFile, this->m_szDefaultFileName);
			this->m_BreakState.m_nLineNo = linecount;
			this->m_BreakState.m_BreakType = eBreakType::BREAKTYPE_NORMAL;

			this->m_WalktoLineNo = linecount;
			// still increase linecount
			linecount++;

			continue;
		}


		/* TODO:

		   Flesh this shit out!
		
		*/
		if (indexRead != std::string::npos)
		{
			// Should I just.. do the same thing but read from this file?
			unsigned long long indexOfDelimeter = stringBuffer.find(",");
			if (indexOfDelimeter != std::string::npos)
			{
				// ok, everything after this should be the filename
				std::string fileName = stringBuffer.substr(indexOfDelimeter + 1);
				linecount++;

				// NOTE: This probably completely destroys current Walkto implementation!
				unsigned long inRecordsRead = 0;
				unsigned long outLinesRead = 0;
				bool multiResult = this->ReadMutilLevelInputFile(fileName.c_str(), linecount, &inRecordsRead, &outLinesRead);

				if (multiResult)
					DebugOutput("Read %u InputRecords from %s.", inRecordsRead, fileName.c_str());
				else
					DebugOutput("Couldn't read multi level input file %s.", fileName.c_str());

				continue;
			}
			
		}


		InputRecord * p = new InputRecord(std::string(LineBuffer), ++linecount, this->m_szDefaultFileName, 0);

		// invalid InputRecord.
		if (p->m_Frames == -1)
		{
			// free this crap, get it out of here
			delete p;
			memset(LineBuffer, 0, sizeof(LineBuffer) / sizeof(LineBuffer[0]));
			continue;
		}

		this->m_nTotalFrameCount += p->m_Frames;
		this->m_Inputs.push_back(p);
		memset(LineBuffer, 0, sizeof(LineBuffer) / sizeof(LineBuffer[0]));
	}

	DebugOutput("Leaving ReadInputFile.");

	return true;
}

unsigned long PlaybackManager::GetTotalFrameCount()
{
	return this->m_nTotalFrameCount;
}

void PlaybackManager::InitPlayback(bool bReload = true)
{
	this->m_RuntoLineNo = -1;
	this->m_WalktoLineNo = -1;
	this->m_nTotalFrameCount = 0;

	if (this->m_bPlayingBack && bReload)
	{
		this->m_bPlayingBack = false;
		return;
	}
	

	bool result = this->ReadInputFile();


	if (!result)
	{
		DebugOutput("Failed to read input file.");
		return;
	}
	

	this->m_bPlayingBack = true;

	this->m_CurrentFrame = 0;
	this->m_InputIndex = 0;


	if (this->m_Inputs.size() > 0)
	{
		this->m_pCurrentInput = this->m_Inputs[0];
		this->m_FrameToNext = m_pCurrentInput->m_Frames;
	}
	else
	{
		DebugOutput("!Inputs.size");
		this->m_FrameToNext = 1;
		// Disable playback

		this->m_bPlayingBack = false;
		return;
	}

}

unsigned long PlaybackManager::ReloadPlayback()
{
	// Save it
	unsigned long dwPlayedBackFrames = this->m_CurrentFrame;
	this->InitPlayback(false);

	// Restore it
	this->m_CurrentFrame = dwPlayedBackFrames;

	// Step on the index until we get  back to where we were.
	while (this->m_CurrentFrame > this->m_FrameToNext)
	{
		if (this->m_InputIndex + 1 >= this->m_Inputs.size())
		{
			this->m_InputIndex++;
			return this->m_Inputs.size();
		}

		this->m_pCurrentInput = this->m_Inputs[++this->m_InputIndex];
		this->m_FrameToNext += this->m_pCurrentInput->m_Frames;
	}

	return this->m_Inputs.size();
}

bool PlaybackManager::IsPlayingBack()
{
	return this->m_bPlayingBack;
}

unsigned long PlaybackManager::GetCurrentInputIndex()
{
	return this->m_InputIndex;
}


// shut up c4996
#pragma warning(disable : 4996)
void PlaybackManager::DoPlayback(bool wasFramestepped, XINPUT_STATE*pxInpState)
{
	if (!this->m_bPlayingBack)
	{
		memset(&this->m_szCurrentManagerState[0], 0, 120);
		return;
	}

	if(wasFramestepped)
		memset(&this->m_szCurrentManagerState[0], 0, 120);

	if (!pxInpState)
	{
		DebugOutput("Null input state in DoPlayback??? ! ?");
		return;
	}

	// This might not be the actual IsLoading, but this is a good test.
	if (this->m_InputIndex < this->m_Inputs.size() && !IsLoading(g_llGameLoopRcx))
	{
		if (wasFramestepped)
		{
			unsigned long OldInputDoneCount = m_pCurrentInput->m_Done;
			unsigned long ReloadedCount = this->ReloadPlayback();
			m_pCurrentInput->m_Done += OldInputDoneCount;
		}

		if (this->m_CurrentFrame >= this->m_FrameToNext)
		{
			if (this->m_InputIndex + 1 >= this->m_Inputs.size())
			{
				if (wasFramestepped)
				{
					unsigned long ReloadedCountScope2 = this->ReloadPlayback();
					if (this->m_InputIndex + 1 >= ReloadedCountScope2)
					{
						this->m_InputIndex++;

						// disable playback
						this->m_bPlayingBack = false;
						DebugOutput("Index+1 > ReloadedScope, playback done.");

						return;
					}
				}
				else
				{
					if (this->m_InputIndex + 1 >= this->m_Inputs.size())
					{
						this->m_InputIndex++;

						// Disable playback
						this->m_bPlayingBack = false;
						DebugOutput("Index+1 > inputSize, playback done.");

						return;
					}
				}

			} // index + 1 > = inputs.size() scope end

			this->m_pCurrentInput = this->m_Inputs[++this->m_InputIndex];

			// RNG, other stuff here, etc


			// Breakpoints
			if (this->m_BreakState.m_nLineNo != -1)
			{
				if (this->m_BreakState.m_BreakType == eBreakType::BREAKTYPE_NORMAL)
				{
					// Check the FILE name the input is from, before we decide to break.
					
					if (this->m_pCurrentInput->m_bMultiLevelFile &&
						!strcmpi(this->m_pCurrentInput->m_szFromFile, this->m_BreakState.m_szCurrentFile) &&
						this->m_BreakState.m_nLineNo == this->m_pCurrentInput->m_nInternalLineNo)
					{
						this->m_BreakState.m_nLineNo = -1;
						g_bPaused = true;
					}
					else
					{
						if (this->m_BreakState.m_nLineNo == this->m_pCurrentInput->m_nLineNo && !strcmpi(this->m_pCurrentInput->m_szFromFile, this->m_BreakState.m_szCurrentFile))
						{
							this->m_BreakState.m_nLineNo = -1;
							g_bPaused = true;
						}
					}

				}
			}

			this->m_FrameToNext += this->m_pCurrentInput->m_Frames;
		} // frame to next scope end
		else
		{
			//DebugOutput("input.done++");
			this->m_pCurrentInput->m_Done++;
		}

		//  Increase current frame.
		this->m_CurrentFrame++;



		// Set our pointer to the one from the GetState hook.
		this->m_pGamePadState = pxInpState;

		// Set the pad state to the input record state.
		m_pCurrentInput->GetRecordState(pxInpState);

		// Done / Frames
		//sprintf(this->m_szCurrentManagerState, "Ln: %u (%u / %u) - [%s]\n(Cur:%u / Total:%u)", this->m_pCurrentInput->m_nLineNo, this->m_pCurrentInput->m_Done, this->m_pCurrentInput->m_Frames,
			//this->m_pCurrentInput->ToString().c_str(), this->m_CurrentFrame,this->m_nTotalFrameCount);


		sprintf(this->m_szCurrentManagerState, "[%s]-Ln: %u (%u / %u) - [%s]\n(Cur:%u / Total:%u)",
			this->m_pCurrentInput->m_szFromFile,
			(m_pCurrentInput->m_bMultiLevelFile) ? this->m_pCurrentInput->m_nInternalLineNo : this->m_pCurrentInput->m_nLineNo,
			this->m_pCurrentInput->m_Done, this->m_pCurrentInput->m_Frames,
			this->m_pCurrentInput->ToString().c_str(), this->m_CurrentFrame, this->m_nTotalFrameCount);

		// Packet number is our current frame.
		pxInpState->dwPacketNumber = this->m_CurrentFrame;

	}

	return;
}

InputRecord * PlaybackManager::GetCurrentInput()
{
	return this->m_pCurrentInput;
}

InputRecord * PlaybackManager::GetCurrentInputIndexBased()
{
	return (this->m_InputIndex == this->m_Inputs.size()) ? nullptr : this->m_Inputs[this->m_InputIndex];
}
#pragma optimize("", on)