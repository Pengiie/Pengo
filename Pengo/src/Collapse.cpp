#include "Collapse.h"
#include <stack>

std::vector<std::unique_ptr<Statement>> collapseParseTree(Node* root)
{
	std::vector<std::unique_ptr<Statement>> statements;
	std::stack<Node*> stack;
	stack.push(root);
	while (!stack.empty())
	{
		Node* next = stack.top();
		stack.pop();
		if (next->type == NodeType::Statement)
			statements.push_back(std::move(deriveStatement(next->children[0])));
		else
			for (Node& child : next->children)
				stack.push(&child);
	}
	std::reverse(statements.begin(), statements.end());
	return statements;
}

static std::unique_ptr<Statement> deriveStatement(const Node& root)
{
	switch (root.type)
	{
	case NodeType::Expression:
	{
		ExpressionStatement expStatement;
		expStatement.exp = deriveExpression(root.children[0]);
		return std::make_unique<ExpressionStatement>(std::move(expStatement));
	}
	break;
	case NodeType::PrintStatement:
	{
		PrintStatement printStatement;
		printStatement.exp = deriveExpression(root.children[0]);
		return std::make_unique<PrintStatement>(std::move(printStatement));
	}
	break;
	case NodeType::VarDeclareStatement:
	{
		VarDeclareStatement varDeclareStatement;
		varDeclareStatement.name = deriveToken(root.children[0]);
		varDeclareStatement.exp = deriveExpression(root.children[1]);
		return std::make_unique<VarDeclareStatement>(std::move(varDeclareStatement));
	}
	break;
	}
}

static std::unique_ptr<Expression> deriveExpression(const Node& root)
{
	switch (root.type)
	{
	case NodeType::Term:
	case NodeType::Factor:
		if (root.children.size() == 3)
		{
			BinaryExpression exp;
			exp.left = deriveExpression(root.children[0]);
			exp.op = deriveToken(root.children[1]);
			exp.right = deriveExpression(root.children[2]);
			return std::make_unique<BinaryExpression>(std::move(exp));
		}
		break;
	case NodeType::Literal:
	{
		LiteralExpression exp;
		exp.value = root.token;
		return std::make_unique<LiteralExpression>(exp);
	}
	break;
	case NodeType::Identifier:
	{
		VarExpression varExpression;
		varExpression.name = root.token;
		return std::make_unique<VarExpression>(std::move(varExpression));
	}
	break;
	}
	return deriveExpression(root.children[0]);
}

static Token deriveToken(const Node& root)
{
	if (root.terminal)
		return root.token;
	else
		return deriveToken(root.children[0]);
}