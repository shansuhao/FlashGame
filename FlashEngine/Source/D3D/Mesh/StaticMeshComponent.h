#pragma once
#include <D3D/D3DLibary.h>
#include <Utils/ComPointer.h>

struct Vector4d {
	float X, Y, Z, W = 0;
	
	Vector4d() { X = Y = Z = W = 1.f; }
	Vector4d(float Value) { X = Y = Z = W = Value; }
	Vector4d(const Vector4d& p_Vector4d) { 
		X = p_Vector4d.X;
		Y = p_Vector4d.Y;
		Z = p_Vector4d.Z;
		W = p_Vector4d.W;
	}
	Vector4d(float p_X, float p_Y, float p_Z, float p_W) : X(p_X), Y(p_Y), Z(p_Z), W(p_W) {}

	Vector4d& operator=(const Vector4d & p_Vector4d) {
		X = p_Vector4d.X;
		Y = p_Vector4d.Y;
		Z = p_Vector4d.Z;
		W = p_Vector4d.W;
	}

	float* XYZW() {
		return new float[4] {X,Y,Z,W};
	}
};

struct StaticMeshComponentVertexData {
	float mPosition[4] = {};
	float mTexcoord[4] = {};
	float mNormal[4] = {};
	float mTangent[4] = {};
	
	StaticMeshComponentVertexData() {}

	StaticMeshComponentVertexData(float pPosition[4], float pTexcoord[4], float pNormal[4], float pTangent[4])
	{
		for (size_t i = 0; i < 4; i++)
		{
			mPosition[i] = pPosition[i];
			mTexcoord[i] = pTexcoord[i];
			mNormal[i] = pNormal[i];
			mTangent[i] = pTangent[i];
		}
	}

	//StaticMeshComponentVertexData(float* pPosition, float* pTexcoord, float* pNormal, float* pTangent)
	//{
	//	for (size_t i = 0; i < 4; i++)
	//	{
	//		mPosition[i] = pPosition[i];
	//		mTexcoord[i] = pTexcoord[i];
	//		mNormal[i] = pNormal[i];
	//		mTangent[i] = pTangent[i];
	//	}
	//}
};

class StaticMeshComponent {
public:
	StaticMeshComponent() = default;
public:
	int m_VertexCount;
	ComPointer<ID3D12Resource> m_VBO;
	D3D12_VERTEX_BUFFER_VIEW m_VBOView;
	StaticMeshComponentVertexData* m_VertexData;

public:
	inline StaticMeshComponentVertexData* GetVertexData() const { return m_VertexData; }
	inline ComPointer<ID3D12Resource> GetVBO()const { return m_VBO; }
	inline D3D12_VERTEX_BUFFER_VIEW& GetVBOView() { return m_VBOView; }
	inline int GetVertexCount() { return m_VertexCount; }

	void CreateVBOView();
	void SetVertexCount(UINT p_VertexCount);
	void SetVertexPosition(int p_Index, Vector4d p_Position);
	void SetVertexTexcoord(int p_Index, Vector4d p_Texcoord);
	void SetVertexNomal(int p_Index, Vector4d p_Nomal);
	void SetVertexTangent(int p_Index, Vector4d p_Tangent);
};