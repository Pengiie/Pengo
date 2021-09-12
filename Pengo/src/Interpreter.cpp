#include "Interpreter.h"

#include <iostream>
#include <string>
#include <variant>
#include <time.h>

#include "Function.h"

Interpreter::Interpreter(std::vector<std::unique_ptr<Statement>> statements)
{
	m_statements = std::move(statements);

	m_functions[FUN_PRINT] = new Print();
	m_functions[FUN_PRINTLN] = new PrintLn();
	m_functions[FUN_INPUT] = new Input();
	m_functions[FUN_TOINT] = new ToInt();
	m_functions[FUN_RANDOM] = new Random();

	m_random.seed(time(NULL));
}

Interpreter::~Interpreter()
{
	for (auto& func : m_functions)
		delete func.second;
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
	m_envStack.push({});
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
	case TokenType::Minus: return { ValueType::Integer, -std::get<int>(val.val) };
	case TokenType::Bang: return { ValueType::Bool, !std::get<bool>(val.val) };
	}
}

Value Interpreter::visitCall(CallExpression* expression)
{
	Value function = evaluate(expression->callee);
	std::vector<Value> values;
	for (std::unique_ptr<Expression>& exp : expression->args)
		values.push_back(evaluate(exp));
	return std::get<Function*>(function.val)->call(*this, values);
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
			val.type = ValueType::Integer;
			val.val = std::get<int>(left.val) + std::get<int>(right.val);
			return val;
		}
	}
	else
	{
		if (left.type == ValueType::Integer && right.type == ValueType::Integer)
		{
			Value val;
			val.type = ValueType::Integer;
			switch (op.type)
			{
			case TokenType::Minus: val.val = std::get<int>(left.val) - std::get<int>(right.val); break;
			case TokenType::Asterisk: val.val = std::get<int>(left.val) * std::get<int>(right.val); break;
			case TokenType::Slash: val.val = std::get<int>(left.val) / std::get<int>(right.val); break;
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
	case TokenType::Number: return { ValueType::Integer, std::stoi(val.token) };
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
	if (value.type == ValueType::Bool)
		return { ValueType::String, std::get<bool>(value.val) ? std::string("true") : std::string("false") };
}

Value Interpreter::findVariable(const Token& name)
{
	for(int i = m_envStack.size() - 1; i >= 0; i--)
		if (m_envStack.begin()[i].hasVariable(name.token))
			return m_envStack.begin()[i].getVariable(name);

	if(m_globalEnvironment.hasVariable(name.token))
		return m_globalEnvironment.getVariable(name);

	if (m_functions.find(name.token) != m_functions.end())
	{
		Value val;
		val.type = ValueType::Function;
		val.val = m_functions.at(name.token);
		return val;
	}
	return m_globalEnvironment.getVariable(name);
}