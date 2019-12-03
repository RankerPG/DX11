#include "Box.h"
#include "Device.h"
#include "FigureMesh.h"
#include "Shader.h"
#include "transform.h"

CBox::CBox(ID3D11Device* p_Device, ID3D11DeviceContext* p_Context, COMHASHMAP* p_hashMap)
	: CObject(p_Device, p_Context, p_hashMap)
	, m_pMesh(nullptr)
	, m_pTransform(nullptr)
	, m_pShader(nullptr)
{
}

CBox::~CBox()
{
	Release();
}

void CBox::Init()
{
	assert(m_pMapComponent);

	// 메쉬 생성
	m_pMesh = static_cast<CMesh*>(m_pMapComponent->find("CubeMesh")->second->Clone());

	// 트랜스폼 생성
	m_pTransform = static_cast<CTransform*>(m_pMapComponent->find("Transform")->second->Clone());
	m_pTransform->Set_Trans(XMVectorSet(0.f, 1.f, 0.f, 1.f));

	// 쉐이더 생성
	m_pShader = static_cast<CShader*>(m_pMapComponent->find("DefaultShader")->second->Clone());
}

void CBox::Update(float p_deltaTime)
{
	m_pTransform->Acc_Rotation(XMVectorSet(0.f, p_deltaTime, 0.f, 0.f));

	m_pTransform->Update_Transform();
}

void CBox::Render()
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

void CBox::Release()
{
	SAFE_DELETE(m_pMesh);
	SAFE_DELETE(m_pTransform);
	SAFE_DELETE(m_pShader);
}
