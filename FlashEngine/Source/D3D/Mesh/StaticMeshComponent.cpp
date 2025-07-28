#include "pch.h"
#include "StaticMeshComponent.h"

void StaticMeshComponent::CreateVBOView() {
	// ´´½¨VBO
	m_VBOView.BufferLocation = m_VBO->GetGPUVirtualAddress();
	m_VBOView.SizeInBytes = sizeof(StaticMeshComponentVertexData) * m_VertexCount;
	m_VBOView.StrideInBytes = sizeof(StaticMeshComponentVertexData);
}

void StaticMeshComponent::SetVertexCount(UINT p_VertexCount)
{
	m_VertexCount = p_VertexCount;
	m_VertexData = new StaticMeshComponentVertexData[m_VertexCount];
	memset(m_VertexData, 0, sizeof(StaticMeshComponentVertexData) * p_VertexCount);
}

void StaticMeshComponent::SetVertexPosition(int p_Index, Vector4d p_Position)
{
	m_VertexData[p_Index].mPosition[0] = p_Position.X;
	m_VertexData[p_Index].mPosition[1] = p_Position.Y;
	m_VertexData[p_Index].mPosition[2] = p_Position.Z;
	m_VertexData[p_Index].mPosition[3] = p_Position.W;
}

void StaticMeshComponent::SetVertexTexcoord(int p_Index, Vector4d p_Texcoord)
{
	m_VertexData[p_Index].mTexcoord[0] = p_Texcoord.X;
	m_VertexData[p_Index].mTexcoord[1] = p_Texcoord.Y;
	m_VertexData[p_Index].mTexcoord[2] = p_Texcoord.Z;
	m_VertexData[p_Index].mTexcoord[3] = p_Texcoord.W;
}

void StaticMeshComponent::SetVertexNomal(int p_Index, Vector4d p_Nomal)
{
	m_VertexData[p_Index].mNormal[0] = p_Nomal.X;
	m_VertexData[p_Index].mNormal[1] = p_Nomal.Y;
	m_VertexData[p_Index].mNormal[2] = p_Nomal.Z;
	m_VertexData[p_Index].mNormal[3] = p_Nomal.W;
}

void StaticMeshComponent::SetVertexTangent(int p_Index, Vector4d p_Tangent)
{
	m_VertexData[p_Index].mTangent[0] = p_Tangent.X;
	m_VertexData[p_Index].mTangent[1] = p_Tangent.Y;
	m_VertexData[p_Index].mTangent[2] = p_Tangent.Z;
	m_VertexData[p_Index].mTangent[3] = p_Tangent.W;
}
