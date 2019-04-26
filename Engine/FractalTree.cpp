#include "FractalTree.h"

#include "Structs.h"

#include <iostream>



FractalTree::FractalTree(ID3D11Device* device, XMVECTOR startPosition)
{
	m_Device = device;
	m_StartPosition = startPosition;

	//L_SYSTEM
	//-------------------------------------------------
	m_Axiom = "X";
	m_CurrentSentence = m_Axiom;

	m_Rules.push_back(new LSystemRule('X', "F[+X][-X][^X][&X]F","","",1.0f));
	m_Rules.push_back(new LSystemRule('X', "F[-X][^X]F","","",0.6f));
	m_Rules.push_back(new LSystemRule('F', "FF","0","",1.0f));

	//
	m_WorldMatrix = XMMatrixIdentity();
	m_DiffuseTexture = 0;
}

bool FractalTree::Initialize()
{
	
	for (size_t i = 0; i < m_Models.size(); i++)
	{
		m_Models[i]->SetWorldMatrix(m_Models[i]->GetWorldMatrix() * m_WorldMatrix);
		//m_Models[i]->SetTexture(m_DiffuseTexture);
		m_Models[i]->InitializeBuffers(m_Device);
	}
	
	return true;
}

FractalTree::~FractalTree()
{
}

void FractalTree::ShutDown()
{
	if (m_DiffuseTexture)
	{
		m_DiffuseTexture->Shutdown();
		m_DiffuseTexture = 0;
	}

	if (m_NormalTexture)
	{
		m_NormalTexture->Shutdown();
		m_NormalTexture = 0;
	}

	for (size_t i = 0; i < m_Models.size(); i++)
	{
		delete m_Models[i];
		m_Models[i] = nullptr;
	}
}

void FractalTree::Generate()
{
	ResetModels();

	std::string nextSentence = "";
	
	for (size_t i = 0; i < m_CurrentSentence.length(); i++)
	{
		char current = m_CurrentSentence.at(i);

		bool found = false;

		for (size_t j = 0; j < m_Rules.size(); j++)
		{
			if (current == m_Rules[j]->GetSymbol())
			{
				bool contextCorrect = true;

				ContextData leftContextData = m_Rules[j]->GetLeftContext();
				ContextData rightContextData = m_Rules[j]->GetRightContext();

				//left context
				if (leftContextData.useContext && i >= leftContextData.contextLength)
				{
					if (m_CurrentSentence.substr(i - leftContextData.contextLength, leftContextData.contextLength) != leftContextData.context)
						contextCorrect = false;
				}
				else if (leftContextData.useContext && leftContextData.context == "0" && i == 0)
				{
					// context of 0 means it has to be the first character of the sentence
					contextCorrect = false;

				}

				//right context
				if (rightContextData.useContext && i >= rightContextData.contextLength)
				{

					if (m_CurrentSentence.substr(i, rightContextData.contextLength) != rightContextData.context)
						contextCorrect = false;
				}
				else if (rightContextData.useContext && rightContextData.context == "0" && i != m_CurrentSentence.length()-1)
				{
					// context of 0 means it has to be the first character of the sentence
					contextCorrect = false;

				}

				if (contextCorrect)
				{
					float random = (float)rand() / (float)(RAND_MAX / 1.0f);

					if (random <= m_Rules[j]->GetProbability())
					{

						nextSentence.append(m_Rules[j]->GetReplacement());
						found = true;
					}
				
				}
			
			}
		}

		if (!found)
		{
			nextSentence.append(std::string(1,current));
		}
	}

	m_CurrentSentence = nextSentence;
   	std::cout << "new sentence: " + m_CurrentSentence << std::endl;

	//GenerateLines(m_CurrentSentence , m_StartPosition,  m_BranchLength,m_BranchAngle);
	InterpretSystem(m_CurrentSentence,m_StartPosition, m_BranchLength, m_BranchAngle, m_BranchStartRadius);
}

void FractalTree::InterpretSystem(std::string sentence, XMVECTOR startingPoint, float stepSize, float angleDelta, float branchRadius)
{
	TurtleState curState, nextState;
	curState.pos = startingPoint;
	//curState.pos = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	curState.rotation = XMMatrixIdentity();
	curState.stepSize = stepSize;
	curState.radius = branchRadius;
	curState.nrInBranch = 0;

	std::stack<int> indexStack;				//order of indexes to be rendered
	std::stack<TurtleState> turtleStack;	//all points 

	XMMATRIX rotMatrix = XMMatrixIdentity();
	int index = 0;

	float origRad = curState.radius;		//radius branch
	float origStepSize = curState.stepSize;	//length branch

	CylinderShape cyl;
	cyl.GenCylinder(origRad, origStepSize, 24);
	
	for (char c : sentence)
	{
		nextState = curState;
		XMVECTOR rotated = XMVector3Transform(curState.dir, rotMatrix);

		float randomValue = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (m_AngleRandomAmount * 2)));
		randomValue -= m_AngleRandomAmount;

		switch (c)
		{
		case 'F':
		{
			//NEW POINT
			nextState.pos = XMVectorAdd(nextState.pos, XMVectorScale(rotated, curState.stepSize));
			nextState.nrInBranch++;
			
			m_Models.push_back(new SimpleObject(cyl));
			float amount = pow(0.85f, curState.nrInBranch);
			m_Models.back()->SetWorldMatrix(XMMatrixMultiply(XMMatrixMultiply(XMMatrixScaling((curState.radius * amount) / origRad, curState.stepSize / origStepSize, (curState.radius * amount) / origRad),
				rotMatrix), XMMatrixTranslationFromVector(curState.pos)));
			
			break;
		}
		case '+':
		{
			
			rotMatrix *= XMMatrixRotationAxis(rotMatrix.r[2], angleDelta + randomValue);
			
			break;
		}
		case '-':
		{
			rotMatrix *= XMMatrixRotationAxis(rotMatrix.r[2], -angleDelta + randomValue);
			break;
		}
		case '&':
		{
			rotMatrix *= XMMatrixRotationAxis(rotMatrix.r[0], angleDelta + randomValue);
			break;
		}
		case '^':
		{
			rotMatrix *= XMMatrixRotationAxis(rotMatrix.r[0], -angleDelta + randomValue);
			break;
		}
		case '\\':
		{
			rotMatrix *= XMMatrixRotationAxis(rotMatrix.r[1], angleDelta + randomValue);
			break;
		}
		case '/':
		{
			rotMatrix *= XMMatrixRotationAxis(rotMatrix.r[1], -angleDelta + randomValue);
			break;
		}
		case '|':
		{
			rotMatrix *= XMMatrixRotationAxis(rotMatrix.r[2], (180.0f * XM_PI) / 180);
			break;
		}
		case '[':
		{
			nextState.stepSize = curState.stepSize/4*2;
			nextState.radius = curState.radius/4*3;
			nextState.nrInBranch = curState.nrInBranch;
			curState.rotation = rotMatrix;

			//add new point and index
			turtleStack.push(curState); 
			indexStack.push(index);
			break;
		}
		case ']':
		{
			nextState = turtleStack.top();
			rotMatrix = nextState.rotation;

			//remove last inserted point and index
			turtleStack.pop();
			index = indexStack.top();
			indexStack.pop();
			break;
		}
		default:
			break;
		}

		curState = nextState;
	}
}

void FractalTree::ResetModels()
{

	for (size_t i = 0; i < m_Models.size(); i++)
	{
		delete m_Models[i];
		m_Models[i] = nullptr;
	}
	m_Models.clear();

	m_Indices.clear();
}


