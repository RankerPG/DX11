#pragma once

#include "Component.h"

class CMesh abstract : public CComponent
{
public:
	explicit CMesh(ID3D11Device* p_Device, ID3D11DeviceContext* p_Context);
	virtual ~CMesh();

public:
	// ���� �����͸� �䱸�ϱ� ������ l-value ������ Ŭ���� ��ü���� ���� ������ ����
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

