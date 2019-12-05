#pragma once

#include "Component.h"

class CTexture : public CComponent
{
	struct TEXINFO
	{
		LPCWSTR pTexName;
		ID3D11ShaderResourceView* pTexRV;
		D3D11_TEXTURE2D_DESC texDesc;
	};

private:
	explicit CTexture(ID3D11Device* p_Device, ID3D11DeviceContext* p_Context);
	CTexture(const CTexture& rhs);

public:
	virtual ~CTexture();

public:
	inline virtual CComponent* Clone() { return new CTexture(*this); }

public:
	// RV 요구하는 함수가 더블 포인터 요구
	inline ID3D11ShaderResourceView** Get_TextureRV() { return &m_texInfo.pTexRV; }

public:
	HRESULT Load_Texture(LPCWSTR pTexName, BOOL isWIC);
	HRESULT Load_TextureDesc();

public:
	static CTexture* Create_Texture(ID3D11Device* p_Device, ID3D11DeviceContext* p_Context, LPCWSTR pTexName, BOOL isWIC = TRUE);

private:
	TEXINFO		m_texInfo;
};

