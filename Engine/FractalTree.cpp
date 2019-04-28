#include "FractalTree.h"

#include "Structs.h"

#include <iostream>



FractalTree::FractalTree(ID3D11Device* device, XMVECTOR startPosition)
{
	m_Device = device;
	m_StartPosition = startPosition;

	//L_SYSTEM
	//-------------------------------------------------

	//RULESET 1
	RuleSet* rules1 = new RuleSet();
	rules1->axiom = "X";
	rules1->rules.push_back(new LSystemRule('X', "[&FFFLXL]///[&FLFFLXL]///[L&FFLFXL]", "", "", 1.0f));
	m_RuleSets.push_back(rules1);

	//RULESET 2
	RuleSet* rules2 = new RuleSet();
	rules2->axiom = "X";
	rules2->rules.push_back(new LSystemRule('X', "F[+X][&-X]F[^X][&X]F", "", "", 1.0f));
	rules2->rules.push_back(new LSystemRule('&', "FL[&+XL|FL][^-XL\FL][^FXL]F", "", "", 0.8f));
	rules2->rules.push_back(new LSystemRule('F', "FF", "0", "", 0.5f));
	m_RuleSets.push_back(rules2);


	//RULESET 3
	RuleSet* rules3 = new RuleSet();
	rules3->axiom = "X";
	rules3->rules.push_back(new LSystemRule('X', "F[+X][^X][/F]", "", "", 1.0f));
	rules3->rules.push_back(new LSystemRule('F', "F[&+XL+LF][^L-X&FL][^LFLX]F", "/", "", 0.8f));
	m_RuleSets.push_back(rules3);


	m_CurrentSentence = m_RuleSets[0]->axiom;

	m_WorldMatrix = XMMatrixIdentity();
	m_DiffuseTexture = 0;
	m_NormalTexture = 0;
	m_LeafDiffuseTexture = 0;
	m_LeafNormalTexture = 0;
}

bool FractalTree::Initialize()
{
	
	for (size_t i = 0; i < m_BranchModels.size(); i++)
	{
		m_BranchModels[i]->SetWorldMatrix(m_BranchModels[i]->GetWorldMatrix() * m_WorldMatrix);
		m_BranchModels[i]->InitializeBuffers(m_Device);
	}

	for (size_t i = 0; i < m_LeafModels.size(); i++)
	{
		m_LeafModels[i]->SetWorldMatrix(m_LeafModels[i]->GetWorldMatrix() * m_WorldMatrix);
		m_LeafModels[i]->InitializeBuffers(m_Device);
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

	if (m_LeafDiffuseTexture)
	{
		m_LeafDiffuseTexture->Shutdown();
		m_LeafDiffuseTexture = 0;
	}

	if (m_LeafNormalTexture)
	{
		m_LeafNormalTexture->Shutdown();
		m_LeafNormalTexture = 0;
	}

	for (size_t i = 0; i < m_BranchModels.size(); i++)
	{
		delete m_BranchModels[i];
		m_BranchModels[i] = nullptr;
	}

	for (size_t i = 0; i < m_LeafModels.size(); i++)
	{
		delete m_LeafModels[i];
		m_LeafModels[i] = nullptr;
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


		for (size_t j = 0; j < m_RuleSets[m_CurrentRuleSet]->rules.size(); j++)
		{
			char ruleSymbol = m_RuleSets[m_CurrentRuleSet]->rules[j]->GetSymbol();
			if (current == ruleSymbol)
			{
				bool contextCorrect = true;

				ContextData leftContextData = m_RuleSets[m_CurrentRuleSet]->rules[j]->GetLeftContext();
				ContextData rightContextData = m_RuleSets[m_CurrentRuleSet]->rules[j]->GetRightContext();

				//left context
				if (leftContextData.useContext && i >= leftContextData.contextLength)
				{
					if (m_CurrentSentence.substr(i - leftContextData.contextLength, leftContextData.contextLength) != leftContextData.context)
						contextCorrect = false;
				}
				else if (leftContextData.useContext && leftContextData.context == "0" && i != 0)
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

					if (!(m_PrevRuleUsed && m_PrevSymbol == ruleSymbol))	//stochaistic
					{
						float random = (float)rand() / (float)(RAND_MAX / 1.0f);

						if (random <= m_RuleSets[m_CurrentRuleSet]->rules[j]->GetProbability() )
						{
							nextSentence.append(m_RuleSets[m_CurrentRuleSet]->rules[j]->GetReplacement());
							found = true;
							m_PrevRuleUsed = true;
						}
						else
						{
							m_PrevRuleUsed = false;
						}
					}
					else
					{
						m_PrevRuleUsed = false;
					}
				
				}

				m_PrevSymbol = ruleSymbol;
			
			}
		}

		if (!found)
		{
			nextSentence.append(std::string(1,current));
		}
	}

	m_CurrentSentence = nextSentence;
	m_AmountOfIterations++;

	InterpretSystem(m_CurrentSentence,m_StartPosition, m_BranchLength, m_BranchAngle, m_BranchStartRadius);
}

void FractalTree::InterpretSystem(std::string sentence, XMVECTOR startingPoint, float stepSize, float angleDelta, float branchRadius)
{
	TurtleState curState, nextState, leafState;
	curState.pos = startingPoint;
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

	//generate the shape and size of the branch
	CylinderShape cyl;
	
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
			
			cyl.GenCylinder(origRad, origStepSize, 24  * (curState.radius/ origRad));
			m_BranchModels.push_back(new SimpleObject(cyl));
			float amount = pow(0.85f, curState.nrInBranch);
			m_BranchModels.back()->SetWorldMatrix(XMMatrixMultiply(XMMatrixMultiply(XMMatrixScaling((curState.radius * amount) / origRad, curState.stepSize / origStepSize, (curState.radius * amount) / origRad),
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

			turtleStack.push(curState); 
			indexStack.push(index);
			break;
		}
		case ']':
		{
			nextState = turtleStack.top();
			rotMatrix = nextState.rotation;

			turtleStack.pop();
			index = indexStack.top();
			indexStack.pop();
			break;
		}
		case 'L':
		{
			//randomly spawn at the right or left
			int randomInt = (rand() % 2);

			leafState = curState;

			XMMATRIX leafRotMatrix = rotMatrix;
			if (randomInt == 0)
			{
				leafRotMatrix *= XMMatrixRotationAxis(leafRotMatrix.r[2], (90.0f * XM_PI) / 180);
			}
			else
			{
				leafRotMatrix *= XMMatrixRotationAxis(leafRotMatrix.r[2], (-90.0f * XM_PI) / 180);
			}

			XMVECTOR leafRotated = XMVector3Transform(leafState.dir, leafRotMatrix);
			leafState.pos = XMVectorAdd(leafState.pos, XMVectorScale(leafRotated, leafState.radius));

			//leafRotMatrix *= XMMatrixRotationAxis(leafRotMatrix.r[1], (90.0f * XM_PI) / 180);
			leafRotMatrix *= XMMatrixRotationAxis(leafRotMatrix.r[0], (90.0f * XM_PI) / 180);

			cyl.GenCylinder(1.5f, 0.25f, 18 * (leafState.radius / origRad));
			m_LeafModels.push_back(new SimpleObject(cyl));
			float amount = pow(0.85f, leafState.nrInBranch);
			m_LeafModels.back()->SetWorldMatrix(XMMatrixMultiply(XMMatrixMultiply(XMMatrixScaling(0.25f, 0.1f, 0.25f),
				leafRotMatrix), XMMatrixTranslationFromVector(leafState.pos)));
		}
		default:
			break;
		}

		curState = nextState;
	}
}

void FractalTree::ResetModels()
{

	for (size_t i = 0; i < m_BranchModels.size(); i++)
	{
		delete m_BranchModels[i];
		m_BranchModels[i] = nullptr;
	}
	m_BranchModels.clear();

	for (size_t i = 0; i < m_LeafModels.size(); i++)
	{
		delete m_LeafModels[i];
		m_LeafModels[i] = nullptr;
	}
	m_LeafModels.clear();

	m_Indices.clear();
}

void FractalTree::NextRuleSet()
{
	m_AmountOfIterations = 0;
	m_BranchLength = m_BranchStartLength;

	m_CurrentRuleSet++;
	if (m_CurrentRuleSet >= m_RuleSets.size())
	{
		m_CurrentRuleSet = 0;
	}

	m_Axiom = m_RuleSets[m_CurrentRuleSet]->axiom;
	m_CurrentSentence = m_Axiom;

	//std::cout << m_CurrentSentence << std::endl;
	m_PrevSymbol = '0';
	m_PrevRuleUsed = false;


	Generate();
	Initialize();
}

void FractalTree::SetRuleSet(int id)
{
	m_AmountOfIterations = 0;
	m_BranchLength = m_BranchStartLength;

	m_CurrentRuleSet=id;
	
	m_Axiom = m_RuleSets[m_CurrentRuleSet]->axiom;
	m_CurrentSentence = m_Axiom;
	m_PrevSymbol = '0';
	m_PrevRuleUsed = false;

	Generate();
	Initialize();
}