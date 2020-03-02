#include "Texture.h"
#include "DirectXTex.h"
#include "WICTextureLoader.h"
#include "DDSTextureLoader.h"

CTexture::CTexture(ID3D11Device* p_Device, ID3D11DeviceContext* p_Context)
	: CComponent(p_Device, p_Context)
	, m_pVecTexInfo(nullptr)
{
}

CTexture::CTexture(const CTexture& rhs)
	: CComponent(rhs)
	, m_pVecTexInfo(rhs.m_pVecTexInfo)
{
}

CTexture::~CTexture()
{
	if (FALSE == m_isClone)
	{
		UINT size = m_pVecTexInfo->size();

		for (UINT i = 0; i < size; ++i)
		{
			SAFE_RELEASE((*m_pVecTexInfo)[i].pTexRV);
		}

		m_pVecTexInfo->clear();
		SAFE_DELETE(m_pVecTexInfo);
	}
}

HRESULT CTexture::Load_Texture(LPCWSTR pTexName, BOOL p_isWIC)
{
	HRESULT hr = S_OK;

	m_pVecTexInfo = new vector<TEXINFO>(1);

	wsprintf((*m_pVecTexInfo)[0].pTexName, pTexName);

	if (TRUE == p_isWIC)
	{
		if (FAILED(hr = CreateWICTextureFromFileEx(m_pDevice, m_pContext, pTexName, 0, D3D11_USAGE_DEFAULT,
			D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET,
			0, D3D11_RESOURCE_MISC_GENERATE_MIPS,
			WIC_LOADER_DEFAULT, nullptr,
			&(*m_pVecTexInfo)[0].pTexRV)))
		{
			MessageBox(g_hWnd, L"Texture Load Failed!!", 0, 0);
			return hr;
		}
	}
	else
	{
		if (FAILED(hr = CreateDDSTextureFromFile(m_pDevice, m_pContext, pTexName, nullptr, &(*m_pVecTexInfo)[0].pTexRV)))
		{
			MessageBox(g_hWnd, L"Texture Load Failed!!", 0, 0);
			return hr;
		}
	}

	return hr;
}

HRESULT CTexture::Load_Texture_Array(LPCWSTR pTexName, BOOL p_isWIC, UINT p_Cnt)
{
	HRESULT hr = S_OK;

	m_pVecTexInfo = new vector<TEXINFO>(p_Cnt);

	for (UINT i = 0; i < p_Cnt; ++i)
	{
		wsprintf((*m_pVecTexInfo)[i].pTexName, pTexName, i);

		if (TRUE == p_isWIC)
		{
			if (FAILED(hr = CreateWICTextureFromFileEx(m_pDevice, m_pContext, (*m_pVecTexInfo)[i].pTexName, 0, D3D11_USAGE_DEFAULT,
				D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET,
				0, D3D11_RESOURCE_MISC_GENERATE_MIPS,
				WIC_LOADER_DEFAULT, nullptr,
				&(*m_pVecTexInfo)[i].pTexRV)))
			{
				MessageBox(g_hWnd, L"Texture Load Failed!!", 0, 0);
				return hr;
			}
		}
		else
		{
			if (FAILED(hr = CreateDDSTextureFromFile(m_pDevice, m_pContext, (*m_pVecTexInfo)[i].pTexName, nullptr, &(*m_pVecTexInfo)[i].pTexRV)))
			{
				MessageBox(g_hWnd, L"Texture Load Failed!!", 0, 0);
				return hr;
			}
		}
	}

	return hr;
}

HRESULT CTexture::Load_TextureDesc()
{
	HRESULT hr = S_OK;

	D3D11_SHADER_RESOURCE_VIEW_DESC dc;
	(*m_pVecTexInfo)[0].pTexRV->GetDesc(&dc);

	ID3D11Resource* pRes = nullptr;
	(*m_pVecTexInfo)[0].pTexRV->GetResource(&pRes);

	ID3D11Texture2D* pTex = nullptr;
	pRes->QueryInterface<ID3D11Texture2D>(&pTex);

	if (nullptr != pTex)
	{
		pTex->GetDesc(&(*m_pVecTexInfo)[0].texDesc);

		SAFE_RELEASE(pTex);
	}

	return hr;
}

HRESULT CTexture::Create_RandomTexture()
{
	m_pVecTexInfo = new vector<TEXINFO>(1);

	XMFLOAT4 vRandomValues[512];

	for (int i = 0; i < 512; ++i)
	{
		vRandomValues[i].x = (float)rand() / RAND_MAX;
		vRandomValues[i].y = (float)rand() / RAND_MAX;
		vRandomValues[i].z = (float)rand() / RAND_MAX;
		vRandomValues[i].w = (float)rand() / RAND_MAX;
	}

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = vRandomValues;
	initData.SysMemPitch = 512 * sizeof(XMFLOAT4);
	initData.SysMemSlicePitch = 0;

	D3D11_TEXTURE1D_DESC texDesc;
	texDesc.Width = 512;
	texDesc.MipLevels = 1;
	texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	texDesc.Usage = D3D11_USAGE_IMMUTABLE;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;
	texDesc.ArraySize = 1;

	ID3D11Texture1D* pTexture = nullptr;

	if (FAILED(m_pDevice->CreateTexture1D(&texDesc, &initData, &pTexture)))
	{
		MessageBox(g_hWnd, L"Random Texture Create Failed!!", 0, 0);
		return E_FAIL;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
	viewDesc.Format = texDesc.Format;
	viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
	viewDesc.Texture1D.MipLevels = 1;
	viewDesc.Texture1D.MostDetailedMip = 0;

	TEXINFO texinfo;

	if (FAILED(m_pDevice->CreateShaderResourceView(pTexture, &viewDesc, &texinfo.pTexRV)))
	{
		MessageBox(g_hWnd, L"Random SRV Create Failed!!", 0, 0);
		SAFE_RELEASE(pTexture);
		return E_FAIL;
	}

	SAFE_RELEASE(pTexture);

	m_pVecTexInfo->push_back(texinfo);

	return S_OK;
}

CTexture* CTexture::Create_Texture(ID3D11Device* p_Device, ID3D11DeviceContext* p_Context, LPCWSTR pTexName, BOOL p_isWIC, UINT p_Cnt)
{
	CTexture* pInstance = new CTexture(p_Device, p_Context);

	if (1 == p_Cnt)
	{
		if (FAILED(pInstance->Load_Texture(pTexName, p_isWIC)))
		{
			MessageBox(g_hWnd, L"CTexture Create Failed!!", 0, 0);
			SAFE_DELETE(pInstance);
			return nullptr;
		}
	}
	else
	{
		if (FAILED(pInstance->Load_Texture_Array(pTexName, p_isWIC, p_Cnt)))
		{
			MessageBox(g_hWnd, L"CTexture Create Failed!!", 0, 0);
			SAFE_DELETE(pInstance);
			return nullptr;
		}
	}

	return pInstance;
}

CTexture* CTexture::Create_Texture(ID3D11Device* p_Device, ID3D11DeviceContext* p_Context)
{
	CTexture* pInstance = new CTexture(p_Device, p_Context);

	if (FAILED(pInstance->Create_RandomTexture()))
	{
		MessageBox(g_hWnd, L"CTexture Create Failed!!", 0, 0);
		SAFE_DELETE(pInstance);
		return nullptr;
	}

	return pInstance;
}
