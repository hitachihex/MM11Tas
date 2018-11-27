#pragma once


// Make some sense, sensemaker!
namespace MTFramework
{

#pragma pack(push, 1)
	typedef struct t_GameState
	{
		// 0x00 - 0x07
		unsigned long long m_qwUnknown;

		// 0x08 - 0x0B
		char * m_szMenuState;

	} GameState;

	class UnkClass_01
	{
	public:
		// 0x00 - 0x07
		virtual void _A() = 0;

		// 0x08 - 0x0F
		virtual void _B() = 0;

		// 0x10 - 0x17
		virtual void _C() = 0;

		// 0x18 - 0x1F
		virtual void _D() = 0;

		// 0x20 - 0x27
		virtual GameState * _GetGameState() = 0;
	protected:
	private:

	};

	class UnkClass_02
	{
	public:

		// 0x0000 - 0x7057
		unsigned char m_ucUnknown0000_007057[0x7058 - 0x0];

		// 0x7058 - 0x705F
		UnkClass_01 * m_pUnk01;
	protected:
	private:
	};
#pragma pack(pop)

	// We're loading if the m_szMenuState string == "aBriefing"
	static GameState * GetGameState()
	{
		UnkClass_02 * m_pUnk02 = (UnkClass_02*)(*(unsigned long long*)0x140B87490);
		if (m_pUnk02->m_pUnk01)
		{
			return m_pUnk02->m_pUnk01->_GetGameState();
		}
		return nullptr;
	}

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
	public:
		// 0x00 - 0x07
		/*
		virtual void _A() = 0;											-  table[0]
		virtual void _B() = 0;											-  table[1]
		virtual void _C() = 0;											-  table[2]
		virtual void _D() = 0;											-  table[3]
		virtual void _E() = 0;											-  table[4]
		virtual void _F() = 0;											-  table[5]
		virtual void _G() = 0;											-  table[6]
		virtual void _H() = 0;											-  table[7]
		virtual void _I() = 0;											-  table[8]
		virtual void _J() = 0;											-  table[9]   
		virtual void _K() = 0;											-  table[10]
		virtual void _L() = 0;											-  table[11]
		virtual void _M() = 0;											-  table[12]
		virtual void _N() = 0;											-  table[13]
		virtual void _O() = 0;											-  table[14]
		virtual void _P() = 0;											-  table[15]
		virtual void _Q() = 0;											-  table[16]
		virtual void _R() = 0;											-  table[17]
		virtual void HandleCameraStuff(unsigned long long* rdx) = 0;	-  table[18] - Deals with position and stuff.
		virtual void HandleCameraStuff2(unsigned long long* rdx) = 0;	-  table[19] - No idea what it does, but stuff.
		*/
		unsigned long long m_Vtable;

		// 0x08 - 0x0F
		unsigned long long m_qwUnknown08_0F;

		// 0x10 - 0x13
		/*
		   mov eax, dword ptr ds:[rdi+0x10]
		   and eax, 0x07
		   dec eax
		   cmp eax, 1
		   jbe game.1403a0103

		   // Remove bits?
		   this->m_dwMaskUnknown_10_13 &= 0x07;

		   // Decrease?
		   this->m_dwMaskUnknown_10_13--;

		   if(this->m_dwMaskUnknown <= 1)
		      ; ??

	       Apparently determines if the class has a Critical Section object?
		*/
		unsigned long  m_dwMaskUnknown_10_13;

		// 0x14 - 0x47
		unsigned char m_ucUnknown0014_0047[0x48 - 0x14];

		// 0x48 - 0x4B
		float m_AspectRatio;

		// 0x4C - 0x4F
		float m_Zoom;

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

	protected:
	private:

	};

	class UnknownCameraClass00
	{
	public:
		// 0x00 - 0x07
		unsigned long long m_qwVtable;
	protected:
	private:
	};

	class GameCamera
	{
	public:

		// 0x00 - 0x07
		unsigned long long m_qwVtable;

		// 0x08 - 0x4F
		unsigned char m_ucUnknown08_4F[0x50 - 0x08];

		// 0x50 - 0x57
		//unsigned long long m_qwClassPtr;
		UnknownCameraClass00 * m_pCameraClassUnknown00;

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

	// I've really no idea what it's name is.
	class MTUnitObject
	{
	public:

		// 0x00 - 0x07
		unsigned long long m_Vtable;

		// 0x08 - 0x30
		unsigned char m_ucUnknown08_30[0x30 - 0x08];

	protected:
	private:

	};

	class Unit
	{
	public:
		// 0x00 - 0x07

		//vtable[6]  - Dunno
		//vtable[9]  - Does the meat of it
		//vtable[10] - Does the meat of it
		//vtable[11] - Dunno
		//vtable[12] - Does the meat of it
		//vtable[13] - Dunno
		unsigned long long m_Vtable;


		// 0x06 * 0x08 = 0x30


		// lea r8, qword ptr ds:[r9+r9*2],   - objCount=r8
		// add r8, r8                        - objCount+=objCount
		// mov eax, dword ptr ds:[this+objCount*8+0x48]
		// shr eax, 0x01
		// test al, 0x01
		// jne game.140372846
		// mov qword ptr ss:[rsp+0x50], rbx  - localScopeQword=objectCount
		// lea rax, qword ptr ds:[r9+0x02]   - r9 += 0x02;
		// mov rbx, qword ptr ds:[this+objCount*8+0x50]  - .
		// lea rax, qword ptr ds:[rax+rax*2] - rax *= 0x03;
		// add rax, rax
		// movaps xmmword ptr ss:[rsp+0x20], xmm6    - localScopeDouble = xmm6
		// ...
		// ...
		// test rbx, rbx                     - !object
		// je game.14037283C                 - we have null, can't process this object at this index.

		// (this + (objCount+objCount*0x08+0x50))

		// 0x08 - 0x4F
		unsigned char m_ucUnknown08_4F[0x50 - 0x08];

		// 0x50 - ??
		MTUnitObject * m_pObjects;

		static constexpr unsigned long long _UnitManagerPointer = 0x140C3EF58;
		static constexpr unsigned int       NumObjectsToIterateOffset = 0xC38;

	protected:
	private:

	};

	class Resources
	{
	public:
		// 0x00 - 0x07
		unsigned long long m_Vtable;

	protected:
	private:

	};



	// cGeometryArray
	class MTCollisionObjectData
	{
	public:
		// 0x00 - 0x07
		unsigned long long m_Vtable;

		// 0x08 - 0x4F
		unsigned char m_ucUnknown[0x50 - 0x08];

		// 0x50 - 0x53
		float m_PositionX;

		// 0x54 - 0x57
		float m_PositionY;

		// 0x58 - 0x5B
		float m_PositionZ;

	protected:
	private:

	};

	class MtCollisionObjectInstance
	{
	public:

		// 0x00 - 0x07
		unsigned long long m_Vtable;

		// 0x08 - 0x0B
		unsigned long  m_dwUnknown08_0B;

		// 0x0C - 0x3F
		unsigned char m_ucUnknown[0x40 - 0x0C];

		// 0x40 - 0x47
		MTCollisionObjectData * m_pObjectData;

		// 0x48 - 0x15F
		unsigned char m_ucUnknown0048_0015F[0x160 - 0x48];

		// 0x160
		bool m_bUnk00;

		// 0x161
		bool m_bUnk01;

		// 0x162
		bool m_bUnk02;

		// 0x163
		bool m_bUnk03;

		// 0x164
		bool m_bUnk04;

		// 0x165
		bool m_bUnk05;

		// 0x166
		bool m_bUnk06;

		// 0x167
		bool m_bUnk07;

		// 0x168
		bool m_bUnk08;

		// 0x169
		bool m_bUnk09;

		// 0x16A
		bool m_bUnk0A;

		// 0x16B
		bool m_bUnk0B;

		// 0x16C
		bool m_bUnk0C;

		// 0x16D
		bool m_bUnk0D;

	protected:
	private:

	};

	class MTCollisionObjects
	{
	public:
		// 0x00 - 0x07
		unsigned long long m_Vtable;

		// 0x08 - 0x0B
		unsigned long m_dwUnknown08_0B;

		// 0x0C - 0x0F
		unsigned long m_dwUnknown0C_0F;

		// 0x10 - 0x17
		MtCollisionObjectInstance * m_pObjInstance;
	protected:
	private:

	};

	// Template class
	// cGeometryArray
	class CollisionManager
	{
	public:

		// 0x00 - 0x07
		unsigned long long m_Vtable;

		// 0x08 - 0x47
		unsigned char m_ucUnknown[0x48 - 0x08];

		// 0x48 - 0x4B
		// mov eax, dword ptr ds:[rbx+0x48]
		unsigned long m_NumObjects;

		// 0x4C - 0x4F
		unsigned long m_dwUnknown004C_004F;

		// 0x50 - 0x53
		unsigned long m_dwUnknown0050_0053;

		// 0x54 - 0x57
		unsigned long m_dwUnknown0054_0057;

		// 0x58 - 0x5F
		MTCollisionObjects ** m_pCollObjects;

		// ..
		// xor edi, edi                        - objIndex = 0
		// mov rax, qword ptr ds:[rbx+0x58]    - objects
		// mov rcx, qword ptr ds:[rax+rdi]     - objects[objIndex]
		// mov rcx, qword ptr ds:[rcx+0x10]    - more offsetting
	protected:
	private:
	};

	class Collision
	{
	public:
		// 0x00 - 0x07
		unsigned long long m_Vtable;

		// 0x08 - 0x2F
		CRITICAL_SECTION m_CriticalSection;

		// 0x30 - 0x43B7
		unsigned char m_ucUnknown0030_043B7[0x43B8 - 0x30];

		// 0x43B8 - 0x????
		CollisionManager m_CollManager;

	protected:
	private:

	};

	class MainGame
	{
	public:

		// 0x00 - 0x401A7
		unsigned char m_ucUnknown0000_401B7[0x401A8 - 0x0];

		// 0x401A8 - 0x401AF
		Unit * m_pUnit;

		// 0x401B0 - 0x401B7
		Resources * m_pResource;

		// 0x401B8 - 0x401BF
		Overmap * m_pCamera;

		// 0x401C0 - 0x401C7
		unsigned long long m_qwUnknown401C0_401C7;

		// 0x401C8 - 0x401CF
		unsigned long long m_qwUnknown401C8_401CF;

		// 0x401D0 - 0x401D7
		Collision * m_pCollision;

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