#pragma once

#include <string>
#include <algorithm>
#include <sstream>
#include <vector>
#include "Utils.h"
#include <map>
#include <Xinput.h>
#include <cctype>

// Input state bitflags.

enum EInputState
{
	// for null frames
	DEFAULT_NONE=0x0,

	LEFT=1<<0,
	RIGHT=1<<1,
	UP=1<<2,
	DOWN=1<<3,
	JUMP=1<<4,
	SHOOT=1<<5,
	DASH=1<<6,
	RUSH_COIL=1<<7,
	RUSH_JET=1<<8,
	WEAPON_SELECT=1<<9,
	SPEED_GEAR   = 1<<10,
	POWER_GEAR   = 1<<11,
	START        = 1<<12,
	SELECT       = 1<<13,
	BUSTER       = 1<<14,
	RTRIG        = 1<<15,
	LTRIG        = 1<<16
};

//const static unsigned int eDashState = EInputState::DOWN | EInputState::JUMP;

typedef struct t_InputRecord
{
	// all public
	EInputState m_InputState;

	int m_Frames;

	int m_Done;

	unsigned long  m_Seed;

	unsigned long long m_nLineNo;

	// For multi-level input files.
	unsigned long long m_nInternalLineNo;

	bool m_bMultiLevelFile;

	char m_szFromFile[56];

	std::map<std::string, std::pair<signed short, signed short>> m_WeaponSelectMapping = {
		{"Block", std::make_pair(-1*32768, 1*32767)},
		{"Acid",  std::make_pair(0, 1*32767)},
		{"Blast", std::make_pair(1* 32767, 1* 32767)},
		{"Fuse",  std::make_pair(-1* 32768, 0)},
	    {"Bounce", std::make_pair(1* 32767, 0)},
	    {"Torch", std::make_pair(-1* 25000, -1* 32768)},
	    {"Impact", std::make_pair(0, -1* 32768)},
	    {"Tundra", std::make_pair(1* 32767, -1* 32768)}
	};

	std::pair<signed short, signed short> m_WeaponSelectDir;

	std::string ToString()
	{
		std::string result = "";
		bool bSlideAlias = false;

		//result += "(LineNo: " + this->m_nLineNo;
		result += std::to_string(this->m_Frames);
		if (this->IsLeft())
			result += ",Left";

		if (this->IsRight())
			result += ",Right";

		if (this->IsUp())
			result += ",Up";


		if (this->IsDown() && this->IsJump())
		{
			result += ",Slide";
			bSlideAlias = true;
		}

		if (this->IsDown() && !bSlideAlias)
			result += ",Down";
		
		if (this->IsJump() && !bSlideAlias)
			result += ",Jump";

		if (this->IsPowerGear())
			result += ",PowerGear";

		if (this->IsSpeedGear())
			result += ",SpeedGear";

		if (this->IsWeaponSelect())
			result += ",WeaponSelect";

		if (this->IsStart())
			result += ",Start";

		if (this->IsSelect())
			result += ",Select";

		if (this->IsRushCoil())
			result += ",RushCoil";

		if (this->IsRushJet())
			result += ",RushJet";

		if (this->IsShoot())
			result += ",Shoot";

		if (this->IsBuster())
			result += ",Buster";


		return result;

	}

	void GetRecordState(XINPUT_STATE * pState)
	{
		if (!pState) return;

		auto pad = &pState->Gamepad;

		// Null the pad state.
		if (this->IsEmpty())
		{
			memset(pad, 0, sizeof(XINPUT_GAMEPAD));
			return;
		}


		if (this->IsLeft())
			pad->wButtons |= XINPUT_GAMEPAD_DPAD_LEFT;
		
		if (this->IsRight())
			pad->wButtons |= XINPUT_GAMEPAD_DPAD_RIGHT;

		if (this->IsDown())
			pad->wButtons |= XINPUT_GAMEPAD_DPAD_DOWN;

		if (this->IsUp())
			pad->wButtons |= XINPUT_GAMEPAD_DPAD_UP;

		if (this->IsJump())
			pad->wButtons |= XINPUT_GAMEPAD_A;

		if (this->IsShoot())
			pad->wButtons |= XINPUT_GAMEPAD_X;

		if (this->IsStart())
			pad->wButtons |= XINPUT_GAMEPAD_START;

		if (this->IsRushCoil())
			pad->wButtons |= XINPUT_GAMEPAD_Y;

		if (this->IsRushJet())
			pad->wButtons |= XINPUT_GAMEPAD_B;

		if (this->IsSpeedGear())
			pad->wButtons |= XINPUT_GAMEPAD_RIGHT_SHOULDER;

		if (this->IsPowerGear())
			pad->wButtons |= XINPUT_GAMEPAD_LEFT_SHOULDER;

		// Added buster.
		if (this->IsBuster())
			pad->wButtons |= XINPUT_GAMEPAD_RIGHT_THUMB;

		// Set them to max.
		if (this->IsRTrigger())
			pad->bRightTrigger = 255;

		// Set them to max.
		if (this->IsLTrigger())
			pad->bLeftTrigger = 255;

		if (this->IsWeaponSelect())
		{
			// Set tRx and tRy
			pad->sThumbRX = this->m_WeaponSelectDir.first;
			pad->sThumbRY = this->m_WeaponSelectDir.second;
		}
	}

	bool HasFlag(EInputState state, EInputState which)
	{
		return (state & which) == which;
	}

	bool IsLeft()
	{
		return this->HasFlag(this->m_InputState, EInputState::LEFT);
	}

	bool IsRight()
	{
		return this->HasFlag(this->m_InputState, EInputState::RIGHT);
	}

	bool IsUp()
	{
		return this->HasFlag(this->m_InputState, EInputState::UP);
	}

	bool IsDown()
	{
		return this->HasFlag(this->m_InputState, EInputState::DOWN);
	}

	bool IsJump()
	{
		return this->HasFlag(this->m_InputState, EInputState::JUMP);
	}

	bool IsRushJet()
	{
		return this->HasFlag(this->m_InputState, EInputState::RUSH_JET);
	}

	bool IsRushCoil()
	{
		return this->HasFlag(this->m_InputState, EInputState::RUSH_COIL);
	}

	bool IsShoot()
	{
		return this->HasFlag(this->m_InputState, EInputState::SHOOT);
	}

	bool IsWeaponSelect()
	{
		return this->HasFlag(this->m_InputState, EInputState::WEAPON_SELECT);
	}

	bool IsStart()
	{
		return this->HasFlag(this->m_InputState, EInputState::START);
	}

	bool IsSelect()
	{
		return this->HasFlag(this->m_InputState, EInputState::SELECT);
	}

	bool IsSpeedGear()
	{
		return this->HasFlag(this->m_InputState, EInputState::SPEED_GEAR);
	}

	bool IsPowerGear()
	{
		return this->HasFlag(this->m_InputState, EInputState::POWER_GEAR);
	}

	bool IsBuster()
	{
		return this->HasFlag(this->m_InputState, EInputState::BUSTER);
	}

	bool IsRTrigger()
	{
		return this->HasFlag(this->m_InputState, EInputState::RTRIG);
	}


	bool IsLTrigger()
	{
		return this->HasFlag(this->m_InputState, EInputState::LTRIG);
	}

	bool IsEmpty()
	{
		return this->m_InputState == EInputState::DEFAULT_NONE;
	}

	t_InputRecord(unsigned long frames, EInputState state)
	{
		this->m_Frames = frames;
		this->m_InputState = state;
	}

#pragma warning(disable: 4996)
	t_InputRecord(std::string line, unsigned int ln, const char*filename, unsigned int otherln)
	{
		// Default this.
		this->m_Seed = -1;

		// fiddy six
		strncpy(this->m_szFromFile, filename, sizeof(this->m_szFromFile) / sizeof(this->m_szFromFile[0]));

		// This needs to be defaulted!
		this->m_bMultiLevelFile = false;

		// if(filename != megaman.rec)
		if (strcmpi(filename, "megaman.rec"))
		{
			this->m_nInternalLineNo = 0;
			this->m_bMultiLevelFile = true;
		}

		this->m_nInternalLineNo = otherln;
		this->m_nLineNo = ln;
		this->m_Done = 0;

		std::istringstream ss(line);
		std::string token;

		std::vector < std::string> tokens;

		auto delimited = line.find(',');

		while (std::getline(ss, token, ','))
			tokens.push_back(token);

		bool bWasValidToken = false;
		// If we don't even have frames, well, this isn't a valid input record then.
		if (!isdigit(line[0]))
		{
			this->m_Frames = -1;
			return;
		}

		this->m_Frames = (delimited == std::string::npos) ? std::stoul(line) : std::stoul(tokens[0]);
		unsigned int TempState = EInputState::DEFAULT_NONE;

		// Make sure we don't have JUST framecount (null or empty inputs)
		if (tokens.size() > 1 && delimited != std::string::npos)
		{
			for (unsigned int i = 1; i < tokens.size(); i++)
			{
				token = tokens[i];

				// Just continue again, we already handled cases where we needed to process integers.
				if (isdigit(tokens[i][0]))
					continue;

				auto negativelamb = [](char& ch) { ch = toupper((unsigned char)ch); };
				std::for_each(token.begin(), token.end(), negativelamb);

				auto lhstrim = [](std::string& in)
				{
					//lambception
					auto iter = std::find_if(in.begin(), in.end(), [](char ch) { return !std::isspace((unsigned char)ch); });
					in.erase(in.begin(), iter);
					return in;
				};

				auto rhstrim = [](std::string& in)
				{
					auto iter2 = std::find_if(in.rbegin(), in.rend(), [](char ch) { return !std::isspace((unsigned char)ch); });
					in.erase(iter2.base(), in.end());
					return in;
				};

				// Remove the leading and trailing spaces.
				token = lhstrim(token);
				token = rhstrim(token);

				if (token == "LEFT")
				{
					TempState |= EInputState::LEFT;
					bWasValidToken = true;
					continue;
				}
				else if(token=="RIGHT")
				{
					TempState |= EInputState::RIGHT;
					bWasValidToken = true;
					continue;
				}
				else if (token == "UP")
				{
					TempState |= EInputState::UP;
					bWasValidToken = true;
					continue;
				}
				else if (token == "DOWN")
				{
					TempState |= EInputState::DOWN;
					bWasValidToken = true;
					continue;
				}
				else if (token == "START")
				{
					TempState |= EInputState::START;
					bWasValidToken = true;
					continue;
				}
				else if (token == "SELECT")
				{
					TempState |= EInputState::SELECT;
					bWasValidToken = true;
					continue;
				}
				else if (token == "JUMP")
				{
					TempState |= EInputState::JUMP;
					bWasValidToken = true;
					continue;
				}
				else if (token == "SHOOT")
				{
					TempState |= EInputState::SHOOT;
					bWasValidToken = true;
					continue;
				}
				else if (token == "SGEAR")
				{
					TempState |= EInputState::SPEED_GEAR;
					bWasValidToken = true;
					continue;
				}
				else if (token == "PGEAR")
				{
					TempState |= EInputState::POWER_GEAR;
					bWasValidToken = true;
					continue;
				}
				else if (token == "WSELECT")
				{
					std::string weaponName = tokens[i + 1];
					// Special case. don't set it as weapon select, set it as buster.
					if (weaponName == "Buster" || weaponName == "BUSTER")
					{
						TempState |= EInputState::BUSTER;
						bWasValidToken = true;
						continue;
					}

					TempState |= EInputState::WEAPON_SELECT;
					this->m_WeaponSelectDir = this->m_WeaponSelectMapping[weaponName];
					bWasValidToken = true;
					continue;
				}
				else if (token == "RCOIL")
				{
					TempState |= EInputState::RUSH_COIL;
					bWasValidToken = true;
					continue;
				}
				else if (token == "RJET")
				{
					TempState |= EInputState::RUSH_JET;
					bWasValidToken = true;
					continue;
				}
				else if (token == "BUSTER")
				{
					TempState |= EInputState::BUSTER;
					bWasValidToken = true;
					continue;
				}
				else if (token == "SLIDE")
				{
					TempState |= (EInputState::JUMP | EInputState::DOWN);
					bWasValidToken = true;
					continue;
				}
				else if (token == "RTRIG")
				{
				    TempState |= EInputState::RTRIG;
					bWasValidToken = true;
				    continue;
				}
				else if (token == "LTRIG")
				{
				    TempState |= EInputState::LTRIG;
					bWasValidToken = true;
				    continue;
				}
				else if (token == "SEED")
				{
				
				    bWasValidToken = true;
				    if((i+1) < tokens.size())
						this->m_Seed = std::stoul(tokens[i + 1]);
					else
					{
						DebugOutput("Seed is missing numeric value?");
					}
					continue;
				}

			}
		}

		this->m_InputState = (EInputState)(TempState);

	}

	t_InputRecord() {}

} InputRecord;

