#pragma once

#include <string>
#include <variant>

#include "../Function.h"

#include <memory>

enum class ValueType
{
	Integer,
	String,
	Function,
	Null
};

#pragma warning( push )
#pragma warning( disable : 26495 )
struct Value
{
	ValueType type = ValueType::Null;
	std::variant<int, std::string, 
		Function*> val;
};
#pragma warning( pop )