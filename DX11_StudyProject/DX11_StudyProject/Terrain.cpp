#include "Terrain.h"
#include "Device.h"
#include "FigureMesh.h"
#include "Shader.h"
#include "transform.h"

CTerrain::CTerrain(ID3D11Device* p_Device, ID3D11DeviceContext* p_Context, COMHASHMAP* p_hashMap)
	: CObject(p_Device, p_Context, p_hashMap)
	, m_pMesh(nullptr)
	, m_pTransform(nullptr)
	, m_pShader(nullptr)
{
}

CTerrain::~CTerrain()
{
	Release();
}

void CTerrain::Init()
{
	assert(m_pMapComponent);

	// 메쉬 생성
	m_pMesh = static_cast<CMesh*>(m_pMapComponent->find("TerrainMesh")->second->Clone());

	// 트랜스폼 생성
	m_pTransform = static_cast<CTransform*>(m_pMapComponent->find("Transform")->second->Clone());

	// 쉐이더 생성
	m_pShader = static_cast<CShader*>(m_pMapComponent->find("DefaultShader")->second->Clone());
}

void CTerrain::Update(float p_deltaTime)
{
	m_pTransform->Update_Transform();
}

void CTerrain::Render()
{
	m_pContext->IASetVertexBuffers(0, 1, m_pMesh->Get_VertexBuffer(), &m_pMesh->Get_Stride(), &m_pMesh->Get_Offset());
	m_pContext->IASetIndexBuffer(m_pMesh->Get_IndexBuffer(), DXGI_FORMAT_R32_UINT, 0);
	m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_pContext->IASetInputLayout(m_pShader->Get_Layout());

	m_pShader->Update_Shader();

	XMVECTOR pos = XMVectorSet(0.f, 10.f, -20.f, 1.f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	XMMATRIX matWVP = m_pTransform->Get_World() * g_matView * g_matProj;

	m_pShader->Update_ConstantBuffer((void*)&matWVP, sizeof(XMMATRIX));

	m_pMesh->Draw_Mesh();
}

void CTerrain::Release()
{
	SAFE_DELETE(m_pMesh);
	SAFE_DELETE(m_pTransform);
	SAFE_DELETE(m_pShader);
}
