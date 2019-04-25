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
	m_UseModel = false;
	m_WorldMatrix = XMMatrixIdentity();
	m_Texture = 0;
}

bool FractalTree::Initialize()
{
	if (m_UseModel)
	{
		//for (Model* m : m_models)
		//{
		//	m->SetWorldMatrix(m->GetWorldMatrix() * m_WorldMatrix);
		//	m->SetTexture(m_texture);
		//	m->InitializeBuffers(m_Device);
		//}
	}
	else
	{
		D3D11_BUFFER_DESC vBufferDesc;
		D3D11_BUFFER_DESC iBufferDesc;

		D3D11_SUBRESOURCE_DATA vData;
		D3D11_SUBRESOURCE_DATA iData;

		HRESULT result;

		vBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vBufferDesc.ByteWidth = sizeof(VertexType) * m_Vertices.size();
		vBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vBufferDesc.CPUAccessFlags = 0;
		vBufferDesc.MiscFlags = 0;
		vBufferDesc.StructureByteStride = 0;

		vData.pSysMem = m_Vertices.data();
		vData.SysMemPitch = 0;
		vData.SysMemSlicePitch = 0;

		result = m_Device->CreateBuffer(&vBufferDesc, &vData, &m_VertexBuffer);
		if (FAILED(result))
			return false;

		iBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		iBufferDesc.ByteWidth = sizeof(int) * m_Indices.size();
		iBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		iBufferDesc.CPUAccessFlags = 0;
		iBufferDesc.MiscFlags = 0;
		iBufferDesc.StructureByteStride = 0;

		iData.pSysMem = m_Indices.data();
		iData.SysMemPitch = 0;
		iData.SysMemSlicePitch = 0;

		result = m_Device->CreateBuffer(&iBufferDesc, &iData, &m_IndexBuffer);
		if (FAILED(result))
			return false;
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
		m_Texture->Release();
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
	InterpretSystem(m_CurrentSentence,m_StartPosition, 0.75f, (25.7f * XM_PI) / 180);
}

void FractalTree::InterpretSystem(std::string lResult, XMVECTOR startingPoint, float stepSize, float angleDelta)
{
	TurtleState curState, nextState;
	curState.pos = startingPoint;
	curState.rotation = XMMatrixIdentity();
	curState.stepSize = stepSize;
	curState.radius = 1.0f;

	std::stack<int> indexStack;				//order of indexes to be rendered
	std::stack<TurtleState> turtleStack;	//all points 

	XMMATRIX rotMatrix = XMMatrixIdentity();
	int index = 0;

	float origRad = curState.radius;		//radius branch
	float origStepSize = curState.stepSize;	//length branch


	for (char c : lResult)
	{
		nextState = curState;
		XMVECTOR rotated = XMVector3Transform(curState.dir, rotMatrix);

		switch (c)
		{
		case 'F':
		{
			nextState.pos = XMVectorAdd(nextState.pos, XMVectorScale(rotated, curState.stepSize));

			
			XMFLOAT3 vertPos;
			XMStoreFloat3(&vertPos, curState.pos);
			m_Vertices.push_back(vertPos);

			XMStoreFloat3(&vertPos, nextState.pos);
			m_Vertices.push_back(vertPos);

			m_Indices.push_back(index);
			index = m_Indices.size(); //can't add 1, what if we branched
			m_Indices.push_back(index);
			
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
	if (m_UseModel)
	{
		//for (Model* m : m_models)
		//{
		//	m->Render(context);
		//}
	}
	else
	{
		unsigned int stride;
		unsigned int offset;

		// Set vertex buffer stride and offset.
		stride = sizeof(VertexType);
		offset = 0;

		// Set the vertex buffer to active in the input assembler so it can be rendered.
		context->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);

		// Set the index buffer to active in the input assembler so it can be rendered.
		context->IASetIndexBuffer(m_IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

		// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	}
	return true;
}

