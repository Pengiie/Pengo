#pragma once

#include "Value.h"
#include "../tokenizer/Token.h"

#include <memory>
#include <vector>

struct CallExpression;
struct BinaryExpression;
struct VarExpression;
struct LiteralExpression;

struct Expression
{
	struct Visitor
	{
		virtual Value visitCall(CallExpression* expression) = 0;
		virtual Value visitBinary(BinaryExpression* expression) = 0;
		virtual Value visitLiteral(LiteralExpression* expression) = 0;
		virtual Value visitVar(VarExpression* expression) = 0;
	};

	virtual Value accept(Visitor& visitor) = 0;
};

struct CallExpression : public Expression
{
	std::unique_ptr<Expression> callee;
	std::vector<std::unique_ptr<Expression>> args;

	inline Value accept(Expression::Visitor& visitor) { return visitor.visitCall(this); }
};

struct BinaryExpression : public Expression
{
	/*BinaryExpression(const BinaryExpression& other) : left(std::move(other.left)), op(other.op), right(std::move(other.right))
	{
	}*/

	std::unique_ptr<Expression> left;
	Token op;
	std::unique_ptr<Expression> right;

	inline Value accept(Expression::Visitor& visitor) { return visitor.visitBinary(this); }
};

struct LiteralExpression : public Expression
{
	Token value;

	inline Value accept(Expression::Visitor& visitor) { return visitor.visitLiteral(this); }
};

struct VarExpression : public Expression
{
	Token name;

	inline Value accept(Expression::Visitor& visitor) { return visitor.visitVar(this); }
};

