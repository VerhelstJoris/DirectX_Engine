#pragma once

#include <string>
#include "LSystemHelpers.h"

class LSystemRule
{
public:
	LSystemRule(char symbol, std::string replacement, std::string leftContext = "", std::string rightContext = "", float probability = 1.0f);
	~LSystemRule();


	char GetSymbol() { return m_Symbol; };
	std::string GetReplacement() { return m_Replacement; };
	ContextData GetLeftContext() { return ContextData{ m_UsingLeftContext, m_LeftContext , m_LeftContextLength}; };
	ContextData GetRightContext() { return ContextData{ m_UsingRightContext, m_RightContext , m_RightContextLength }; };
	float GetProbability() { return m_Probability; };

	void SetLeftContext(std::string text);
	void SetRightContext(std::string text);
	void SetSymbol(char symbol) { m_Symbol = symbol; };
	void SetReplacement(std::string replacementString) { m_Replacement = replacementString; };
	void SetProbability(float probability);

private:
	
	std::string m_LeftContext;
	std::string m_RightContext;
	char m_Symbol;

	std::string m_Replacement;

	float m_Probability;

	int m_LeftContextLength;
	int m_RightContextLength;
	bool m_UsingLeftContext;
	bool m_UsingRightContext;

};

