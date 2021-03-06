#pragma once

#include "ast\Expression.h"
#include "ast\Statement.h"
#include "Environment.h"
#include "tokenizer\Token.h"
#include "Function.h"

#include <vector>
#include <unordered_map>
#include "IterableStack.h"

#include <random>


class Interpreter : private Expression::Visitor, private Statement::Visitor
{
public:
	Interpreter(std::vector<std::unique_ptr<Statement>> statements);
	~Interpreter();

	void interpret();

	Value toString(const Value& value);

	//inline const std::unordered_map<std::string, Function*>& getFunctions() const { return m_functions; }
	inline std::default_random_engine& getRandom() { return m_random;  }
	inline std::iterable_stack<Environment>& getEnvStack() { return m_envStack; }
private:
	std::vector<std::unique_ptr<Statement>> m_statements;
	Environment m_globalEnvironment;
	std::iterable_stack<Environment> m_envStack;
	std::default_random_engine m_random;

	Value evaluate(const std::unique_ptr<Expression>& expression);

	Value visitLogical(LogicalExpression* expression);
	Value visitConditional(ConditionalExpression* expression);
	Value visitUnary(UnaryExpression* expression);
	Value visitCall(CallExpression* expression);
	Value visitBinary(BinaryExpression* expression);
	Value visitLiteral(LiteralExpression* expression);
	Value visitVar(VarExpression* expression);

	void visitReturn(ReturnStatement* statement);
	void visitFuncDeclare(FuncDeclareStatement* statement);
	void visitWhile(WhileStatement* statement);
	void visitIf(IfStatement* statement);
	void visitBlock(BlockStatement* statement);
	void visitExpression(ExpressionStatement* statement);
	void visitPrint(PrintStatement* statement);
	void visitVarDeclare(VarDeclareStatement* statement);

	Value findVariable(const Token& name);
};