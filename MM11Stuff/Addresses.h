#pragma once

#define MPGAME_PTR 0x00000001409D2520
#define MPGAME_UPDATE_ADDRESS 0x149dcb035

#define MPGAME_GETINPUTSTATE04_ADDRESS 0x00000001403DFF00
#define XINPUT_IAT_ADDRESS 0x140B83220
#define XINPUT_GETCAPS_IAT_ADDRESS  0x140B83210
#define MPGAME_INPUT_POINTER 0x140C3FE60

#define REGISTERCLASSEXW_IAT_ADDRESS 0x000000014E0066A8
#define TIMEGETTIME_IAT_ADDRESS      0x00000001409B0738

#define MPENVIRONMENTVARIABLES_ADDRESS 0x140C3F8A0
#define D3D11MANAGER_ADDRESS 0x140C408F0

//000000014BE73794 | 48:8B0D 4DE7DCF4         | mov rcx,qword ptr ds:[140C41EE8]                                           |
#define MPGAME_PHYSICSENGINE_POINTER  0x140C41EE8

//000000014912092F | 48:8B05 2AF5B1F7         | mov rax,qword ptr ds:[140C3FE60]                             |0