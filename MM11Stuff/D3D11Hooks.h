#pragma once
#include <d3d11.h>
#include "FW1FontWrapper.h"

#define IDKWTFLOLCONSTANT 0x892EA8

typedef HRESULT(__fastcall * oD3D11Present)(IDXGISwapChain*, UINT, UINT);

extern oD3D11Present original_D3D11Present;
extern ID3D11Device *g_pDevice;
extern ID3D11DeviceContext * g_pDeviceContext;
extern IFW1Factory * g_pFontFactory;
extern IFW1FontWrapper * g_pFontWrapper;
extern ID3D11RenderTargetView* g_pRenderTargetView;
extern ID3D11Texture2D* g_pRenderTargetTexture;
extern unsigned long long g_dwOriginalDXGISwapChainPresent_Address;

extern HRESULT __fastcall D3D11Present_Hook(IDXGISwapChain*, UINT, UINT);

extern void SetupD3D11Hooks();

