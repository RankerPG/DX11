#include "Texture.h"
#include "DirectXTex.h"
#include "WICTextureLoader.h"
#include "DDSTextureLoader.h"

CTexture::CTexture(ID3D11Device* p_Device, ID3D11DeviceContext* p_Context)
	: CComponent(p_Device, p_Context)
	, m_texInfo(TEXINFO())
{
}

CTexture::CTexture(const CTexture& rhs)
	: CComponent(rhs.m_pDevice, rhs.m_pContext)
	, m_texInfo(rhs.m_texInfo)
{
	m_texInfo.pTexRV->AddRef();
}

CTexture::~CTexture()
{
	SAFE_RELEASE(m_texInfo.pTexRV);
}

HRESULT CTexture::Load_Texture(LPCWSTR pTexName, BOOL isWIC)
{
	HRESULT hr;

	m_texInfo.pTexName = pTexName;

	if (TRUE == isWIC)
	{
		if (FAILED(hr = CreateWICTextureFromFileEx(m_pDevice, m_pContext, pTexName, 0, D3D11_USAGE_DEFAULT,
			D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET,
			0, D3D11_RESOURCE_MISC_GENERATE_MIPS,
			WIC_LOADER_DEFAULT, nullptr,
			&m_texInfo.pTexRV)))
		{
			MessageBox(g_hWnd, L"Texture Load Failed!!", 0, 0);
			return hr;
		}
	}
	else
	{
		if (FAILED(hr = CreateDDSTextureFromFile(m_pDevice, m_pContext, pTexName, nullptr, &m_texInfo.pTexRV)))
		{
			MessageBox(g_hWnd, L"Texture Load Failed!!", 0, 0);
			return hr;
		}
	}

	return hr;
}

HRESULT CTexture::Load_TextureDesc()
{
	HRESULT hr = S_OK;

	D3D11_SHADER_RESOURCE_VIEW_DESC dc;
	m_texInfo.pTexRV->GetDesc(&dc);

	ID3D11Resource* pRes = nullptr;
	m_texInfo.pTexRV->GetResource(&pRes);

	ID3D11Texture2D* pTex = nullptr;
	pRes->QueryInterface<ID3D11Texture2D>(&pTex);

	if (nullptr != pTex)
	{
		pTex->GetDesc(&m_texInfo.texDesc);

		SAFE_RELEASE(pTex);
	}

	return hr;
}

CTexture* CTexture::Create_Texture(ID3D11Device* p_Device, ID3D11DeviceContext* p_Context, LPCWSTR pTexName, BOOL isWIC)
{
	CTexture* pInstance = new CTexture(p_Device, p_Context);

	if (FAILED(pInstance->Load_Texture(pTexName, isWIC)))
	{
		MessageBox(g_hWnd, L"CTexture Create Failed!!", 0, 0);
		SAFE_DELETE(pInstance);
		return nullptr;
	}

	return pInstance;
}
