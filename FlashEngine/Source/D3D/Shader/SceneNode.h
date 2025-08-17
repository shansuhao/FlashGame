#pragma once
#include "D3D/D3DLibary.h"
class StaticMeshComponent;
class SceneNode
{
public:
	bool m_bNeedUpdate;
	DirectX::XMVECTOR m_Position;

	StaticMeshComponent* m_StaticMeshComponent;

	SceneNode();
	void Update(float inX, float inY, float inZ);
	void Render();
};

