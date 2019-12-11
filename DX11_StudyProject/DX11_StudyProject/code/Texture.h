#pragma once

#include "Component.h"

class CTexture : public CComponent
{
	struct TEXINFO
	{
		wchar_t pTexName[64];
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
	inline ID3D11ShaderResourceView** Get_TextureRV(UINT p_texIdx = 0) { return &((*m_pVecTexInfo)[p_texIdx].pTexRV); }
	inline D3D11_TEXTURE2D_DESC Get_TextureDesc() { return (*m_pVecTexInfo)[0].texDesc; }

public:
	HRESULT Load_Texture(LPCWSTR pTexName, BOOL p_isWIC);
	HRESULT Load_Texture_Array(LPCWSTR pTexName, BOOL p_isWIC, UINT p_Cnt);
	HRESULT Load_TextureDesc();

public:
	static CTexture* Create_Texture(ID3D11Device* p_Device, ID3D11DeviceContext* p_Context, LPCWSTR pTexName, BOOL p_isWIC = TRUE, UINT p_Cnt = 1);

private:
	vector<TEXINFO>*	m_pVecTexInfo;
};

