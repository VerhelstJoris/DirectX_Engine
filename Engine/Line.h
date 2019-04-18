#pragma once


#include <vector>
#include <d3d11.h>
#include <d3dx10math.h>
#include <DirectXMath.h>

#include "Structs.h"

class Line
{
private:
	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR3 color;
	};


public:
	Line(ID3D11Device* device, std::vector<float3> points , float3 color);
	Line(const Line&);
	~Line();

	bool Initialize(ID3D11Device*);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();

private:

	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();


private:

	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	int m_vertexCount, m_indexCount;

	D3DXMATRIX m_worldMatrix, m_projectionMatrix;


	std::vector<float3> m_Points;
	float3 m_Color;
};

