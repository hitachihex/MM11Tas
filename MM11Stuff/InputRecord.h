#pragma once

#include <string>
#include <algorithm>
#include <sstream>
#include <vector>
#include "Utils.h"
#include <map>
#include <Xinput.h>

// Input state bitflags.


//asd 
//asd
//asd
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
	BUSTER       = 1<<14
};

//const static unsigned int eDashState = EInputState::DOWN | EInputState::JUMP;

typedef struct t_InputRecord
{
	// all public
	EInputState m_InputState;

	int m_Frames;

	int m_Done;

	unsigned int m_nLineNo;

	std::map<std::string, std::pair<signed short, signed short>> m_WeaponSelectMapping = {
		{"Block", std::make_pair(-1*32768, 1*32767)},
		{"Acid",  std::make_pair(0, 1*32767)},
		{"Blast", std::make_pair(1* 32767, 1* 32767)},
		{"Fuse",  std::make_pair(-1* 32768, 0)},
	    {"Bounce", std::make_pair(1* 32767, 0)},
	    {"Torch", std::make_pair(-1* 32768, -1* 32768)},
	    {"Impact", std::make_pair(0, -1* 32768)},
	    {"Tundra", std::make_pair(1* 32767, -1* 32768)}
	};

	std::pair<signed short, signed short> m_WeaponSelectDir;

	std::string ToString()
	{
		std::string result = "";

		result += std::to_string(this->m_Frames);

		if (this->IsLeft())
			result += ",Left";

		if (this->IsRight())
			result += ",Right";

		if (this->IsUp())
			result += ",Up";

		if (this->IsDown())
			result += ",Down";
		
		if (this->IsJump())
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

		if (this->IsRushJet())
		{

		}

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

	bool IsEmpty()
	{
		return this->m_InputState == EInputState::DEFAULT_NONE;
	}

	t_InputRecord(unsigned long frames, EInputState state)
	{
		this->m_Frames = frames;
		this->m_InputState = state;
	}

	t_InputRecord(std::string line, unsigned int ln)
	{
		this->m_nLineNo = ln;
		this->m_Done = 0;

		std::istringstream ss(line);
		std::string token;

		std::vector < std::string> tokens;

		auto delimited = line.find(',');

		while (std::getline(ss, token, ','))
			tokens.push_back(token);

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

				if (token == "LEFT")
				{
					TempState |= EInputState::LEFT;
					continue;
				}
				else if(token=="RIGHT")
				{
					TempState |= EInputState::RIGHT;
					continue;
				}
				else if (token == "UP")
				{
					TempState |= EInputState::UP;
					continue;
				}
				else if (token == "DOWN")
				{
					TempState |= EInputState::DOWN;
					continue;
				}
				else if (token == "START")
				{
					TempState |= EInputState::START;
					continue;
				}
				else if (token == "SELECT")
				{
					TempState |= EInputState::SELECT;
					continue;
				}
				else if (token == "JUMP")
				{
					TempState |= EInputState::JUMP;
					continue;
				}
				else if (token == "SHOOT")
				{
					TempState |= EInputState::SHOOT;
					continue;
				}
				else if (token == "SGEAR")
				{
					TempState |= EInputState::SPEED_GEAR;
					continue;
				}
				else if (token == "PGEAR")
				{
					TempState |= EInputState::POWER_GEAR;
					continue;
				}
				else if (token == "WSELECT")
				{
					TempState |= EInputState::WEAPON_SELECT;
					std::string weaponName = tokens[i + 1];
					this->m_WeaponSelectDir = this->m_WeaponSelectMapping[weaponName];
					continue;
				}
				else if (token == "RCOIL")
				{
					TempState |= EInputState::RUSH_COIL;
					continue;
				}
				else if (token == "RJET")
				{
					TempState |= EInputState::RUSH_JET;
					continue;
				}
				else if (token == "BUSTER")
				{
					TempState |= EInputState::BUSTER;
					continue;
				}

			}
		}

		this->m_InputState = (EInputState)(TempState);

	}

	t_InputRecord() {}

} InputRecord;

