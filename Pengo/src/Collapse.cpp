#include "Collapse.h"
#include <stack>
#include <queue>
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
	bool lastWasIf = false;
	while (!stack.empty())
	{
		const Node* next = stack.top();
		stack.pop();
		if (next->type == NodeType::Block)
		{
			std::unique_ptr<Statement> stmt = std::move(deriveStatement(*next));
			bool tLastWas = lastWasIf;
			lastWasIf = stmt->type == StatementType::If || stmt->type == StatementType::ElseIf;
			if (stmt->type == StatementType::ElseIf || stmt->type == StatementType::Else)
			{
				if (tLastWas)
				{
					IfStatement* currentIf = (IfStatement*)statements[statements.size() - 1].get();
					while (currentIf->hasElse && currentIf->elze->type == StatementType::ElseIf)
						currentIf = (IfStatement*)currentIf->elze.get();
					currentIf->elze = std::move(stmt);
					currentIf->hasElse = true;
				}
				else
				{
					// DISPLAY ERROR
				}
			}
			else statements.push_back(std::move(stmt));
		}
		else
		{
			std::vector<Node*> tChildren;
			for (const Node& child : next->children)
				tChildren.push_back((Node*) &child);
			std::reverse(tChildren.begin(), tChildren.end());
			for (Node* child : tChildren)
				stack.push(child);
		}
	}
	return statements;
}

static std::unique_ptr<Statement> deriveStatement(const Node& root)
{
	switch (root.type)
	{
	case NodeType::ReturnStatement:
	{
		ReturnStatement returnStatement;
		returnStatement.hasExp = root.children.size() == 1;
		if(returnStatement.hasExp)
			returnStatement.exp = deriveExpression(root.children[0]);
		return std::make_unique<ReturnStatement>(std::move(returnStatement));
	}
	break;
	case NodeType::FunctionDeclareStatement:
	{
		FuncDeclareStatement funcStatement;
		funcStatement.name = deriveToken(root.children[0]);
		std::stack<const Node*> stack;
		stack.push(&(root.children[1]));
		while (!stack.empty())
		{
			const Node* next = stack.top();
			stack.pop();
			if (next->type == NodeType::Identifier)
				funcStatement.params.push_back(next->token);
			else
				for (const Node& child : next->children)
					stack.push(&child);
		}
		std::reverse(funcStatement.params.begin(), funcStatement.params.end());
		funcStatement.body = std::move(deriveStatement(root.children[2]));
		((BlockStatement*)funcStatement.body.get())->envType = EnvironmentType::Function;
		return std::make_unique<FuncDeclareStatement>(std::move(funcStatement));
	}
	break;
	case NodeType::WhileStatement:
	{
		WhileStatement whileStatement;
		whileStatement.condition = deriveExpression(root.children[0]);
		whileStatement.body = deriveStatement(root.children[1]);
		((BlockStatement*)whileStatement.body.get())->envType = EnvironmentType::Loop;
		return std::make_unique<WhileStatement>(std::move(whileStatement));
	}
	break;
	case NodeType::IfStatement:
	{
		IfStatement ifStatement;
		ifStatement.condition = deriveExpression(root.children[0]);
		ifStatement.body = deriveStatement(root.children[1]);
		ifStatement.hasElse = false;
		return std::make_unique<IfStatement>(std::move(ifStatement));
	}
	break;
	case NodeType::ElseIfStatement:
	{
		IfStatement ifStatement;
		ifStatement.condition = deriveExpression(root.children[0]);
		ifStatement.body = deriveStatement(root.children[1]);
		ifStatement.hasElse = false;
		ifStatement.type = StatementType::ElseIf;
		return std::make_unique<IfStatement>(std::move(ifStatement));
	}
	break;
	case NodeType::ElseStatement:
	{
		BlockStatement elseStatement;
		elseStatement.statements = deriveStatements(&root.children[0]);
		elseStatement.type = StatementType::Else;
		return std::make_unique<BlockStatement>(std::move(elseStatement));
	}
	break;
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
		else
		{
			return deriveStatement(root.children[0]);
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
	case NodeType::Logical:
		if (root.children.size() == 3)
		{
			LogicalExpression exp;
			exp.left = deriveExpression(root.children[0]);
			exp.op = deriveToken(root.children[1]);
			exp.right = deriveExpression(root.children[2]);
			return std::make_unique<LogicalExpression>(std::move(exp));
		}
		break;
	case NodeType::Conditional:
		if (root.children.size() == 3)
		{
			ConditionalExpression exp;
			exp.left = deriveExpression(root.children[0]);
			exp.op = deriveToken(root.children[1]);
			exp.right = deriveExpression(root.children[2]);
			return std::make_unique<ConditionalExpression>(std::move(exp));
		}
		break;
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
	case NodeType::Unary:
		if (root.children.size() == 2)
		{
			UnaryExpression exp;
			exp.op = deriveToken(root.children[0]);
			exp.exp = deriveExpression(root.children[1]);
			return std::make_unique<UnaryExpression>(std::move(exp));
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