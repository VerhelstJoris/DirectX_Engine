#pragma once
#include <vector>
#include <string>
#include <directxmath.h>
#include <d3d11.h>
#include <stack>

#include "LSystemHelpers.h"
#include "LSystemRule.h"
#include "Line.h"
#include "d3dclass.h"

using namespace DirectX;


class FractalTree
{
private:
	struct TurtleState
	{
		XMVECTOR pos;
		XMVECTOR dir = XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);
		float stepSize;
		float radius;
		XMMATRIX rotation;
	};

	struct VertexType
	{
		VertexType() {};
		VertexType(XMFLOAT3 position) : pos(position) {};
		XMFLOAT3 pos;
		XMFLOAT2 tex;
		XMFLOAT3 normal;
	};

public:
	FractalTree(ID3D11Device* device, XMVECTOR startPosition);
	~FractalTree();

	void Generate();
	void ShutDown();
	bool Render(ID3D11DeviceContext* context);
	bool Initialize();

	int GetIndexCount() { return m_Indices.size(); };

private:
	void InterpretSystem(std::string lResult, XMVECTOR startingPoint, float stepSize, float angleDelta);

private:
	//variables
	ID3D11Device* m_Device;
	ID3D11Buffer* m_VertexBuffer, *m_IndexBuffer;
	ID3D11ShaderResourceView* m_Texture;

	//axiom
	std::string m_Axiom = "X";
	std::string m_CurrentSentence;

	//rules
	std::vector<LSystemRule*> m_Rules;

	XMVECTOR m_StartPosition;

	std::vector<VertexType> m_Vertices;	//vector filled with all the vertices
	std::vector<int> m_Indices;			//order of all indices in order of to be rendered
	
	XMMATRIX m_WorldMatrix;
	bool m_UseModel;
};

