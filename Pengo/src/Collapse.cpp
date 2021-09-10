#include "Collapse.h"
#include <stack>
#include <type_traits>

std::vector<std::unique_ptr<Statement>> collapseParseTree(const Node* root)
{
	return deriveStatements(root);
}

static std::vector<std::unique_ptr<Statement>> deriveStatements(const Node* root)
{
	std::vector<std::unique_ptr<Statement>> statements;
	std::stack<const Node*> stack;
	stack.push(root);
	while (!stack.empty())
	{
		const Node* next = stack.top();
		stack.pop();
		if (next->type == NodeType::Block)
			statements.push_back(std::move(deriveStatement(*next)));
		else
			for (const Node& child : next->children)
				stack.push(&child);
	}
	std::reverse(statements.begin(), statements.end());
	return statements;
}

static std::unique_ptr<Statement> deriveStatement(const Node& root)
{
	switch (root.type)
	{
	case NodeType::Block:
	{
		if (root.children[0].type == NodeType::Statement)
		{
			return deriveStatement(root.children[0].children[0]);
		}
		else if (root.children[0].type == NodeType::Statements)
		{
			BlockStatement blockStatement;
			blockStatement.statements = deriveStatements(&root.children[0]);
			return std::make_unique<BlockStatement>(std::move(blockStatement));
		}
	}
	break;
	case NodeType::Expression:
	{
		ExpressionStatement expStatement;
		expStatement.exp = deriveExpression(root.children[0]);
		return std::make_unique<ExpressionStatement>(std::move(expStatement));
	}
	break;
	/*case NodeType::PrintStatement:
	{
		PrintStatement printStatement;
		printStatement.exp = deriveExpression(root.children[0]);
		return std::make_unique<PrintStatement>(std::move(printStatement));
	}
	break;*/
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
	case NodeType::Call:
		if (root.children.size() == 2) {
			CallExpression callExp;
			callExp.callee = deriveExpression(root.children[0]);
			std::stack<const Node*> stack;
			stack.push(&(root.children[1]));
			while (!stack.empty())
			{
				const Node* next = stack.top();
				stack.pop();
				if (next->type == NodeType::Expression)
					callExp.args.push_back(std::move(deriveExpression(*next)));
				else
					for (const Node& child : next->children)
						stack.push(&child);
			}
			std::reverse(callExp.args.begin(), callExp.args.end());
			return std::make_unique<CallExpression>(std::move(callExp));
		}
		break;
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