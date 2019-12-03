#include "Box.h"
#include "Device.h"
#include "FigureMesh.h"
#include "Shader.h"
#include "transform.h"

CBox::CBox(ID3D11Device* p_Device, ID3D11DeviceContext* p_Context)
	: m_pDevice(p_Device)
	, m_pContext(p_Context)
	, m_pMesh(nullptr)
	, m_pTransform(nullptr)
	, m_pShader(nullptr)
{
	m_pDevice->AddRef();
	m_pContext->AddRef();
}

CBox::~CBox()
{
	Release();
}

void CBox::Init()
{
	// 메쉬 생성
	m_pMesh.reset(new CFigureMesh(m_pDevice, m_pContext));
	m_pMesh->Init_Mesh();

	// 트랜스폼 생성
	m_pTransform.reset(new CTransform(m_pDevice, m_pContext));

	// 쉐이더 생성
	m_pShader.reset(new CShader(m_pDevice, m_pContext));
	m_pShader->Create_VertexShader(L"./Shader/Default.fx", "vs_main", "vs_5_0");
	m_pShader->Create_PixelShader(L"./Shader/Default.fx", "ps_main", "ps_5_0");
	m_pShader->Create_ConstantBuffer((void*)&m_pTransform->Get_World(), sizeof(m_pTransform->Get_World()));
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

	XMMATRIX matView = XMMatrixLookAtLH(pos, target, up);

	XMMATRIX matProj = XMMatrixPerspectiveFovLH(XM_PIDIV4, 1280.f / 720.f, 1.f, 1000.f);
	
	XMMATRIX matWVP = m_pTransform->Get_World() * matView * matProj;

	m_pShader->Update_ConstantBuffer((void*)&matWVP, sizeof(XMMATRIX));

	m_pContext->DrawIndexed(36, 0, 0);
}

void CBox::Release()
{
	m_pDevice->Release();
	m_pContext->Release();
}
