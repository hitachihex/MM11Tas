#pragma once


// Make some sense, sensemaker!
namespace MTFramework
{
	enum eCameraDisplayType : unsigned char
	{
		CDT_NORMAL            = 0x00,
		CDT_UNK1              = 0x01,
		CDT_TOP_STRETCHED     = 0x02,
		CDT_BOTTOM_STRETCHED  = 0x03,
		CDT_LEFT_SQUEEZED     = 0x04,
		CDT_RIGHT_SQUEEZED    = 0x05,
		CDT_TOPLEFT_SMALL     = 0x06,
		CDT_BOTTOMLEFT_SMALL  = 0x07,
		CDT_TOPRIGHT_SMALL    = 0x08,
		CDT_BOTTOMRIGHT_SMALL = 0x09,
		CDT_END               = 0x0A
	};

#pragma pack(push, 1)


	class ScreenStuff
	{
		// 0x00 - 0x07
		// m_Vtable[9] - Deals with positions and stuff.
		unsigned long long m_Vtable;

		// 0x08 - 0x0F
		unsigned long long m_qwUnknown08_0F;

		// 0x10 - 0x13
		unsigned long  m_dwMaskUnknown_10_13;

		// 0x14 - 0x47
		unsigned char m_ucUnknown0014_0047[0x48 - 0x14];

		// 0x48 - 0x4B
		float m_AspectRatio;

		// 0x4C - 0x4F
		unsigned long m_dwUnknown_4C_4F;

		// 0x50 - 0x53
		float m_PositionX;

		// 0x54 - 0x57
		float m_PositionY;

		// 0x58 - 0x5B
		float m_PositionZ;

		// 0x5C - 0x5F
		unsigned long m_dwUnknown_5C_5F;

		// 0x60 - 0x63
		unsigned long m_dwUnknown_60_63;

		// 0x64 - 0x67
		unsigned long m_dwUnknown_64_67;

		// 0x68 - 0x6B
		unsigned long m_dwUnknown_68_6B;

		// 0x6C - 0x6F
		unsigned long m_dwUnknown_6C_6F;

		// 0x70 - 0x73
		float m_PrevPositionX;

		// 0x74 - 0x77
		float m_PrevPositionY;

		// 0x78 - 0x7B
		float m_PrevPositionZ;

	};


	class GameCamera
	{
	public:

		// 0x00 - 0x07
		unsigned long long m_qwVtable;

		// 0x08 - 0x4F
		unsigned char m_ucUnknown08_4F[0x50 - 0x08];

		// 0x50 - 0x57
		unsigned long long m_qwClassPtr;

		// 0x58 - 0x5F
		// Lol, this is terrible name for this.
		ScreenStuff * m_pScreenStuff;

		// 0x60 - 0x63
		unsigned long m_dwUnknown60_63;
		
		// 0x64 - 0x67
		unsigned long m_dwUnknown64_67;

		// 0x68 - 0x6F
		unsigned long long m_qwUnknown00;

		// 0x70
		bool m_bUnknown00;

		// 0x71
		bool m_bUnknown01;

		// 0x72
		bool m_bUnknown02;

		// 0x73
		eCameraDisplayType m_DisplayType;

		// 0x74 - 0x77
		unsigned long m_dwUnknown74_77;

		// 0x78 - 0x7B
		// Subtracted from  Screen Width. 0 always, maybe.
		unsigned long m_dwUnknown78_7B;

		// 0x7C - 0x7F
		// Subtracted from Screen Height (bordered). 0 always, maybe.
		unsigned long m_dwUnknown7C_7F;

		// 0x80 - 0x83
		unsigned long m_ScreenWidth;

		// 0x84 - 0x87
		unsigned long m_ScreenHeightBordered;

		// 0x88 - 0x1A0
		unsigned char uc_Unknown0088_1A0[0x1A0 - 0x88];


		// --------
		static constexpr unsigned long long CullingConstant = 0x1409BE55C;

		static constexpr float * _CullingConstant = (float*)(0x1409BE55C);
	protected:
	private:

	};


	// lol nice name dork
	class Overmap
	{
	public:
		// (0-8) * 0x1a0
		// r15 = (0-8) * 0x1a0
		// rbx = qword ptr ds:[r15+this]
		GameCamera m_Cameras[0x9];

	protected:
	private:


	};

	class MainGame
	{
	public:

		// 0x00 - 0x401B7
		unsigned char m_ucUnknown0000_401B7[0x401B8 - 0x0];

		// 0x401B8 - 0x401BF
		Overmap * m_pCamera;

	protected:
	private:

	};

	class Game
	{
	public:
	protected:
	private:

	};

	class Physics
	{
	public:
	protected:
	private:

	};

	class PlayerManager
	{
	public:
	protected:
	private:

	};
#pragma pack(pop)

}