#pragma once

namespace Game
{
	static constexpr unsigned int ARM_BLOCK      = 0x01;
	static constexpr unsigned int ARM_IMPACT     = 0x03;
	static constexpr unsigned int ARM_FUSE       = 0x05;

	static const unsigned long long _RockManArmSystemGPtr = 0x140B87A20;
#pragma pack(push, 1)
	class RockmanArmSystem
	{
	public:

		// 0x000 - 0x477
		unsigned char m_ucUnknown[0x478 - 0x0];

		// 0x478 - ??
		unsigned int m_ArmAmmo[6];

		void GiveMaxAmmo(unsigned int ArmIndex)
		{
			switch (ArmIndex)
			{
			case ARM_BLOCK:
				this->m_ArmAmmo[ARM_BLOCK] = 56;
				break;
			case ARM_IMPACT:
				this->m_ArmAmmo[ARM_IMPACT] = 56;
				break;
			case ARM_FUSE:
				this->m_ArmAmmo[ARM_FUSE] = 56;
				break;
			default:
				break;
			}
		}

		void GiveMaxRushSharedAmmo()
		{
			*(unsigned long*)((unsigned long)this + 0x49C) = 56;
			*(unsigned long*)((unsigned long)this + 0x4A0) = 56;
		}

	protected:
	private:

	};

	static RockmanArmSystem * GetRockManArmSystemPtr()
	{
		return (RockmanArmSystem*)(*(unsigned long long*)(_RockManArmSystemGPtr));
	}
#pragma pack(pop)
};
