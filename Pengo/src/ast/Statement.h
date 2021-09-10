#pragma once

#include "Expression.h"
#include "../tokenizer/Token.h"

#include <memory>
#include <vector>

struct BlockStatement;
struct ExpressionStatement;
struct PrintStatement;
struct VarDeclareStatement;

struct Statement
{
	struct Visitor
	{
		virtual void visitBlock(BlockStatement* statement) = 0;
		virtual void visitExpression(ExpressionStatement* statement) = 0;
		virtual void visitPrint(PrintStatement* statement) = 0;
		virtual void visitVarDeclare(VarDeclareStatement* statement) = 0;
	};

	virtual void accept(Visitor& visitor) = 0;
};

struct BlockStatement : public Statement
{
	std::vector<std::unique_ptr<Statement>> statements;

	inline void accept(Statement::Visitor& visitor) { visitor.visitBlock(this); }
};

struct ExpressionStatement : public Statement
{
	std::unique_ptr<Expression> exp;

	inline void accept(Statement::Visitor& visitor) { visitor.visitExpression(this); }
};

struct PrintStatement : public Statement
{
	std::unique_ptr<Expression> exp;

	inline void accept(Statement::Visitor& visitor) { visitor.visitPrint(this); }
};

struct VarDeclareStatement : public Statement
{
	Token name;
	std::unique_ptr<Expression> exp;

	inline void accept(Statement::Visitor& visitor) { visitor.visitVarDeclare(this); }
};