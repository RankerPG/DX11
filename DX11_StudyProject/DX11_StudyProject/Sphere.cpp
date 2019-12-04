#include "Sphere.h"
#include "Device.h"
#include "FigureMesh.h"
#include "Shader.h"
#include "transform.h"

CSphere::CSphere(ID3D11Device* p_Device, ID3D11DeviceContext* p_Context, COMHASHMAP* p_hashMap)
	: CObject(p_Device, p_Context, p_hashMap)
	, m_pMesh(nullptr)
	, m_pTransform(nullptr)
	, m_pShader(nullptr)
	, m_pCB(nullptr)
	, m_pCBMtrl(nullptr)
	, m_mat(TRANSMATRIX())
	, m_mtrl(MATERIAL())
{
}

CSphere::~CSphere()
{
	Release();
}

void CSphere::Init()
{
	assert(m_pMapComponent);

	// �޽� ����
	m_pMesh = static_cast<CMesh*>(m_pMapComponent->find("SphereMesh")->second->Clone());

	// Ʈ������ ����
	m_pTransform = static_cast<CTransform*>(m_pMapComponent->find("Transform")->second->Clone());
	m_pTransform->Set_Trans(XMVectorSet(5.f, 1.f, 0.f, 1.f));

	// ���̴� ����
	m_pShader = static_cast<CShader*>(m_pMapComponent->find("LightShader")->second->Clone());
	m_pShader->Create_ConstantBuffer(&m_mat, sizeof(TRANSMATRIX), &m_pCB);
	m_pShader->Create_ConstantBuffer(&m_mtrl, sizeof(MATERIAL), &m_pCBMtrl);

	m_mtrl.diffuse = XMFLOAT4(0.f, 0.f, 1.f, 1.f);
	m_mtrl.ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.f);
	m_mtrl.specular = XMFLOAT4(1.f, 1.f, 1.f, 8.f);
}

void CSphere::Update(float p_deltaTime)
{
	m_pTransform->Acc_Rotation(XMVectorSet(0.f, p_deltaTime, 0.f, 0.f));

	m_pTransform->Update_Transform();
}

void CSphere::Render()
{
	m_pContext->IASetVertexBuffers(0, 1, m_pMesh->Get_VertexBuffer(), &m_pMesh->Get_Stride(), &m_pMesh->Get_Offset());
	m_pContext->IASetIndexBuffer(m_pMesh->Get_IndexBuffer(), DXGI_FORMAT_R32_UINT, 0);
	m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_pContext->IASetInputLayout(m_pShader->Get_Layout());

	m_pShader->Update_Shader();

	XMStoreFloat4x4(&m_mat.matWorld, m_pTransform->Get_World());
	XMStoreFloat4x4(&m_mat.matWorldRT, InverseTranspose(m_pTransform->Get_World()));
	XMStoreFloat4x4(&m_mat.matWVP, m_pTransform->Get_World() * g_matView * g_matProj);

	m_pShader->Update_ConstantBuffer((void*)&m_mat, sizeof(TRANSMATRIX), m_pCB);
	m_pShader->Update_ConstantBuffer(&m_mtrl, sizeof(MATERIAL), m_pCBMtrl, 2);

	m_pMesh->Draw_Mesh();
}

void CSphere::Release()
{
	SAFE_DELETE(m_pMesh);
	SAFE_DELETE(m_pTransform);
	SAFE_DELETE(m_pShader);

	SAFE_RELEASE(m_pCB);
	SAFE_RELEASE(m_pCBMtrl);
}
