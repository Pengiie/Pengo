#include "Interpreter.h"

#include <iostream>
#include <string>
#include <variant>
#include <time.h>

#include "Function.h"

Interpreter::Interpreter(std::vector<std::unique_ptr<Statement>> statements)
{
	m_statements = std::move(statements);

	m_globalEnvironment.m_functions[FUN_PRINT] = std::make_shared<Print>(Print());
	m_globalEnvironment.m_functions[FUN_PRINTLN] = std::make_shared<PrintLn>(PrintLn());
	m_globalEnvironment.m_functions[FUN_INPUT] = std::make_shared<Input>(Input());
	m_globalEnvironment.m_functions[FUN_TOINT] = std::make_shared<ToInt>(ToInt());
	m_globalEnvironment.m_functions[FUN_TOFLOAT] = std::make_shared<ToFloat>(ToFloat());
	m_globalEnvironment.m_functions[FUN_RANDOM] = std::make_shared<Random>(Random());

	m_random.seed(time(NULL));
	m_globalEnvironment.type = EnvironmentType::Global;
}

Interpreter::~Interpreter()
{
	
}

void Interpreter::interpret()
{
	for (const std::unique_ptr<Statement>& statement : m_statements)
		statement->accept(*this);
}

Value Interpreter::evaluate(const std::unique_ptr<Expression>& expression)
{
	return expression->accept(*this);
}

void Interpreter::visitReturn(ReturnStatement* statement)
{
	for (int i = m_envStack.size() - 1; i >= 0; i--)
		if (m_envStack.begin()[i].type == EnvironmentType::Function)
		{
			m_envStack.begin()[i].stop = true;
			if (statement->hasExp)
				m_envStack.begin()[i].returnVal = evaluate(statement->exp);
			else
				m_envStack.begin()[i].returnVal = {};
			return;
		}
	exit(0);
}

void Interpreter::visitFuncDeclare(FuncDeclareStatement* statement)
{
	std::vector<std::string> params;
	for (Token& token : statement->params)
		params.push_back(token.token);
	std::shared_ptr<UserFunction> func = std::make_shared<UserFunction>(UserFunction(statement->params.size(), params, statement->body));
	if (m_envStack.size() == 0)
		m_globalEnvironment.m_functions[statement->name.token] = func;
	else
		m_envStack.top().m_functions[statement->name.token] = func;
}

void Interpreter::visitWhile(WhileStatement* statement)
{
	Value val = evaluate(statement->condition);
	if (val.type != ValueType::Bool)
	{
		std::cout << "Expected boolean value for while statement condition" << std::endl;
		exit(-2);
	}
	while (std::get<bool>(val.val))
	{
		statement->body->accept(*this);
		val = evaluate(statement->condition);
	}
}

void Interpreter::visitIf(IfStatement* statement)
{
	Value val = evaluate(statement->condition);
	if (val.type != ValueType::Bool)
	{
		std::cout << "Expected boolean value for if statement condition" << std::endl;
		exit(-2);
	}
	bool isTrue = std::get<bool>(val.val);
	if (isTrue)
		statement->body->accept(*this);
	else if (statement->hasElse)
		statement->elze->accept(*this);
}

void Interpreter::visitBlock(BlockStatement* statement)
{
	m_envStack.push({statement->envType});
	for (std::unique_ptr<Statement>& s : statement->statements)
		s->accept(*this);
	m_envStack.pop();
}

void Interpreter::visitExpression(ExpressionStatement* statement)
{
	evaluate(statement->exp);
}

void Interpreter::visitPrint(PrintStatement* statement)
{
	std::cout << std::get<std::string>(toString(evaluate(statement->exp)).val) << std::endl;
}

void Interpreter::visitVarDeclare(VarDeclareStatement* statement)
{
	Token& name = statement->name;
	for (int i = m_envStack.size() - 1; i >= 0; i--)
		if (m_envStack.begin()[i].hasVariable(name.token))
		{
			m_envStack.begin()[i].m_variables[name.token] = evaluate(statement->exp);
			return;
		}
	if (m_globalEnvironment.hasVariable(name.token))
	{
		m_globalEnvironment.m_variables[name.token] = evaluate(statement->exp);
		return;
	}

	if (m_envStack.empty())
		m_globalEnvironment.m_variables[statement->name.token] = evaluate(statement->exp);
	else
		m_envStack.top().m_variables[statement->name.token] = evaluate(statement->exp);
}

Value Interpreter::visitLogical(LogicalExpression* expression)
{
	Value left = evaluate(expression->left);
	Token op = expression->op;
	Value right = evaluate(expression->right);

	if (left.type != ValueType::Bool || right.type != ValueType::Bool)
	{
		std::cout << "Error at (" << op.line << ":" << op.pos << "):" << std::endl;
		std::cout << "Left and right hand side types must be booleans" << std::endl;
		exit(-2);
	}

	if (op.type == TokenType::AndAnd)
		return { ValueType::Bool, std::get<bool>(left.val) && std::get<bool>(right.val) };
	if (op.type == TokenType::OrOr)
		return { ValueType::Bool, std::get<bool>(left.val) || std::get<bool>(right.val) };
}

Value Interpreter::visitConditional(ConditionalExpression* expression) 
{
	Value left = evaluate(expression->left);
	Token op = expression->op;
	Value right = evaluate(expression->right);

	if (left.type != right.type)
	{
		std::cout << "Error at (" << op.line << ":" << op.pos << "):" << std::endl;
		std::cout << "Left and right hand side types do not match" << std::endl;
		exit(-2);
	}
	switch (op.type)
	{
	case TokenType::EqualEqual:
	{
		switch (left.type)
		{
		case ValueType::Integer: return { ValueType::Bool, std::get<int>(left.val) == std::get<int>(right.val) };
		case ValueType::Float: return { ValueType::Bool, std::get<float>(left.val) == std::get<float>(right.val) };
		case ValueType::Bool: return { ValueType::Bool, std::get<bool>(left.val) == std::get<bool>(right.val) };
		case ValueType::String: return { ValueType::Bool, std::get<std::string>(left.val) == std::get<std::string>(right.val) };
		}
	}
	break;
	case TokenType::NotEqual:
	{
		switch (left.type)
		{
		case ValueType::Integer: return { ValueType::Bool, std::get<int>(left.val) != std::get<int>(right.val) };
		case ValueType::Bool: return { ValueType::Bool, std::get<bool>(left.val) != std::get<bool>(right.val) };
		case ValueType::String: return { ValueType::Bool, std::get<std::string>(left.val) != std::get<std::string>(right.val) };
		}
	}
	break;
	case TokenType::GreaterThan:
	case TokenType::LessThan:
	case TokenType::GreaterThanEqual:
	case TokenType::LessThanEqual:
	{
		if (left.type != ValueType::Integer)
		{
			std::cout << "Error at (" << op.line << ":" << op.pos << "):" << std::endl;
			std::cout << "Left and right hand side types must be integers" << std::endl;
			exit(-2);
		}
		switch (op.type)
		{
		case TokenType::GreaterThan: return { ValueType::Bool, (std::get<int>(left.val) > std::get<int>(right.val)) };
		case TokenType::LessThan: return { ValueType::Bool, (std::get<int>(left.val) < std::get<int>(right.val)) };
		case TokenType::GreaterThanEqual: return { ValueType::Bool, (std::get<int>(left.val) >= std::get<int>(right.val)) };
		case TokenType::LessThanEqual: return { ValueType::Bool, (std::get<int>(left.val) <= std::get<int>(right.val)) };
		}
	}
	break;
	}
}

Value Interpreter::visitUnary(UnaryExpression* expression)
{
	TokenType type = expression->op.type;
	Value val = evaluate(expression->exp);
	switch (type)
	{
	case TokenType::Minus: {
		if(val.type == ValueType::Integer)
			return { ValueType::Integer, -std::get<int>(val.val) };
		if (val.type == ValueType::Float)
			return { ValueType::Float, -std::get<int>(val.val) };
	}
	case TokenType::Bang: return { ValueType::Bool, !std::get<bool>(val.val) };
	}
}

Value Interpreter::visitCall(CallExpression* expression)
{
	Value function = evaluate(expression->callee);
	std::vector<Value> values;
	for (std::unique_ptr<Expression>& exp : expression->args)
		values.push_back(evaluate(exp));
	return std::get<std::shared_ptr<Function>>(function.val)->call(*this, values);
}

Value Interpreter::visitBinary(BinaryExpression* expression)
{
	Value left = evaluate(expression->left);
	Value right = evaluate(expression->right);
	Token& op = expression->op;
	if (op.type == TokenType::Plus)
	{
		if(left.type == ValueType::String || right.type == ValueType::String)
		{
			return { ValueType::String, std::get<std::string>(toString(left).val) + std::get<std::string>(toString(right).val) };
		}
		else {
			Value val;
			bool doLeftFloat = left.type == ValueType::Float, doRightFloat = right.type == ValueType::Float;
			bool floatBoth = doLeftFloat || doRightFloat;
			val.type = floatBoth ? ValueType::Float : ValueType::Integer;
			{
				if (floatBoth)
				{
					float result = (doLeftFloat ? std::get<float>(left.val) : std::get<int>(left.val)) +
						(doRightFloat ? std::get<float>(right.val) : std::get<int>(right.val));
					val.val = result;
				}
				else
				{
					int result = (doLeftFloat ? std::get<float>(left.val) : std::get<int>(left.val)) +
						(doRightFloat ? std::get<float>(right.val) : std::get<int>(right.val));
					val.val = result;
				}
			}
			return val;
		}
	}
	else
	{
		if ((left.type == ValueType::Integer || left.type == ValueType::Float) &&
			(right.type == ValueType::Integer || right.type == ValueType::Float))
		{
			Value val;
			bool doLeftFloat = left.type == ValueType::Float, doRightFloat = right.type == ValueType::Float;
			bool floatBoth = doLeftFloat || doRightFloat;
			val.type = floatBoth ? ValueType::Float : ValueType::Integer;
			switch (op.type)
			{
			case TokenType::Minus: 
			{
				if (floatBoth)
				{
					float result = (doLeftFloat ? std::get<float>(left.val) : std::get<int>(left.val)) -
						(doRightFloat ? std::get<float>(right.val) : std::get<int>(right.val));
					val.val = result;
				}
				else
				{
					int result = (doLeftFloat ? std::get<float>(left.val) : std::get<int>(left.val)) -
						(doRightFloat ? std::get<float>(right.val) : std::get<int>(right.val));
					val.val = result;
				}
			}
			break;
			case TokenType::Asterisk: 
			{
				if (floatBoth)
				{
					float result = (doLeftFloat ? std::get<float>(left.val) : std::get<int>(left.val)) *
						(doRightFloat ? std::get<float>(right.val) : std::get<int>(right.val));
					val.val = result;
				}
				else
				{
					int result = (doLeftFloat ? std::get<float>(left.val) : std::get<int>(left.val)) *
						(doRightFloat ? std::get<float>(right.val) : std::get<int>(right.val));
					val.val = result;
				}
			}
									break;
			case TokenType::Slash: 
			{
				if (floatBoth)
				{
					float result = (doLeftFloat ? std::get<float>(left.val) : std::get<int>(left.val)) /
						(doRightFloat ? std::get<float>(right.val) : std::get<int>(right.val));
					val.val = result;
				}
				else
				{
					int result = (doLeftFloat ? std::get<float>(left.val) : std::get<int>(left.val)) /
						(doRightFloat ? std::get<float>(right.val) : std::get<int>(right.val));
					val.val = result;
				}
			}
			break;
			}
			return val;
		}
		else
		{
			std::cout << "Error at (" << op.line << ":" << op.pos << "):" << std::endl;
			std::cout << "Cannot use operator " << op.token << " here" << std::endl;
			std::cin.get();
		}
	}
}

Value Interpreter::visitLiteral(LiteralExpression* expression)
{
	Token& val = expression->value;
	switch (val.type)
	{
	case TokenType::Integer: return { ValueType::Integer, std::stoi(val.token) };
	case TokenType::Float: return { ValueType::Float, std::stof(val.token) };
	case TokenType::String: return { ValueType::String, val.token };
	case TokenType::Bool: return { ValueType::Bool, val.token == "true" ? true : false };
	}
	return {ValueType::Null };
}

Value Interpreter::visitVar(VarExpression* expression)
{
	Token& name = expression->name;
	return findVariable(name);
}

Value Interpreter::toString(const Value& value)
{
	if (value.type == ValueType::String)
		return value;
	if (value.type == ValueType::Integer)
		return { ValueType::String, std::to_string(std::get<int>(value.val)) };
	if(value.type == ValueType::Float)
		return { ValueType::String, std::to_string(std::get<float>(value.val)) };
	if (value.type == ValueType::Bool)
		return { ValueType::String, std::get<bool>(value.val) ? std::string("true") : std::string("false") };
	if (value.type == ValueType::Null)
		return { ValueType::String, "Null"};
}

Value Interpreter::findVariable(const Token& name)
{
	for (int i = m_envStack.size() - 1; i >= 0; i--)
	{
		auto& env = m_envStack.begin()[i];
		if (env.hasVariable(name.token))
			return env.getVariable(name);
		else if (env.m_functions.find(name.token) != env.m_functions.end())
			return { ValueType::Function, env.m_functions[name.token] };
	}
	if(m_globalEnvironment.hasVariable(name.token))
		return m_globalEnvironment.getVariable(name);
	else if (m_globalEnvironment.m_functions.find(name.token) != m_globalEnvironment.m_functions.end())
		return { ValueType::Function, m_globalEnvironment.m_functions[name.token] };
	return m_globalEnvironment.getVariable(name);
}