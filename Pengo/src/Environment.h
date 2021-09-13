#pragma once

#include "ast/Value.h"
#include <string>
#include <unordered_map>
#include <iostream>

#include <memory>

enum class EnvironmentType
{
	Global,
	Function,
	Loop,
	Generic
};

struct Environment
{
	EnvironmentType type = EnvironmentType::Generic;
	std::unordered_map<std::string, Value> m_variables;
	std::unordered_map<std::string, std::shared_ptr<Function>> m_functions;

	bool stop = false;
	Value returnVal; // Used only for functions, too lazy to create a whole OOP environment its literally 11 pm lmao

	~Environment()
	{
		
	}

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