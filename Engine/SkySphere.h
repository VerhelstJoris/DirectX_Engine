#pragma once

#include "d3dclass.h"
#include "Structs.h"

class SkySphere
{
public:
	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
	};

	SkySphere(ModelInfo*, D3DXVECTOR4 , D3DXVECTOR4);
	~SkySphere();

	bool Initialize(ID3D11Device*);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();
	D3DXMATRIX GetWorldMatrix() { return m_WorldMatrix; };
	void SetPosition(float x, float y, float z);

	D3DXVECTOR4 GetApexColor() { return m_ApexColor; };
	D3DXVECTOR4 GetCenterColor() { return m_CenterColor; };


private:
	bool InitializeBuffers(ID3D11Device*);
	void ReleaseBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

private:
	ModelInfo* m_ModelInfo;
	ID3D11Buffer *m_VertexBuffer, *m_IndexBuffer;

	D3DXVECTOR4 m_ApexColor, m_CenterColor;

	D3DXMATRIX m_WorldMatrix;
	float m_positionX, m_positionY, m_positionZ;

};

