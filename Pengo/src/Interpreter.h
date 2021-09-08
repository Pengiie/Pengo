#pragma once

#include "ast\Expression.h"
#include "ast\Statement.h"

#include <vector>
#include <unordered_map>

class Interpreter : private Expression::Visitor, private Statement::Visitor
{
public:
	Interpreter(std::vector<std::unique_ptr<Statement>> statements);

	void interpret();
private:
	std::vector<std::unique_ptr<Statement>> m_statements;
	std::unordered_map<std::string, Value> m_variables;

	Value evaluate(const std::unique_ptr<Expression>& expression);

	Value visitBinary(BinaryExpression* expression);
	Value visitLiteral(LiteralExpression* expression);
	Value visitVar(VarExpression* expression);

	void visitExpression(ExpressionStatement* statement);
	void visitPrint(PrintStatement* statement);
	void visitVarDeclare(VarDeclareStatement* statement);

	Value toString(const Value& value);
};