#include <string>
#include "tokenizer\Token.h"
#include "tokenizer\Tokenizer.h"
#include <iostream>
#include <sstream>
#include <fstream>

#include <queue>

#include "parser\Parser.h"

#include "Interpreter.h"
#include "Collapse.h"

std::string getType(NodeType type)
{
	switch (type)
	{
	case NodeType::File: return "File";
	case NodeType::Statements: return "Statements";
	case NodeType::Block: return "Block";
	case NodeType::Statement: return "Statement";
	//case NodeType::PrintStatement: return "PrintStatement";
	case NodeType::ReturnStatement: return "Return";
	case NodeType::FunctionDeclareStatement: return "FuncDeclare";
	case NodeType::Parameters: return "Parameters";
	case NodeType::ParamRecurse: return "ParamRecurse";
	case NodeType::WhileStatement: return "While";
	case NodeType::IfStatement: return "If";
	case NodeType::ElseIfStatement: return "Elseif";
	case NodeType::ElseStatement: return "Else";
	case NodeType::VarDeclareStatement: return "VarDeclareStatement";
	case NodeType::Expression: return "Expression";
	case NodeType::Logical: return "Logical";
	case NodeType::LogicalOp: return "LogicalOp";
	case NodeType::Conditional: return "Conditional";
	case NodeType::ConditionalOp: return "ConditionalOp";
	case NodeType::Term: return "Term";
	case NodeType::TermOp: return "TermOp";
	case NodeType::Factor: return "Factor";
	case NodeType::FactorOp: return "FactorOp";
	case NodeType::Unary: return "Unary";
	case NodeType::UnaryOp: return "UnaryOp";
	case NodeType::Call: return "Call";
	case NodeType::Arguments: return "Arguments";
	case NodeType::ArgRecurse: return "ArgRecurse";
	case NodeType::Primary: return "Primary";
	case NodeType::Literal: return "Literal";
	case NodeType::Grouping: return "Grouping";
	case NodeType::Identifier: return "Identifier";
	
	}
}

#define INC 3
void print(Node& node, int &inc)
{
	for (int i = 0; i < inc; i++)
		std::cout << " ";
	std::cout << getType(node.type) << std::endl;
	if (node.terminal)
	{
		for (int i = 0; i < inc + INC; i++)
			std::cout << " ";
		std::cout << node.token.token << std::endl;
	}
	inc += INC;
	for (Node& child : node.children)
		print(child, inc);
	inc -= INC;
}

int main(int argc, char* args[])
{
	if (argc == 1)
	{
		std::cout << "Expected input file! " << std::endl;
	}
	else
	{
		bool debug = false;
		if (argc == 3)
			if (std::string(args[2]) == std::string("-d"))
				debug = true;
		const char* path = args[1];
		std::ifstream file(path);

		if (!file.is_open())
			throw std::runtime_error(std::string("Couldn't open file ") + path);
		std::stringstream stream;
		stream << file.rdbuf();

		std::string source = stream.str();

		Tokenizer t(source);
		std::vector<Token> tokens = t.tokenize();

		Parser parser;

		Node base = parser.parse(tokens);

		// Print the parse tree
		if (debug)
		{
			int inc = 0;
			print(base, inc);
			std::cout << std::endl;
		}

		Interpreter interpreter(collapseParseTree(&base));
		interpreter.interpret();
	}
}