#pragma once

#include "Function.h"
#include "Interpreter.h"

#include "ast/Statement.h"

#include <type_traits>

Value UserFunction::call(Interpreter& interpreter, std::vector<Value> values)
{
	if (values.size() != args)
	{
		std::cout << "Argument count does not match parameter count when calling function!" << std::endl;
		exit(-2);
	}
	Environment env;
	env.type = EnvironmentType::Function;
	for (int i = 0; i < args; i++)
		env.m_variables[params[i]] = values[i];
	interpreter.getEnvStack().push(env);
	Environment& envRef = interpreter.getEnvStack().top();
	for (std::unique_ptr<Statement>& s : ((BlockStatement*)body.get())->statements)
	{
		s->accept((Statement::Visitor&)interpreter);
		if (envRef.stop)
		{
			Value returnVal = envRef.returnVal;
			interpreter.getEnvStack().pop();
			return returnVal;
		}
	}
	interpreter.getEnvStack().pop();
	return {};
}

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
	Value val = values[0];
	if(val.type == ValueType::String)
		return { ValueType::Integer, std::stoi(std::get<std::string>(val.val)) };
	if (val.type == ValueType::Float)
		return { ValueType::Integer, static_cast<int>(std::get<float>(val.val)) };
}

Value ToFloat::call(Interpreter& interpreter, std::vector<Value> values)
{
	Value val = values[0];
	if (val.type == ValueType::String)
		return { ValueType::Float, std::stof(std::get<std::string>(val.val)) };
	if (val.type == ValueType::Integer)
		return { ValueType::Float, static_cast<float>(std::get<int>(val.val)) };
}

Value Random::call(Interpreter& interpreter, std::vector<Value> values)
{
	std::uniform_int_distribution<int> dist(std::get<int>(values[0].val), std::get<int>(values[1].val));
	return { ValueType::Integer, dist(interpreter.getRandom()) };
}