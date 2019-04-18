#include "LSystemRule.h"



LSystemRule::LSystemRule(char symbol, std::string replacement, std::string leftContext, std::string rightContext, float probability):
	m_Symbol(symbol), m_Replacement(replacement)
{
	SetLeftContext(leftContext);
	SetRightContext(rightContext);
	SetProbability(probability);
}


LSystemRule::~LSystemRule()
{
}


void LSystemRule::SetLeftContext(std::string text)
{
	m_LeftContext = text;

	m_LeftContextLength = text.length();
	if (m_LeftContextLength == 0)
		m_UsingLeftContext = false;
	else
		m_UsingLeftContext = true;
}

void LSystemRule::SetRightContext(std::string text)
{
	m_RightContext = text;

	m_RightContextLength = text.length();
	if (m_RightContextLength == 0)
		m_UsingRightContext = false;
	else
		m_UsingRightContext = true;
}

void LSystemRule::SetProbability(float probability)
{
	//clamp between 0-1
	m_Probability = probability;
}
