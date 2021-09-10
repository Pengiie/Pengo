#pragma once

#include "ast/Value.h"
#include <string>
#include <unordered_map>
#include <iostream>

struct Environment
{
	std::unordered_map<std::string, Value> m_variables;

	bool hasVariable(const std::string& name) 
	{
		return m_variables.find(name) != m_variables.end();
	}

	Value& getVariable(const Token& token) 
	{
		if (hasVariable(token.token))
		{
			return m_variables.at(token.token);
		}
		else
		{
			std::cout << "Error at (" << token.line << ":" << token.pos << "):" << std::endl;
			std::cout << "Variable " << token.token << " not defined" << std::endl;
			exit(-2);
		}
	}
};