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
	m_StaticMeshComponent->m_Material->Active();
	m_StaticMeshComponent->Render();
}