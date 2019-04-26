#pragma once
#include <vector>
#include <string>
#include <directxmath.h>
#include <d3d11.h>
#include <stack>

#include "LSystemHelpers.h"
#include "LSystemRule.h"
#include "SimpleObject.h"
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
		int nrInBranch = 0;
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
	bool Initialize();

	int GetIndexCount() { return m_Indices.size(); };
	XMMATRIX GetWorldMatrix() { return m_WorldMatrix; };

	std::vector<SimpleObject*> GetModels() { return m_Models; };
	ID3D11ShaderResourceView* GetDiffuseTexture() { return m_DiffuseTexture->GetTexture(); };
	ID3D11ShaderResourceView* GetNormalTexture() { return m_NormalTexture->GetTexture(); };


	void SetWorldMatrix(XMMATRIX world) { m_WorldMatrix = world; };
	void SetDiffuseTexture(TextureClass* texture) { m_DiffuseTexture = texture; };
	void SetNormalTexture(TextureClass* texture) { m_NormalTexture = texture; };

	void SetStartPosition(XMVECTOR start) { m_StartPosition = start; };
	void SetBranchAngleRandomAmount(float angleInDegrees) {m_AngleRandomAmount = (angleInDegrees * XM_PI) / 180;};
	void SetBranchAngle(float angleInDegrees) { m_BranchAngle = (angleInDegrees * XM_PI) / 180; };
	void SetBranchLength(float length) { m_BranchLength = length; };
	void SetBranchStartRadius(float radius) { m_BranchStartRadius = radius; };

private:
	void ResetModels();
	void InterpretSystem(std::string lResult, XMVECTOR startingPoint, float stepSize, float angleDelta, float branchRadius);
private:
	//variables
	ID3D11Device* m_Device;
	ID3D11Buffer* m_VertexBuffer, *m_IndexBuffer;
	TextureClass* m_DiffuseTexture , *m_NormalTexture;

	std::vector<SimpleObject*> m_Models;

	//axiom
	std::string m_Axiom = "X";
	std::string m_CurrentSentence;

	//rules
	std::vector<LSystemRule*> m_Rules;

	//tree visual variables
	float m_BranchStartRadius = 0.5f;
	float m_BranchLength = 4.0f;
	float m_BranchAngle = (30.0f * XM_PI) / 180;
	float m_AngleRandomAmount = (25.0f * XM_PI) / 180;

	XMVECTOR m_StartPosition;

	std::vector<VertexType> m_Vertices;	//vector filled with all the vertices
	std::vector<int> m_Indices;			//order of all indices in order of to be rendered
	
	XMMATRIX m_WorldMatrix;
};

