#include "Interpreter.h"

#include <iostream>
#include <string>
#include <variant>

Interpreter::Interpreter(std::vector<std::unique_ptr<Statement>> statements)
{
	m_statements = std::move(statements);
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
	m_variables[statement->name.token] = evaluate(statement->exp);
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
	}
	return {ValueType::Null };
}

Value Interpreter::visitVar(VarExpression* expression)
{
	Token& name = expression->name;
	if (m_variables.find(name.token) != m_variables.end())
	{
		return m_variables[name.token];
	}
	else
	{
		std::cout << "Error at (" << name.line << ":" << name.pos << "):" << std::endl;
		std::cout << "Variable " << name.token << " not defined" << std::endl;
		std::cin.get();
	}
}

Value Interpreter::toString(const Value& value)
{
	if (value.type == ValueType::String)
		return value;
	if (value.type == ValueType::Integer)
		return { ValueType::String, std::to_string(std::get<int>(value.val)) };
}