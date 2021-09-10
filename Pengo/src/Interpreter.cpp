#include "Interpreter.h"

#include <iostream>
#include <string>
#include <variant>

#include "Function.h"

Interpreter::Interpreter(std::vector<std::unique_ptr<Statement>> statements)
{
	m_statements = std::move(statements);

	m_functions[FUN_PRINT] = new Print();
	m_functions[FUN_PRINTLN] = new PrintLn();
	m_functions[FUN_INPUT] = new Input();
	m_functions[FUN_TOINT] = new ToInt();
	m_functions[FUN_RANDOM] = new Random();
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
	if(m_envStack.empty())
		m_globalEnvironment.m_variables[statement->name.token] = evaluate(statement->exp);
	else
		m_envStack.top().m_variables[statement->name.token] = evaluate(statement->exp);
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