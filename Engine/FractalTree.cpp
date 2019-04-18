#include "FractalTree.h"

#include "Structs.h"

#include <iostream>
#include <chrono>



FractalTree::FractalTree(ID3D11Device* device, float3 startPosition)
{
	m_Device = device;

	m_StartPosition = startPosition;

	//L_SYSTEM
	//-------------------------------------------------
	m_CurrentSentence = m_Axiom;

	//m_Rules.push_back(Rule{'F', "FF[F--FF++F]F+++F[FFF-F[FF+F[F--F]F]--F]F++F[FF--F]F++FF"});
	//m_Rules.push_back(Rule{ 'F', "F[--F][+F][F-F-F][F++F-F[F+F]]" });
	m_Rules.push_back(new LSystemRule( 'F', std::string("F[--F][+F][F - F - F][F++F - F[F + F]]") , std::string(""),std::string("") , 0.8f));

	std::cout << "AXIOM: " + m_CurrentSentence << std::endl;

}

FractalTree::~FractalTree()
{
}

void FractalTree::ShutDown()
{
	for (size_t i = 0; i < m_Lines.size(); i++)
	{
		delete m_Lines[i];
		m_Lines[i] = nullptr;
	}
}

void FractalTree::Generate()
{
	//TO-DO:
	//GET ALL RULE DATA BEFORE
	//std::chrono::milliseconds msStart = std::chrono::duration_cast<std::chrono::milliseconds>(
	//	std::chrono::system_clock::now().time_since_epoch());

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

	//std::chrono::milliseconds msEnd = std::chrono::duration_cast<std::chrono::milliseconds>(
	//	std::chrono::system_clock::now().time_since_epoch());
	//
	//std::cout << msEnd.count() - msStart.count() << std::endl;

	for (size_t i = 0; i < m_Lines.size(); i++)
	{
		m_Lines[i]->Shutdown();
		delete m_Lines[i];
		m_Lines[i] = nullptr;
	}

	m_Lines.clear();
	GenerateLines(m_CurrentSentence , m_StartPosition,  m_BranchLength,m_BranchAngle);
}

void FractalTree::Turtle()
{
	for (size_t i = 0; i < m_CurrentSentence.length(); i++)
	{
		char current = m_CurrentSentence.at(i);

		if (current == 'F')
		{
			//draw up

		}
		else if (current == '+')
		{
			//rotate
		}
		else if (current == '-')
		{
			//rotate
		}
		else if (current == '[')
		{
			//open branch
		}
		else if (current == ']')
		{
			//close branch
		}
	}
}

void FractalTree::GenerateLines(std::string text, float3 startingPoint, float branchLength, float angle)
{
	//std::cout << "GENERATING LINES FROM STRING: " << text << std::endl;

	float3 currentPos = startingPoint;

	std::vector<float3> points;
	points.push_back(startingPoint);
	
	for (size_t i = 0; i < text.length(); i++)
	{

		char current = text.at(i);

		if (current == 'F')
		{
			//draw up and add point
			currentPos.y += branchLength;
			points.push_back(currentPos);


		}
		else if (current == '+')
		{
			//rotate
			currentPos.y += branchLength * cos(angle);

			//currentPos.x += branchLength * sin(angle);
			//currentPos.z += branchLength * cos(angle);

			currentPos.x += (float)rand() / (float)(RAND_MAX / branchLength);
			currentPos.z += (float)rand() / (float)(RAND_MAX / branchLength);

			//points.push_back(currentPos);

		}
		else if (current == '-')
		{
			//rotate
			currentPos.y += branchLength * cos(angle);

			//currentPos.x -= branchLength * sin(angle);
			//currentPos.z -= branchLength * cos(angle);

			currentPos.x -= (float)rand() / (float)(RAND_MAX / branchLength);
			currentPos.z -= (float)rand() / (float)(RAND_MAX / branchLength);

			//points.push_back(currentPos);

		}
		else if (current == '[')
		{
			//open branch
			//find the corresponding closing bracket
			int closingPos = FindClosingBracket(text, i); 

			//std::cout << "string substr: " << text.substr(i + 1, closingPos - (i+1) ) << std::endl;
			GenerateLines(text.substr(i + 1, closingPos - (i + 1) ),currentPos, branchLength/2 , angle );

			//skip over to end of Closed Bracket
			i = closingPos-1;
		}
		
	}
	
	//std::cout << "Amount of points in this line: " << points.size() << " FROM STRING: " << string<< std::endl;
	m_Lines.push_back(new Line(m_Device, points, float3{ 1.0f, 1.0f,1.0f }));
}

