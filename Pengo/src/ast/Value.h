#pragma once

#include <string>
#include <variant>

enum class ValueType
{
	Integer,
	String,
	Null
};

#pragma warning( push )
#pragma warning( disable : 26495 )
struct Value
{
	ValueType type = ValueType::Null;
	std::variant<int, std::string> val;
};
#pragma warning( pop )