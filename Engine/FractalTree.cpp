#include "FractalTree.h"

#include "Structs.h"

#include <iostream>



FractalTree::FractalTree(ID3D11Device* device, XMVECTOR startPosition)
{
	m_Device = device;
	m_StartPosition = startPosition;

	//L_SYSTEM
	//-------------------------------------------------
	m_CurrentSentence = m_Axiom;

	//m_Rules.push_back(new LSystemRule( 'F', std::string("F[--F][+F][F - F - F][F++F - F[F + F]]") , std::string(""),std::string("") , 0.8f));
	m_Rules.push_back(new LSystemRule('X', "F[+X][-X][^X][&X]FX"));
	m_Rules.push_back(new LSystemRule('F', "FF"));

	//
	m_WorldMatrix = XMMatrixIdentity();
	m_Texture = 0;
}

bool FractalTree::Initialize()
{
	
	for (size_t i = 0; i < m_Models.size(); i++)
	{
		m_Models[i]->SetWorldMatrix(m_Models[i]->GetWorldMatrix() * m_WorldMatrix);
		m_Models[i]->SetTexture(m_Texture);
		m_Models[i]->InitializeBuffers(m_Device);
	}
	
	return true;
}

FractalTree::~FractalTree()
{
}

void FractalTree::ShutDown()
{
	if (m_Texture)
	{
		m_Texture->Shutdown();
		m_Texture = 0;
	}
}

void FractalTree::Generate()
{
	//TO-DO:
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

				//right context
				if (rightContextData.useContext && i >= rightContextData.contextLength)
				{

					if (m_CurrentSentence.substr(i, rightContextData.contextLength) != rightContextData.context)
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
	InterpretSystem(m_CurrentSentence,m_StartPosition, 4.0f, (25.7f * XM_PI) / 180);
}

void FractalTree::InterpretSystem(std::string sentence, XMVECTOR startingPoint, float stepSize, float angleDelta)
{
	TurtleState curState, nextState;
	//curState.pos = startingPoint;
	curState.pos = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	curState.rotation = XMMatrixIdentity();
	curState.stepSize = stepSize;
	curState.radius = 1.0f;

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

		switch (c)
		{
		case 'F':
		{
			nextState.pos = XMVectorAdd(nextState.pos, XMVectorScale(rotated, curState.stepSize));

			
			m_Models.push_back(new SimpleObject(cyl));
			m_Models.back()->SetWorldMatrix(XMMatrixMultiply(XMMatrixMultiply(XMMatrixScaling(curState.radius / origRad, curState.stepSize / origStepSize, curState.radius / origRad), rotMatrix), XMMatrixTranslationFromVector(curState.pos)));
			m_Models.back()->SetPosition(curState.pos);
			
			break;
		}
		case 'f':
		{
			nextState.pos += curState.stepSize * rotated;
			index = m_Indices.size(); //need to increase the index, in case we  go back or just so we don't draw where we shouldn't 
			break;
		}
		case '+':
		{
			rotMatrix *= XMMatrixRotationAxis(rotMatrix.r[2], angleDelta);
			
			break;
		}
		case '-':
		{
			rotMatrix *= XMMatrixRotationAxis(rotMatrix.r[2], -angleDelta);
			break;
		}
		case '&':
		{
			rotMatrix *= XMMatrixRotationAxis(rotMatrix.r[0], angleDelta);
			break;
		}
		case '^':
		{
			rotMatrix *= XMMatrixRotationAxis(rotMatrix.r[0], -angleDelta);
			break;
		}
		case '\\':
		{
			rotMatrix *= XMMatrixRotationAxis(rotMatrix.r[1], angleDelta);
			break;
		}
		case '/':
		{
			rotMatrix *= XMMatrixRotationAxis(rotMatrix.r[1], -angleDelta);
			break;
		}
		case '|':
		{
			rotMatrix *= XMMatrixRotationAxis(rotMatrix.r[2], (180.0f * XM_PI) / 180);
			break;
		}
		case '[':
		{
			//nextState.stepSize = curState.stepSize + 1.0f;
			nextState.radius = curState.radius - 0.3f;
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

bool FractalTree::Render(ID3D11DeviceContext* context)
{
	for (size_t i = 0; i < m_Models.size(); i++)
	{
		m_Models[i]->Render(context);
	}
	
	return true;
}

