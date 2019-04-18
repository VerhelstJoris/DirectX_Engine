#pragma once
#include <vector>
#include <string>

#include "LSystemHelpers.h"
#include "LSystemRule.h"
#include "Line.h"
#include "d3dclass.h"


class FractalTree
{
public:
	FractalTree(ID3D11Device* device, float3 startPosition);
	~FractalTree();

	void Generate();
	void Turtle();

	void ShutDown();


	std::vector<Line*> getLines() { return m_Lines; };

private:
	void GenerateLines(std::string string, float3 startingPoint, float branchLength, float angle);
private:
	//variables
	ID3D11Device* m_Device;
	std::vector<Line*> m_Lines;



	//axiom
	std::string m_Axiom = "F";
	std::string m_CurrentSentence;

	//rules
	std::vector<LSystemRule*> m_Rules;

	//
	float3 m_StartPosition;
	float m_BranchLength = 1;
	float m_BranchAngle = 45;

	
};

