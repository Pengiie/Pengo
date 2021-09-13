#pragma once

#include "Expression.h"
#include "../tokenizer/Token.h"
#include "../Environment.h"

#include <memory>
#include <vector>

struct ReturnStatement;
struct WhileStatement;
struct IfStatement;
struct BlockStatement;
struct ExpressionStatement;
struct PrintStatement;
struct VarDeclareStatement;
struct FuncDeclareStatement;

enum class StatementType
{
	Return,
	FuncDeclare,
	While,
	If,
	ElseIf,
	Else,
	Block,
	Expression,
	VarDeclare
};

struct Statement
{
	Statement(StatementType type) : type(type) {}
	struct Visitor
	{
		virtual void visitReturn(ReturnStatement* statement) = 0;
		virtual void visitFuncDeclare(FuncDeclareStatement* statement) = 0;
		virtual void visitWhile(WhileStatement* statement) = 0;
		virtual void visitIf(IfStatement* statement) = 0;
		virtual void visitBlock(BlockStatement* statement) = 0;
		virtual void visitExpression(ExpressionStatement* statement) = 0;
		virtual void visitPrint(PrintStatement* statement) = 0;
		virtual void visitVarDeclare(VarDeclareStatement* statement) = 0;
	};
	StatementType type;

	virtual void accept(Visitor& visitor) = 0;
};
struct ReturnStatement : public Statement
{
	ReturnStatement() : Statement(StatementType::Return) {}

	std::unique_ptr<Expression> exp;
	bool hasExp;

	inline void accept(Statement::Visitor& visitor) { visitor.visitReturn(this); }
};

struct WhileStatement : public Statement
{
	WhileStatement() : Statement(StatementType::While) {}

	std::unique_ptr<Expression> condition;
	std::unique_ptr<Statement> body;

	inline void accept(Statement::Visitor& visitor) { visitor.visitWhile(this); }
};

struct FuncDeclareStatement : public Statement
{
	FuncDeclareStatement() : Statement(StatementType::FuncDeclare) {}

	Token name;
	std::vector<Token> params;
	std::shared_ptr<Statement> body;

	inline void accept(Statement::Visitor& visitor) { visitor.visitFuncDeclare(this); }
};

struct IfStatement : public Statement
{
	IfStatement() : Statement(StatementType::If) {}

	std::unique_ptr<Expression> condition;
	std::unique_ptr<Statement> body;

	std::unique_ptr<Statement> elze;
	bool hasElse = false;

	inline void accept(Statement::Visitor& visitor) { visitor.visitIf(this); }
};

struct BlockStatement : public Statement
{
	BlockStatement() : Statement(StatementType::Block) {}

	EnvironmentType envType = EnvironmentType::Generic;
	std::vector<std::unique_ptr<Statement>> statements;

	inline void accept(Statement::Visitor& visitor) { visitor.visitBlock(this); }
};

struct ExpressionStatement : public Statement
{
	ExpressionStatement() : Statement(StatementType::Expression) {}

	std::unique_ptr<Expression> exp;

	inline void accept(Statement::Visitor& visitor) { visitor.visitExpression(this); }
};

struct PrintStatement : public Statement
{
	PrintStatement() : Statement(StatementType::Expression) {}

	std::unique_ptr<Expression> exp;

	inline void accept(Statement::Visitor& visitor) { visitor.visitPrint(this); }
};

struct VarDeclareStatement : public Statement
{
	VarDeclareStatement() : Statement(StatementType::VarDeclare) {}

	Token name;
	std::unique_ptr<Expression> exp;

	inline void accept(Statement::Visitor& visitor) { visitor.visitVarDeclare(this); }
};