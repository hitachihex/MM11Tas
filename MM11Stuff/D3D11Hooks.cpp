#include "stdafx.h"
#include "D3D11Hooks.h"
#include "Addresses.h"
#include "DebugUtils.h"

// Lol.. forgot to link to the library. Wat.
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "FW1FontWrapper.lib")
#pragma comment(lib, "dxgi.lib")



oD3D11Present original_D3D11Present = (oD3D11Present)0x0;

ID3D11Device *g_pDevice = NULL;
ID3D11DeviceContext * g_pDeviceContext = NULL;
IFW1Factory * g_pFontFactory = NULL;
IFW1FontWrapper * g_pFontWrapper = NULL;
ID3D11RenderTargetView* g_pRenderTargetView = NULL;
ID3D11Texture2D* g_pRenderTargetTexture = NULL;

unsigned long long g_dwOriginalDXGISwapChainPresent_Address = 0x0;

HRESULT __fastcall D3D11Present_Hook(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
	static bool bOnce = false;

	if (!bOnce)
	{
		DebugOutput("D3D11Present_Hook, what's going on yo?");
		
		pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&g_pDevice);
		g_pDevice->GetImmediateContext(&g_pDeviceContext);
		

		FW1CreateFactory(FW1_VERSION, &g_pFontFactory);
		

		g_pFontFactory->CreateFontWrapper(g_pDevice, L"Tacoma", &g_pFontWrapper);
		g_pFontFactory->Release();

		// Seems this below spits out Unsupported function??
		
		bOnce = true;
	}

	pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&g_pDevice);
	g_pDevice->GetImmediateContext(&g_pDeviceContext);


	ID3D11Texture2D * pBackBuffer = nullptr;
	pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer);
	ID3D11RenderTargetView * pRTv = nullptr;

	g_pDevice->CreateRenderTargetView(pBackBuffer, NULL, &pRTv);
	g_pDeviceContext->OMSetRenderTargets(1, &pRTv, NULL);
	D3D11_TEXTURE2D_DESC backBufferDesc;

	if (g_pFontWrapper)
		g_pFontWrapper->DrawString(g_pDeviceContext, L"Hello, world!", 10.0f, 16.0, 16.0, 0xff0000, FW1_RESTORESTATE | FW1_LEFT | FW1_NOFLUSH);

	pBackBuffer->GetDesc(&backBufferDesc);


	pBackBuffer->Release();


	return original_D3D11Present(pSwapChain, SyncInterval, Flags);
}

void SetupD3D11Hooks()
{
	unsigned long long _RCX = *(unsigned long long*)(D3D11MANAGER_ADDRESS);

	// boop boop
	unsigned long long _RDI = *(unsigned long long*)(_RCX + 0x0 * 0x08 + IDKWTFLOLCONSTANT);

	unsigned long long _RAX = *(unsigned long long*)(_RDI);

	g_dwOriginalDXGISwapChainPresent_Address = *(unsigned long long*)(_RAX + 0x40);
	*(unsigned long long*)(&original_D3D11Present) = g_dwOriginalDXGISwapChainPresent_Address;

	DWORD dwOldProt;
	VirtualProtect((LPVOID)&*(unsigned long long*)(_RAX + 0x40), 0x8, PAGE_EXECUTE_READWRITE, &dwOldProt);

	*(unsigned long long*)(_RAX + 0x40) = (unsigned long long)D3D11Present_Hook;

	DebugOutput("Hooked DXGISwapChain::Present");
}