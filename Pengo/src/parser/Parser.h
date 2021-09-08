#pragma once
#include "../tokenizer/Token.h"
#include <vector>
#include <stack>
#include <queue>
#include <unordered_map>
#include <iostream>
#include <algorithm>
#define SET_NUM 36
#define GRAMMAR_NUM 24
enum class NodeType
{
	Expression,
	File,
	Term,
	Statements,
	Statement,
	VarDeclareStatement,
	PrintStatement,
	TermOp,
	Factor,
	FactorOp,
	Primary,
	Grouping,
	Literal,
	Identifier,
};
struct Node
{
	NodeType type;
	bool terminal = false;
	Token token;
	std::vector<Node> children;
};
enum class ActionType
{
	None,
	Shift,
	Reduce,
	Accept
};
struct Action
{
	ActionType type = ActionType::None;
	int value = -1;
};
struct Production
{
	NodeType lhs;
	int rhs = 0;
	int rhsNodes = 0;
	bool terminates = false;
};
class Parser
{
public:
	Parser()
	{
		m_actionTable[8][TokenType::Minus] = { ActionType::Shift, 21 };
		m_actionTable[8][TokenType::Semicolon] = { ActionType::Reduce, 9 };
		m_actionTable[8][TokenType::Plus] = { ActionType::Shift, 23 };
		m_actionTable[8][TokenType::RightParen] = { ActionType::Reduce, 9 };
		m_actionTable[0][TokenType::Var] = { ActionType::Shift, 4 };
		m_actionTable[0][TokenType::Identifier] = { ActionType::Shift, 2 };
		m_actionTable[0][TokenType::Print] = { ActionType::Shift, 7 };
		m_actionTable[0][TokenType::Number] = { ActionType::Shift, 6 };
		m_actionTable[0][TokenType::String] = { ActionType::Shift, 5 };
		m_actionTable[0][TokenType::LeftParen] = { ActionType::Shift, 9 };
		m_actionTable[4][TokenType::Identifier] = { ActionType::Shift, 2 };
		m_actionTable[7][TokenType::Number] = { ActionType::Shift, 6 };
		m_actionTable[7][TokenType::String] = { ActionType::Shift, 5 };
		m_actionTable[7][TokenType::LeftParen] = { ActionType::Shift, 9 };
		m_actionTable[7][TokenType::Identifier] = { ActionType::Shift, 2 };
		m_actionTable[17][TokenType::Semicolon] = { ActionType::Shift, 28 };
		m_actionTable[9][TokenType::Number] = { ActionType::Shift, 6 };
		m_actionTable[9][TokenType::String] = { ActionType::Shift, 5 };
		m_actionTable[9][TokenType::LeftParen] = { ActionType::Shift, 9 };
		m_actionTable[9][TokenType::Identifier] = { ActionType::Shift, 2 };
		m_actionTable[13][TokenType::Asterisk] = { ActionType::Shift, 26 };
		m_actionTable[13][TokenType::Slash] = { ActionType::Shift, 27 };
		m_actionTable[13][TokenType::RightParen] = { ActionType::Reduce, 11 };
		m_actionTable[13][TokenType::Plus] = { ActionType::Reduce, 11 };
		m_actionTable[13][TokenType::Semicolon] = { ActionType::Reduce, 11 };
		m_actionTable[13][TokenType::Minus] = { ActionType::Reduce, 11 };
		m_actionTable[18][TokenType::Var] = { ActionType::Shift, 4 };
		m_actionTable[18][TokenType::Identifier] = { ActionType::Shift, 2 };
		m_actionTable[18][TokenType::Print] = { ActionType::Shift, 7 };
		m_actionTable[18][TokenType::Number] = { ActionType::Shift, 6 };
		m_actionTable[18][TokenType::String] = { ActionType::Shift, 5 };
		m_actionTable[18][TokenType::LeftParen] = { ActionType::Shift, 9 };
		m_actionTable[18][TokenType::_EOF] = { ActionType::Reduce, 1 };
		m_actionTable[19][TokenType::Equal] = { ActionType::Shift, 30 };
		m_actionTable[22][TokenType::Identifier] = { ActionType::Shift, 2 };
		m_actionTable[22][TokenType::Number] = { ActionType::Shift, 6 };
		m_actionTable[22][TokenType::String] = { ActionType::Shift, 5 };
		m_actionTable[22][TokenType::LeftParen] = { ActionType::Shift, 9 };
		m_actionTable[24][TokenType::RightParen] = { ActionType::Shift, 32 };
		m_actionTable[25][TokenType::Number] = { ActionType::Shift, 6 };
		m_actionTable[25][TokenType::String] = { ActionType::Shift, 5 };
		m_actionTable[25][TokenType::LeftParen] = { ActionType::Shift, 9 };
		m_actionTable[25][TokenType::Identifier] = { ActionType::Shift, 2 };
		m_actionTable[29][TokenType::Semicolon] = { ActionType::Shift, 34 };
		m_actionTable[30][TokenType::Number] = { ActionType::Shift, 6 };
		m_actionTable[30][TokenType::String] = { ActionType::Shift, 5 };
		m_actionTable[30][TokenType::LeftParen] = { ActionType::Shift, 9 };
		m_actionTable[30][TokenType::Identifier] = { ActionType::Shift, 2 };
		m_actionTable[31][TokenType::Asterisk] = { ActionType::Shift, 26 };
		m_actionTable[31][TokenType::Slash] = { ActionType::Shift, 27 };
		m_actionTable[31][TokenType::RightParen] = { ActionType::Reduce, 10 };
		m_actionTable[31][TokenType::Plus] = { ActionType::Reduce, 10 };
		m_actionTable[31][TokenType::Semicolon] = { ActionType::Reduce, 10 };
		m_actionTable[31][TokenType::Minus] = { ActionType::Reduce, 10 };
		m_actionTable[1][TokenType::_EOF] = { ActionType::Accept }; ;
		m_actionTable[2][TokenType::RightParen] = { ActionType::Reduce, 24 };
		m_actionTable[2][TokenType::Plus] = { ActionType::Reduce, 24 };
		m_actionTable[2][TokenType::Semicolon] = { ActionType::Reduce, 24 };
		m_actionTable[2][TokenType::Equal] = { ActionType::Reduce, 24 };
		m_actionTable[2][TokenType::Minus] = { ActionType::Reduce, 24 };
		m_actionTable[2][TokenType::Slash] = { ActionType::Reduce, 24 };
		m_actionTable[2][TokenType::Asterisk] = { ActionType::Reduce, 24 };
		m_actionTable[3][TokenType::RightParen] = { ActionType::Reduce, 18 };
		m_actionTable[3][TokenType::Plus] = { ActionType::Reduce, 18 };
		m_actionTable[3][TokenType::Semicolon] = { ActionType::Reduce, 18 };
		m_actionTable[3][TokenType::Minus] = { ActionType::Reduce, 18 };
		m_actionTable[3][TokenType::Slash] = { ActionType::Reduce, 18 };
		m_actionTable[3][TokenType::Asterisk] = { ActionType::Reduce, 18 };
		m_actionTable[5][TokenType::RightParen] = { ActionType::Reduce, 23 };
		m_actionTable[5][TokenType::Plus] = { ActionType::Reduce, 23 };
		m_actionTable[5][TokenType::Semicolon] = { ActionType::Reduce, 23 };
		m_actionTable[5][TokenType::Minus] = { ActionType::Reduce, 23 };
		m_actionTable[5][TokenType::Slash] = { ActionType::Reduce, 23 };
		m_actionTable[5][TokenType::Asterisk] = { ActionType::Reduce, 23 };
		m_actionTable[6][TokenType::RightParen] = { ActionType::Reduce, 22 };
		m_actionTable[6][TokenType::Plus] = { ActionType::Reduce, 22 };
		m_actionTable[6][TokenType::Semicolon] = { ActionType::Reduce, 22 };
		m_actionTable[6][TokenType::Minus] = { ActionType::Reduce, 22 };
		m_actionTable[6][TokenType::Slash] = { ActionType::Reduce, 22 };
		m_actionTable[6][TokenType::Asterisk] = { ActionType::Reduce, 22 };
		m_actionTable[10][TokenType::RightParen] = { ActionType::Reduce, 19 };
		m_actionTable[10][TokenType::Plus] = { ActionType::Reduce, 19 };
		m_actionTable[10][TokenType::Semicolon] = { ActionType::Reduce, 19 };
		m_actionTable[10][TokenType::Minus] = { ActionType::Reduce, 19 };
		m_actionTable[10][TokenType::Slash] = { ActionType::Reduce, 19 };
		m_actionTable[10][TokenType::Asterisk] = { ActionType::Reduce, 19 };
		m_actionTable[11][TokenType::RightParen] = { ActionType::Reduce, 20 };
		m_actionTable[11][TokenType::Plus] = { ActionType::Reduce, 20 };
		m_actionTable[11][TokenType::Semicolon] = { ActionType::Reduce, 20 };
		m_actionTable[11][TokenType::Minus] = { ActionType::Reduce, 20 };
		m_actionTable[11][TokenType::Slash] = { ActionType::Reduce, 20 };
		m_actionTable[11][TokenType::Asterisk] = { ActionType::Reduce, 20 };
		m_actionTable[12][TokenType::RightParen] = { ActionType::Reduce, 15 };
		m_actionTable[12][TokenType::Plus] = { ActionType::Reduce, 15 };
		m_actionTable[12][TokenType::Semicolon] = { ActionType::Reduce, 15 };
		m_actionTable[12][TokenType::Minus] = { ActionType::Reduce, 15 };
		m_actionTable[12][TokenType::Slash] = { ActionType::Reduce, 15 };
		m_actionTable[12][TokenType::Asterisk] = { ActionType::Reduce, 15 };
		m_actionTable[14][TokenType::Semicolon] = { ActionType::Reduce, 4 };
		m_actionTable[15][TokenType::Semicolon] = { ActionType::Reduce, 5 };
		m_actionTable[16][TokenType::Semicolon] = { ActionType::Reduce, 6 };
		m_actionTable[20][TokenType::Semicolon] = { ActionType::Reduce, 7 };
		m_actionTable[21][TokenType::Number] = { ActionType::Reduce, 13 };
		m_actionTable[21][TokenType::String] = { ActionType::Reduce, 13 };
		m_actionTable[21][TokenType::LeftParen] = { ActionType::Reduce, 13 };
		m_actionTable[21][TokenType::Identifier] = { ActionType::Reduce, 13 };
		m_actionTable[23][TokenType::Number] = { ActionType::Reduce, 12 };
		m_actionTable[23][TokenType::String] = { ActionType::Reduce, 12 };
		m_actionTable[23][TokenType::LeftParen] = { ActionType::Reduce, 12 };
		m_actionTable[23][TokenType::Identifier] = { ActionType::Reduce, 12 };
		m_actionTable[26][TokenType::Number] = { ActionType::Reduce, 16 };
		m_actionTable[26][TokenType::String] = { ActionType::Reduce, 16 };
		m_actionTable[26][TokenType::LeftParen] = { ActionType::Reduce, 16 };
		m_actionTable[26][TokenType::Identifier] = { ActionType::Reduce, 16 };
		m_actionTable[27][TokenType::Number] = { ActionType::Reduce, 17 };
		m_actionTable[27][TokenType::String] = { ActionType::Reduce, 17 };
		m_actionTable[27][TokenType::LeftParen] = { ActionType::Reduce, 17 };
		m_actionTable[27][TokenType::Identifier] = { ActionType::Reduce, 17 };
		m_actionTable[28][TokenType::_EOF] = { ActionType::Reduce, 3 };
		m_actionTable[28][TokenType::Identifier] = { ActionType::Reduce, 3 };
		m_actionTable[28][TokenType::Var] = { ActionType::Reduce, 3 };
		m_actionTable[28][TokenType::Number] = { ActionType::Reduce, 3 };
		m_actionTable[28][TokenType::String] = { ActionType::Reduce, 3 };
		m_actionTable[28][TokenType::Print] = { ActionType::Reduce, 3 };
		m_actionTable[28][TokenType::LeftParen] = { ActionType::Reduce, 3 };
		m_actionTable[32][TokenType::RightParen] = { ActionType::Reduce, 21 };
		m_actionTable[32][TokenType::Plus] = { ActionType::Reduce, 21 };
		m_actionTable[32][TokenType::Semicolon] = { ActionType::Reduce, 21 };
		m_actionTable[32][TokenType::Minus] = { ActionType::Reduce, 21 };
		m_actionTable[32][TokenType::Slash] = { ActionType::Reduce, 21 };
		m_actionTable[32][TokenType::Asterisk] = { ActionType::Reduce, 21 };
		m_actionTable[33][TokenType::RightParen] = { ActionType::Reduce, 14 };
		m_actionTable[33][TokenType::Plus] = { ActionType::Reduce, 14 };
		m_actionTable[33][TokenType::Semicolon] = { ActionType::Reduce, 14 };
		m_actionTable[33][TokenType::Minus] = { ActionType::Reduce, 14 };
		m_actionTable[33][TokenType::Slash] = { ActionType::Reduce, 14 };
		m_actionTable[33][TokenType::Asterisk] = { ActionType::Reduce, 14 };
		m_actionTable[34][TokenType::_EOF] = { ActionType::Reduce, 2 };
		m_actionTable[34][TokenType::Identifier] = { ActionType::Reduce, 2 };
		m_actionTable[34][TokenType::Var] = { ActionType::Reduce, 2 };
		m_actionTable[34][TokenType::Number] = { ActionType::Reduce, 2 };
		m_actionTable[34][TokenType::String] = { ActionType::Reduce, 2 };
		m_actionTable[34][TokenType::Print] = { ActionType::Reduce, 2 };
		m_actionTable[34][TokenType::LeftParen] = { ActionType::Reduce, 2 };
		m_actionTable[35][TokenType::Semicolon] = { ActionType::Reduce, 8 };
		m_gotoTable[8][NodeType::TermOp] = 22;
		m_gotoTable[0][NodeType::Expression] = 14;
		m_gotoTable[0][NodeType::File] = 1;
		m_gotoTable[0][NodeType::Factor] = 13;
		m_gotoTable[0][NodeType::Identifier] = 3;
		m_gotoTable[0][NodeType::Term] = 8;
		m_gotoTable[0][NodeType::Literal] = 11;
		m_gotoTable[0][NodeType::Grouping] = 10;
		m_gotoTable[0][NodeType::Primary] = 12;
		m_gotoTable[0][NodeType::PrintStatement] = 15;
		m_gotoTable[0][NodeType::VarDeclareStatement] = 16;
		m_gotoTable[0][NodeType::Statement] = 17;
		m_gotoTable[0][NodeType::Statements] = 18;
		m_gotoTable[4][NodeType::Identifier] = 19;
		m_gotoTable[7][NodeType::Expression] = 20;
		m_gotoTable[7][NodeType::Term] = 8;
		m_gotoTable[7][NodeType::Factor] = 13;
		m_gotoTable[7][NodeType::Identifier] = 3;
		m_gotoTable[7][NodeType::Grouping] = 10;
		m_gotoTable[7][NodeType::Literal] = 11;
		m_gotoTable[7][NodeType::Primary] = 12;
		m_gotoTable[9][NodeType::Expression] = 24;
		m_gotoTable[9][NodeType::Term] = 8;
		m_gotoTable[9][NodeType::Factor] = 13;
		m_gotoTable[9][NodeType::Identifier] = 3;
		m_gotoTable[9][NodeType::Grouping] = 10;
		m_gotoTable[9][NodeType::Literal] = 11;
		m_gotoTable[9][NodeType::Primary] = 12;
		m_gotoTable[13][NodeType::FactorOp] = 25;
		m_gotoTable[18][NodeType::Factor] = 13;
		m_gotoTable[18][NodeType::Statement] = 29;
		m_gotoTable[18][NodeType::Identifier] = 3;
		m_gotoTable[18][NodeType::Term] = 8;
		m_gotoTable[18][NodeType::Literal] = 11;
		m_gotoTable[18][NodeType::Grouping] = 10;
		m_gotoTable[18][NodeType::Primary] = 12;
		m_gotoTable[18][NodeType::Expression] = 14;
		m_gotoTable[18][NodeType::PrintStatement] = 15;
		m_gotoTable[18][NodeType::VarDeclareStatement] = 16;
		m_gotoTable[22][NodeType::Factor] = 31;
		m_gotoTable[22][NodeType::Identifier] = 3;
		m_gotoTable[22][NodeType::Grouping] = 10;
		m_gotoTable[22][NodeType::Literal] = 11;
		m_gotoTable[22][NodeType::Primary] = 12;
		m_gotoTable[25][NodeType::Primary] = 33;
		m_gotoTable[25][NodeType::Identifier] = 3;
		m_gotoTable[25][NodeType::Grouping] = 10;
		m_gotoTable[25][NodeType::Literal] = 11;
		m_gotoTable[30][NodeType::Expression] = 35;
		m_gotoTable[30][NodeType::Term] = 8;
		m_gotoTable[30][NodeType::Factor] = 13;
		m_gotoTable[30][NodeType::Identifier] = 3;
		m_gotoTable[30][NodeType::Grouping] = 10;
		m_gotoTable[30][NodeType::Literal] = 11;
		m_gotoTable[30][NodeType::Primary] = 12;
		m_gotoTable[31][NodeType::FactorOp] = 25;
		m_grammar[0] = { NodeType::File, 1, 1, false };
		m_grammar[1] = { NodeType::Statements, 3, 2, false };
		m_grammar[2] = { NodeType::Statements, 2, 1, false };
		m_grammar[3] = { NodeType::Statement, 1, 1, false };
		m_grammar[4] = { NodeType::Statement, 1, 1, false };
		m_grammar[5] = { NodeType::Statement, 1, 1, false };
		m_grammar[6] = { NodeType::PrintStatement, 2, 1, false };
		m_grammar[7] = { NodeType::VarDeclareStatement, 4, 2, false };
		m_grammar[8] = { NodeType::Expression, 1, 1, false };
		m_grammar[9] = { NodeType::Term, 3, 3, false };
		m_grammar[10] = { NodeType::Term, 1, 1, false };
		m_grammar[11] = { NodeType::TermOp, 1, 0, true };
		m_grammar[12] = { NodeType::TermOp, 1, 0, true };
		m_grammar[13] = { NodeType::Factor, 3, 3, false };
		m_grammar[14] = { NodeType::Factor, 1, 1, false };
		m_grammar[15] = { NodeType::FactorOp, 1, 0, true };
		m_grammar[16] = { NodeType::FactorOp, 1, 0, true };
		m_grammar[17] = { NodeType::Primary, 1, 1, false };
		m_grammar[18] = { NodeType::Primary, 1, 1, false };
		m_grammar[19] = { NodeType::Primary, 1, 1, false };
		m_grammar[20] = { NodeType::Grouping, 3, 1, false };
		m_grammar[21] = { NodeType::Literal, 1, 0, true };
		m_grammar[22] = { NodeType::Literal, 1, 0, true };
		m_grammar[23] = { NodeType::Identifier, 1, 0, true };
	}
	Node parse(std::vector<Token> tokens)
	{
		std::queue<Token, std::deque<Token>> input(std::deque<Token>(tokens.begin(), tokens.end()));
		std::stack<int> stack;
		stack.push(0);
		std::stack<Node> output;
		Token beforeToken = input.front();
		int state = 0;
		while (true)
		{
			state = stack.top();
			Token& current = input.front();
			Action& nextAction = m_actionTable[state][current.type];
			if (nextAction.type == ActionType::None)
			{
				if (current.type == TokenType::_EOF)
					std::cout << "Unexpected end of file at line " << current.line << " at pos " << current.pos << std::endl;
				else
					std::cout << "Error parsing line " << current.line << " at pos " << current.pos << std::endl;
				std::cin.get();
			}
			else if (nextAction.type == ActionType::Shift)
			{
				beforeToken = input.front();
				input.pop();
				stack.push(nextAction.value);
			}
			else if (nextAction.type == ActionType::Reduce)
			{
				// Push the rule to the stack
				Production production = m_grammar[nextAction.value - 1];
				for (int i = 0; i < production.rhs; i++)
					stack.pop();
				stack.push(m_gotoTable[stack.top()][production.lhs]);
				Node node;
				node.terminal = production.terminates;
				node.type = production.lhs;
				if (production.terminates)
					node.token = beforeToken;
				else
				{
					for (int i = 0; i < production.rhsNodes; i++)
					{
						node.children.push_back(output.top());
						output.pop();
					}
				}
				output.push(node);
			}
			else if (nextAction.type == ActionType::Accept)
			{
				break;
			}
		}
		Node root = output.top();
		std::stack<Node*> toReverse({ &root });
		while (!toReverse.empty())
		{
			Node* node = toReverse.top();
			toReverse.pop();
			for (Node& child : node->children)
				toReverse.push(&child);
			std::reverse(node->children.begin(), node->children.end());
		}
		return root;
	}
private:
	Production m_grammar[GRAMMAR_NUM];
	std::unordered_map<TokenType, Action> m_actionTable[SET_NUM];
	std::unordered_map<NodeType, int> m_gotoTable[SET_NUM];
};
