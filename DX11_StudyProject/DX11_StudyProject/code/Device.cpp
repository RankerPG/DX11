#include "Device.h"

CDevice* CDevice::m_pInstance = nullptr;

CDevice::CDevice()
	: m_pDeivce(nullptr)
	, m_pContext(nullptr)
	, m_pSwapChain(nullptr)
	, m_pRenderTargetView(nullptr)
	, m_pDepthStencilView(nullptr)
	, m_ClearColor(0.f, 0.f, 1.f, 1.f)
	, m_dwSizeX(0)
	, m_dwSizeY(0)
	, m_dw4xMSAAQuality(0)
{
}

CDevice::~CDevice()
{
	m_pDepthStencilView->Release();
	m_pRenderTargetView->Release();
	m_pSwapChain->Release();
	m_pContext->Release();
	m_pDeivce->Release();
}

BOOL CDevice::Init_Device(UINT p_dwSizeX, UINT p_dwSizeY)
{
	m_dwSizeX = p_dwSizeX;
	m_dwSizeY = p_dwSizeY;

	Create_Device();
	Create_SwapChain();
	Create_RenderTargetView();
	Create_DepthStencilView();

	assert(nullptr != m_pContext);

	m_pContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);

	Init_Viewport();

	return TRUE;
}

void CDevice::Clear_BackBuffer()
{
	assert(m_pContext);
	assert(m_pSwapChain);

	m_pContext->ClearRenderTargetView(m_pRenderTargetView, (float*)&m_ClearColor);
	m_pContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

	m_pSwapChain->Present(0, 0);
}

BOOL CDevice::Create_Device()
{
	UINT createDeviceFlags = 0;
	
#ifdef DEBUG | _DEBUG
	createDeviceFlags = D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL featureLevel;

	HRESULT hr;
	
	if (FAILED(hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr
		, createDeviceFlags, nullptr, 0, D3D11_SDK_VERSION
		, &m_pDeivce, &featureLevel, &m_pContext)))
	{
		MessageBox(0, L"D3D11CreateDeivce Failed!!", 0, 0);
		return FALSE;
	}

	if (D3D_FEATURE_LEVEL_11_0 > featureLevel)
	{
		MessageBox(0, L"Driect3D Feature Level 11 unsupported!!", 0, 0);
		return FALSE;
	}

	hr = m_pDeivce->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &m_dw4xMSAAQuality);

	assert(m_dw4xMSAAQuality > 0);

	return TRUE;
}

BOOL CDevice::Create_SwapChain()
{
	assert(nullptr != m_pDeivce);

	DXGI_SWAP_CHAIN_DESC sd;
	sd.BufferDesc.Width = m_dwSizeX;
	sd.BufferDesc.Height = m_dwSizeY;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	if (0 < m_dw4xMSAAQuality)
	{
		sd.SampleDesc.Count = 4;
		sd.SampleDesc.Quality = m_dw4xMSAAQuality - 1;
	}
	else
	{
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
	}

	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1;
	sd.OutputWindow = g_hWnd;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;

	HRESULT hr = S_OK;

	IDXGIDevice* dxgiDevice = nullptr;

	if (FAILED(hr = m_pDeivce->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice)))
	{
		MessageBox(0, L"IDXGIDevice Create Failed!!", 0, 0);
		return FALSE;
	}

	IDXGIAdapter* dxgiAdapter = nullptr;

	if (FAILED(hr = dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter)))
	{
		MessageBox(0, L"IDXGIAdapter Create Failed!!", 0, 0);
		dxgiDevice->Release();
		return FALSE;
	}

	IDXGIFactory* dxgiFactory = nullptr;

	if (FAILED(hr = dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory)))
	{
		MessageBox(0, L"IDXGIFactory Create Failed!!", 0, 0);
		dxgiAdapter->Release();
		dxgiDevice->Release();
		return FALSE;
	}

	if (FAILED(hr = dxgiFactory->CreateSwapChain(m_pDeivce, &sd, &m_pSwapChain)))
	{
		MessageBox(0, L"SwapChain Create Failed!!", 0, 0);
		dxgiFactory->Release();
		dxgiAdapter->Release();
		dxgiDevice->Release();
		return FALSE;
	}

	dxgiFactory->Release();
	dxgiAdapter->Release();
	dxgiDevice->Release();

	return TRUE;
}

BOOL CDevice::Create_RenderTargetView()
{
	assert(nullptr != m_pDeivce);

	HRESULT hr;

	ID3D11Texture2D* pBackBuffer;

	if (FAILED(hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D)
		, reinterpret_cast<void**>(&pBackBuffer))))
	{
		MessageBox(0, L"ID3D11Texture2D Create Failed!!", 0, 0);
		return FALSE;
	}

	if (FAILED(hr = m_pDeivce->CreateRenderTargetView(pBackBuffer, nullptr, &m_pRenderTargetView)))
	{
		MessageBox(0, L"mRenderTargetView Create Failed!!", 0, 0);
		pBackBuffer->Release();
		return FALSE;
	}

	pBackBuffer->Release();

	return TRUE;
}

BOOL CDevice::Create_DepthStencilView()
{
	assert(nullptr != m_pDeivce);

	HRESULT hr;

	D3D11_TEXTURE2D_DESC depthStencilDesc;
	depthStencilDesc.Width = m_dwSizeX;
	depthStencilDesc.Height = m_dwSizeY;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	if (0 < m_dw4xMSAAQuality)
	{
		depthStencilDesc.SampleDesc.Count = 4;
		depthStencilDesc.SampleDesc.Quality = m_dw4xMSAAQuality - 1;
	}
	else
	{
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
	}

	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	ID3D11Texture2D* pDepthStencilBuffer;

	if (FAILED(hr = m_pDeivce->CreateTexture2D(&depthStencilDesc, nullptr, &pDepthStencilBuffer)))
	{
		MessageBox(0, L"DepthStencilBuffer Create Failed!!", 0, 0);
		return FALSE;
	}

	if (FAILED(hr = m_pDeivce->CreateDepthStencilView(pDepthStencilBuffer, nullptr, &m_pDepthStencilView)))
	{
		MessageBox(0, L"DepthStencilView Create Failed!!", 0, 0);
		return FALSE;
	}

	pDepthStencilBuffer->Release();

	return TRUE;
}

BOOL CDevice::Init_Viewport()
{
	assert(nullptr != m_pContext);

	D3D11_VIEWPORT vp;
	vp.TopLeftX = 0.f;
	vp.TopLeftY = 0.f;
	vp.Width = static_cast<float>(m_dwSizeX);
	vp.Height = static_cast<float>(m_dwSizeY);
	vp.MinDepth = 0.f;
	vp.MaxDepth = 1.f;

	m_pContext->RSSetViewports(1, &vp);

	return TRUE;
}
