#pragma once

#include <string>
#include <variant>

#include "../Function.h"

#include <memory>

enum class ValueType
{
	Integer,
	Float,
	String,
	Bool,
	Function,
	Null
};

#pragma warning( push )
#pragma warning( disable : 26495 )
struct Value
{
	ValueType type = ValueType::Null;
	std::variant<int, float, std::string, bool,
		std::shared_ptr<Function>> val;
};
#pragma warning( pop )