#pragma once

#include "Component.h"

class CMesh abstract : public CComponent
{
public:
	explicit CMesh(ID3D11Device* p_Device, ID3D11DeviceContext* p_Context);
	virtual ~CMesh();

public:
	// 이중 포인터를 요구하기 때문에 l-value 문제로 클래스 자체에서 이중 포인터 리턴
	inline ID3D11Buffer** Get_VertexBuffer() { return &m_pVB; }
	inline ID3D11Buffer* Get_IndexBuffer() { return m_pIB; }

	inline const UINT& const Get_Stride() const { return m_dwStride; }
	inline const UINT& const Get_Offset() const { return m_dwOffset; }

public:
	virtual void Init_Mesh() = 0;

protected:
	ID3D11Buffer*			m_pVB;
	ID3D11Buffer*			m_pIB;

	UINT					m_dwStride;
	UINT					m_dwOffset;
};

