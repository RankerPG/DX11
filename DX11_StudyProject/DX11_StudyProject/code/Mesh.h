#pragma once

#include "Component.h"

class CMesh abstract : public CComponent
{
public:
	explicit CMesh(ID3D11Device* p_Device, ID3D11DeviceContext* p_Context);
	CMesh(const CMesh& rhs);
	virtual ~CMesh();

public:
	// ���� �����͸� �䱸�ϱ� ������ l-value ������ Ŭ���� ��ü���� ���� ������ ����
	inline ID3D11Buffer** Get_VertexBuffer() { return &m_pVB; }
	inline ID3D11Buffer* Get_IndexBuffer() { return m_pIB; }

	inline UINT& Get_Stride() { return m_dwStride; }
	inline UINT& Get_Offset() { return m_dwOffset; }

public:
	virtual void Init_Mesh() = 0;

	void Draw_Mesh();

protected:
	ID3D11Buffer*			m_pVB;
	ID3D11Buffer*			m_pIB;

	UINT					m_dwStride;
	UINT					m_dwOffset;
	UINT					m_dwIdxCnt;
};

