#include "pch.h"
#include "SceneNode.h"
#include "D3D/DXContext.h"
#include "D3D/Mesh/StaticMeshComponent.h"

SceneNode::SceneNode()
{
	m_bNeedUpdate = true;
	m_StaticMeshComponent = nullptr;
}

void SceneNode::Update(float inX, float inY, float inZ)
{
	m_bNeedUpdate = true;
	m_Position = DirectX::XMVectorSet(inX, inY, inZ, 1.0f);
}

void SceneNode::Render() {
	if (m_bNeedUpdate)
	{
		DirectX::XMMATRIX modelMatrix = DirectX::XMMatrixTranslation(0.0f, 0.0f, 0.f);
		DirectX::XMFLOAT4X4 tempMatrix;
		float matrices[32];
		DirectX::XMStoreFloat4x4(&tempMatrix, modelMatrix);
		memcpy(matrices, &tempMatrix, sizeof(float) * 16);
		DirectX::XMVECTOR determinant;
		DirectX::XMMATRIX inverseModelMatrix = DirectX::XMMatrixInverse(&determinant, modelMatrix);
		if (DirectX::XMVectorGetX(determinant) != 0.0f)
		{
			DirectX::XMMATRIX normalMatrix = DirectX::XMMatrixTranspose(inverseModelMatrix);
			DirectX::XMStoreFloat4x4(&tempMatrix, modelMatrix);
			memcpy(matrices + 16, &tempMatrix, sizeof(float) * 16);
		}
		DXContext::Get().UpdateConstantBuffer(m_StaticMeshComponent->m_Material->m_ConstantBuffer, matrices, sizeof(float) * 32);
		m_bNeedUpdate = false;
	}

	m_StaticMeshComponent->m_Material->Active();
	m_StaticMeshComponent->Render();
}