#pragma once

#pragma pack(push, 1)
class CGameWorker
{
public:

	static void CallUpdate(unsigned long ofs, unsigned long long ecx)
	{
		unsigned long long rcx = *(unsigned long long*)(ecx + ofs);
		unsigned long long rax = *(unsigned long long*)(rcx);
		rax = *(unsigned long long*)(rax + 0x30);

		((void(__thiscall*)(unsigned long long))rax)(rcx);

	}
	// M_0->Upd
	class Foo {
	public:

		// 0x00 - 0x07
		virtual void Ctor() = 0;

		// 0x08 - 0x0F
		virtual void Dtor() = 0;

		// 0x10 - 0x17
		virtual void Unk1() = 0;

		// 0x18 = 0x1F
		virtual void Unk2() = 0;

		// 0x20 = 0x27
		virtual void Unk3() = 0;

		// 0x28 - 0x2F
		virtual void Unk4() = 0;

		// 0x30 - 0x37
		virtual void Update() = 0;
	};

	//@0x00
	Foo * m_pSelf;



protected:

private:

};
#pragma pack(pop)
