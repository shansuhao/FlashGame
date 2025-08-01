#include "pch.h"
#include "D3D/Shader/D3DShader.h"
#include "StaticMeshComponent.h"
#include <D3D/DXContext.h>

bool StaticMeshComponent::InitFromFile(const char* p_MeshFile) {
	FILE* pFile = nullptr;
	BOOL b_isCreateBufferObject = true;
	errno_t err = fopen_s(&pFile, p_MeshFile, "rb");
	if (err == 0)
	{
		int temp = 0;
		fread(&temp, 4, 1, pFile);

		m_VertexCount = temp;
		m_VertexData = new StaticMeshComponentVertexData[m_VertexCount];
		fread(m_VertexData, 1, sizeof(StaticMeshComponentVertexData) * m_VertexCount, pFile);

		b_isCreateBufferObject = D3DShader::Get().CreateBufferOBject(m_VBO, sizeof(StaticMeshComponentVertexData) * m_VertexCount,
			m_VertexData, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

		if (!b_isCreateBufferObject)
		{
			fclose(pFile);
			return false;
		}
		CreateVBOView();

		while (!feof(pFile))
		{
			fread(&temp, 4, 1, pFile);
			if (feof(pFile))
			{
				break;
			}
			char name[256] = { 0 };
			fread(name, 1, temp, pFile);
			fread(&temp, 4, 1, pFile);
			SubMesh* submesh = new SubMesh;
			unsigned int* indexes = new unsigned int[temp];
			fread(indexes, 1, sizeof(unsigned int) * temp, pFile);
			b_isCreateBufferObject = D3DShader::Get().CreateBufferOBject(submesh->m_IBO, sizeof(unsigned int) * temp,
				indexes, D3D12_RESOURCE_STATE_INDEX_BUFFER);
			if (!b_isCreateBufferObject)
			{
				break;
			}
			submesh->m_IBView.BufferLocation = submesh->m_IBO->GetGPUVirtualAddress();
			submesh->m_IBView.SizeInBytes = sizeof(unsigned int) * temp;
			submesh->m_IBView.Format = DXGI_FORMAT_R32_UINT;

			m_SubMeshes.insert(std::pair<std::string, SubMesh*>(name, submesh));
			delete[] indexes;
		}
 		fclose(pFile);
	}
	return b_isCreateBufferObject;
}

void StaticMeshComponent::Render() {
	D3D12_VERTEX_BUFFER_VIEW vbos[] = {
		m_VBOView
	};
	DXContext::Get().GetCommandList()->IASetVertexBuffers(0, 1, vbos);
	if (m_SubMeshes.empty())
	{
		DXContext::Get().GetCommandList()->DrawInstanced(m_VertexCount, 1, 0, 0);
	}
	for (auto iter = m_SubMeshes.begin(); iter != m_SubMeshes.end(); iter++)
	{
		DXContext::Get().GetCommandList()->IASetIndexBuffer(&iter->second->m_IBView);
		DXContext::Get().GetCommandList()->DrawIndexedInstanced(iter->second->m_IndexCount, 1, 0, 0, 0);
	}
}

void StaticMeshComponent::CreateVBOView() {
	// ����VBO
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
