#pragma once

#include "Function.h"
#include "Interpreter.h"

#include <type_traits>

Value Print::call(Interpreter& interpreter, std::vector<Value> values)
{
	std::cout << std::get<std::string>(interpreter.toString(values[0]).val);
	return {};
}

Value PrintLn::call(Interpreter& interpreter, std::vector<Value> values)
{
	std::cout << std::get<std::string>(interpreter.toString(values[0]).val) << std::endl;;
	return {};
}

Value Input::call(Interpreter& interpreter, std::vector<Value> values)
{
	std::string in;
	std::cin >> in;
	return { ValueType::String, in };
}

Value ToInt::call(Interpreter& interpreter, std::vector<Value> values)
{
	return { ValueType::Integer, std::stoi(std::get<std::string>(values[0].val)) };
}

Value Random::call(Interpreter& interpreter, std::vector<Value> values)
{
	std::uniform_int_distribution<int> dist(std::get<int>(values[0].val), std::get<int>(values[1].val));
	return { ValueType::Integer, dist(interpreter.getRandom()) };
}