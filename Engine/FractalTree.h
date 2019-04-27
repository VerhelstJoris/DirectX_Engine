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

	struct RuleSet
	{
		std::string axiom;
		std::vector<LSystemRule*> rules;
	};

public:
	FractalTree(ID3D11Device* device, XMVECTOR startPosition);
	~FractalTree();

	void Generate();
	void ShutDown();
	bool Initialize();

	//get
	int GetIndexCount() { return m_Indices.size(); };
	XMMATRIX GetWorldMatrix() { return m_WorldMatrix; };

	std::vector<SimpleObject*> GetBranchModels() { return m_BranchModels; };
	std::vector<SimpleObject*> GetLeafModels() { return m_LeafModels; };

	ID3D11ShaderResourceView* GetBranchDiffuseTexture() { return m_DiffuseTexture->GetTexture(); };
	ID3D11ShaderResourceView* GetBranchNormalTexture() { return m_NormalTexture->GetTexture(); };

	ID3D11ShaderResourceView* GetLeafDiffuseTexture() { return m_LeafDiffuseTexture->GetTexture(); };
	ID3D11ShaderResourceView* GetLeafNormalTexture() { return m_LeafNormalTexture->GetTexture(); };

	float GetBranchLength() { return m_BranchLength; };

	//set
	void SetWorldMatrix(XMMATRIX world) { m_WorldMatrix = world; };
	void SetBranchDiffuseTexture(TextureClass* texture) { m_DiffuseTexture = texture; };
	void SetBranchNormalTexture(TextureClass* texture) { m_NormalTexture = texture; };

	void SetLeafDiffuseTexture(TextureClass* texture) { m_LeafDiffuseTexture = texture; };
	void SetLeafNormalTexture(TextureClass* texture) { m_LeafNormalTexture = texture; };

	void SetStartPosition(XMVECTOR start) { m_StartPosition = start; };
	void SetBranchAngleRandomAmount(float angleInDegrees) {m_AngleRandomAmount = (angleInDegrees * XM_PI) / 180;};
	void SetBranchAngle(float angleInDegrees) { m_BranchAngle = (angleInDegrees * XM_PI) / 180; };
	void SetBranchLength(float length) { m_BranchLength = length; };
	void SetBranchStartRadius(float radius) { m_BranchStartRadius = radius; };

	void NextRuleSet();

private:
	void ResetModels();
	void InterpretSystem(std::string lResult, XMVECTOR startingPoint, float stepSize, float angleDelta, float branchRadius);
private:
	//variables
	ID3D11Device* m_Device;
	ID3D11Buffer* m_VertexBuffer, *m_IndexBuffer;
	TextureClass* m_DiffuseTexture , *m_NormalTexture, *m_LeafDiffuseTexture, *m_LeafNormalTexture;

	std::vector<SimpleObject*> m_BranchModels;
	std::vector<SimpleObject*> m_LeafModels;
	char m_PrevSymbol = '0';
	bool m_PrevRuleUsed = false;

	//axiom
	std::string m_Axiom = "X";
	std::string m_CurrentSentence;

	//rules
	std::vector<RuleSet*> m_RuleSets;

	int m_CurrentRuleSet=0;

	//tree visual variables
	float m_BranchStartRadius = 0.5f;
	float m_BranchLength = 4.0f;
	float m_BranchAngle = (25.0f * XM_PI) / 180;
	float m_AngleRandomAmount = (15.0f * XM_PI) / 180;

	XMVECTOR m_StartPosition;

	std::vector<VertexType> m_Vertices;	//vector filled with all the vertices
	std::vector<int> m_Indices;			//order of all indices in order of to be rendered
	
	XMMATRIX m_WorldMatrix;
};

