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

PlaybackManager::PlaybackManager(const char *pcszFileName)
{
	// Clear this so it doesn't have junk in it
	memset(&this->m_szCurrentManagerState[0], 0, 120);

	this->m_pGamePadState = nullptr;
	this->m_nTotalFrameCount = 0;
	this->m_Fp = NULL;
	this->m_bPlayingBack = false;

	if (pcszFileName)
	{
		this->m_Fp = _fsopen(pcszFileName, "r", _SH_DENYNO);
	}
	else
	{
		DebugOutput("null filename in PlaybackManager ctor.");
	}
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
		if (indexRunto != std::string::npos)
		{
			this->m_RuntoLineNo = linecount;
			// still increase linecount
			linecount++;
			continue;
		}
		else if (indexWalkto != std::string::npos)
		{
			this->m_WalktoLineNo = linecount;
			// still increase linecount
			linecount++;
			continue;
		}


		InputRecord * p = new InputRecord(std::string(LineBuffer), ++linecount);

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

	if (!pxInpState)
	{
		DebugOutput("Null input state in DoPlayback??? ! ?");
		return;
	}

	if (this->m_InputIndex < this->m_Inputs.size())
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


			if (this->m_RuntoLineNo != -1)
			{
				if (m_pCurrentInput->m_nLineNo < this->m_RuntoLineNo)
				{
					;
				}
				else
				{
					this->m_RuntoLineNo = -1;
					g_bPaused = true;
					// set speed here
				}
			}
			else if (this->m_WalktoLineNo != -1)
			{
				if (m_pCurrentInput->m_nLineNo < this->m_WalktoLineNo)
					;
				else
				{
					this->m_WalktoLineNo = -1;
					g_bPaused = true;
					// set speed here
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


		// Done / Frames
		sprintf(this->m_szCurrentManagerState, "Ln: %u (%u / %u) - [%s]\n TotalFrames: %u", this->m_pCurrentInput->m_nLineNo, this->m_pCurrentInput->m_Done, this->m_pCurrentInput->m_Frames,
			this->m_pCurrentInput->ToString().c_str(), this->m_nTotalFrameCount);

		// Set our pointer to the one from the GetState hook.
		this->m_pGamePadState = pxInpState;

		// Set the pad state to the input record state.
		m_pCurrentInput->GetRecordState(pxInpState);

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