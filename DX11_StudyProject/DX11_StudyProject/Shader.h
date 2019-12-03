#pragma once

#include "Component.h"

class CShader : public CComponent
{
public:
	explicit CShader(ID3D11Device* p_Device, ID3D11DeviceContext* p_Context);
	CShader(const CShader& rhs);
	virtual ~CShader();

public:
	inline virtual CComponent* Clone() { return new CShader(*this); }

public:
	inline ID3D11InputLayout* Get_Layout() const { return m_pInputLayout; }

public:
	void Create_VertexShader(LPCWSTR p_filename, LPCSTR p_entrypoint, LPCSTR p_version);
	void Create_PixelShader(LPCWSTR p_filename, LPCSTR p_entrypoint, LPCSTR p_version);
	void Create_ConstantBuffer(LPVOID p_data, UINT p_size);

	void Update_Shader();
	void Update_ConstantBuffer(LPVOID p_data, UINT p_size);

private:
	void Create_InputLayout(ID3D10Blob* p_CompileVS, int p_LayoutType = 0); // 추후 타입 enum화

private:
	ID3D11VertexShader*			m_pVS;
	ID3D11PixelShader*			m_pPS;
	ID3D11Buffer*				m_pCB;
	ID3D11InputLayout*			m_pInputLayout;
};