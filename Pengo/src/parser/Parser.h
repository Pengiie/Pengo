#pragma once
#include "../tokenizer/Token.h"
#include <vector>
#include <stack>
#include <queue>
#include <unordered_map>
#include <iostream>
#include <algorithm>
#define SET_NUM 44
#define GRAMMAR_NUM 30
enum class NodeType
{
	Expression,
	File,
	Term,
	Arguments,
	Statements,
	Block,
	Statement,
	VarDeclareStatement,
	TermOp,
	Factor,
	FactorOp,
	Call,
	ArgRecurse,
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
		m_actionTable[16][TokenType::Semicolon] = { ActionType::Shift, 30 };
		m_actionTable[0][TokenType::Identifier] = { ActionType::Shift, 3 };
		m_actionTable[0][TokenType::Number] = { ActionType::Shift, 5 };
		m_actionTable[0][TokenType::String] = { ActionType::Shift, 4 };
		m_actionTable[0][TokenType::LeftParen] = { ActionType::Shift, 7 };
		m_actionTable[0][TokenType::LeftCurly] = { ActionType::Shift, 15 };
		m_actionTable[10][TokenType::LeftParen] = { ActionType::Shift, 25 };
		m_actionTable[10][TokenType::RightParen] = { ActionType::Reduce, 19 };
		m_actionTable[10][TokenType::Plus] = { ActionType::Reduce, 19 };
		m_actionTable[10][TokenType::Semicolon] = { ActionType::Reduce, 19 };
		m_actionTable[10][TokenType::Minus] = { ActionType::Reduce, 19 };
		m_actionTable[10][TokenType::Comma] = { ActionType::Reduce, 19 };
		m_actionTable[10][TokenType::Slash] = { ActionType::Reduce, 19 };
		m_actionTable[10][TokenType::Asterisk] = { ActionType::Reduce, 19 };
		m_actionTable[2][TokenType::Minus] = { ActionType::Reduce, 24 };
		m_actionTable[2][TokenType::Equal] = { ActionType::Shift, 19 };
		m_actionTable[2][TokenType::Plus] = { ActionType::Reduce, 24 };
		m_actionTable[2][TokenType::Semicolon] = { ActionType::Reduce, 24 };
		m_actionTable[2][TokenType::Slash] = { ActionType::Reduce, 24 };
		m_actionTable[2][TokenType::Asterisk] = { ActionType::Reduce, 24 };
		m_actionTable[2][TokenType::LeftParen] = { ActionType::Reduce, 24 };
		m_actionTable[6][TokenType::Minus] = { ActionType::Shift, 20 };
		m_actionTable[6][TokenType::Semicolon] = { ActionType::Reduce, 9 };
		m_actionTable[6][TokenType::Plus] = { ActionType::Shift, 22 };
		m_actionTable[6][TokenType::Comma] = { ActionType::Reduce, 9 };
		m_actionTable[6][TokenType::RightParen] = { ActionType::Reduce, 9 };
		m_actionTable[15][TokenType::LeftParen] = { ActionType::Shift, 7 };
		m_actionTable[15][TokenType::Identifier] = { ActionType::Shift, 3 };
		m_actionTable[15][TokenType::Number] = { ActionType::Shift, 5 };
		m_actionTable[15][TokenType::String] = { ActionType::Shift, 4 };
		m_actionTable[15][TokenType::LeftCurly] = { ActionType::Shift, 15 };
		m_actionTable[7][TokenType::Number] = { ActionType::Shift, 5 };
		m_actionTable[7][TokenType::String] = { ActionType::Shift, 4 };
		m_actionTable[7][TokenType::LeftParen] = { ActionType::Shift, 7 };
		m_actionTable[7][TokenType::Identifier] = { ActionType::Shift, 3 };
		m_actionTable[12][TokenType::Asterisk] = { ActionType::Shift, 27 };
		m_actionTable[12][TokenType::Slash] = { ActionType::Shift, 28 };
		m_actionTable[12][TokenType::RightParen] = { ActionType::Reduce, 11 };
		m_actionTable[12][TokenType::Plus] = { ActionType::Reduce, 11 };
		m_actionTable[12][TokenType::Semicolon] = { ActionType::Reduce, 11 };
		m_actionTable[12][TokenType::Minus] = { ActionType::Reduce, 11 };
		m_actionTable[12][TokenType::Comma] = { ActionType::Reduce, 11 };
		m_actionTable[18][TokenType::Identifier] = { ActionType::Shift, 3 };
		m_actionTable[18][TokenType::Number] = { ActionType::Shift, 5 };
		m_actionTable[18][TokenType::String] = { ActionType::Shift, 4 };
		m_actionTable[18][TokenType::LeftParen] = { ActionType::Shift, 7 };
		m_actionTable[18][TokenType::LeftCurly] = { ActionType::Shift, 15 };
		m_actionTable[18][TokenType::_EOF] = { ActionType::Reduce, 1 };
		m_actionTable[19][TokenType::Number] = { ActionType::Shift, 5 };
		m_actionTable[19][TokenType::String] = { ActionType::Shift, 4 };
		m_actionTable[19][TokenType::LeftParen] = { ActionType::Shift, 7 };
		m_actionTable[19][TokenType::Identifier] = { ActionType::Shift, 3 };
		m_actionTable[21][TokenType::Identifier] = { ActionType::Shift, 3 };
		m_actionTable[21][TokenType::Number] = { ActionType::Shift, 5 };
		m_actionTable[21][TokenType::String] = { ActionType::Shift, 4 };
		m_actionTable[21][TokenType::LeftParen] = { ActionType::Shift, 7 };
		m_actionTable[23][TokenType::RightParen] = { ActionType::Shift, 34 };
		m_actionTable[25][TokenType::LeftParen] = { ActionType::Shift, 7 };
		m_actionTable[25][TokenType::Number] = { ActionType::Shift, 5 };
		m_actionTable[25][TokenType::String] = { ActionType::Shift, 4 };
		m_actionTable[25][TokenType::Identifier] = { ActionType::Shift, 3 };
		m_actionTable[25][TokenType::RightParen] = { ActionType::Reduce, 20 };
		m_actionTable[26][TokenType::Number] = { ActionType::Shift, 5 };
		m_actionTable[26][TokenType::String] = { ActionType::Shift, 4 };
		m_actionTable[26][TokenType::LeftParen] = { ActionType::Shift, 7 };
		m_actionTable[26][TokenType::Identifier] = { ActionType::Shift, 3 };
		m_actionTable[29][TokenType::RightCurly] = { ActionType::Shift, 38 };
		m_actionTable[29][TokenType::Number] = { ActionType::Shift, 5 };
		m_actionTable[29][TokenType::String] = { ActionType::Shift, 4 };
		m_actionTable[29][TokenType::Identifier] = { ActionType::Shift, 3 };
		m_actionTable[29][TokenType::LeftParen] = { ActionType::Shift, 7 };
		m_actionTable[29][TokenType::LeftCurly] = { ActionType::Shift, 15 };
		m_actionTable[33][TokenType::Asterisk] = { ActionType::Shift, 27 };
		m_actionTable[33][TokenType::Slash] = { ActionType::Shift, 28 };
		m_actionTable[33][TokenType::RightParen] = { ActionType::Reduce, 10 };
		m_actionTable[33][TokenType::Plus] = { ActionType::Reduce, 10 };
		m_actionTable[33][TokenType::Semicolon] = { ActionType::Reduce, 10 };
		m_actionTable[33][TokenType::Minus] = { ActionType::Reduce, 10 };
		m_actionTable[33][TokenType::Comma] = { ActionType::Reduce, 10 };
		m_actionTable[35][TokenType::RightParen] = { ActionType::Shift, 39 };
		m_actionTable[36][TokenType::Comma] = { ActionType::Shift, 41 };
		m_actionTable[36][TokenType::RightParen] = { ActionType::Reduce, 22 };
		m_actionTable[41][TokenType::Number] = { ActionType::Shift, 5 };
		m_actionTable[41][TokenType::String] = { ActionType::Shift, 4 };
		m_actionTable[41][TokenType::LeftParen] = { ActionType::Shift, 7 };
		m_actionTable[41][TokenType::Identifier] = { ActionType::Shift, 3 };
		m_actionTable[42][TokenType::Comma] = { ActionType::Shift, 41 };
		m_actionTable[42][TokenType::RightParen] = { ActionType::Reduce, 22 };
		m_actionTable[1][TokenType::_EOF] = { ActionType::Accept }; ;
		m_actionTable[3][TokenType::RightParen] = { ActionType::Reduce, 30 };
		m_actionTable[3][TokenType::Semicolon] = { ActionType::Reduce, 30 };
		m_actionTable[3][TokenType::Plus] = { ActionType::Reduce, 30 };
		m_actionTable[3][TokenType::Slash] = { ActionType::Reduce, 30 };
		m_actionTable[3][TokenType::Asterisk] = { ActionType::Reduce, 30 };
		m_actionTable[3][TokenType::LeftParen] = { ActionType::Reduce, 30 };
		m_actionTable[3][TokenType::Minus] = { ActionType::Reduce, 30 };
		m_actionTable[3][TokenType::Equal] = { ActionType::Reduce, 30 };
		m_actionTable[3][TokenType::Comma] = { ActionType::Reduce, 30 };
		m_actionTable[4][TokenType::RightParen] = { ActionType::Reduce, 29 };
		m_actionTable[4][TokenType::Plus] = { ActionType::Reduce, 29 };
		m_actionTable[4][TokenType::Semicolon] = { ActionType::Reduce, 29 };
		m_actionTable[4][TokenType::Minus] = { ActionType::Reduce, 29 };
		m_actionTable[4][TokenType::Comma] = { ActionType::Reduce, 29 };
		m_actionTable[4][TokenType::Slash] = { ActionType::Reduce, 29 };
		m_actionTable[4][TokenType::Asterisk] = { ActionType::Reduce, 29 };
		m_actionTable[4][TokenType::LeftParen] = { ActionType::Reduce, 29 };
		m_actionTable[5][TokenType::RightParen] = { ActionType::Reduce, 28 };
		m_actionTable[5][TokenType::Plus] = { ActionType::Reduce, 28 };
		m_actionTable[5][TokenType::Semicolon] = { ActionType::Reduce, 28 };
		m_actionTable[5][TokenType::Minus] = { ActionType::Reduce, 28 };
		m_actionTable[5][TokenType::Comma] = { ActionType::Reduce, 28 };
		m_actionTable[5][TokenType::Slash] = { ActionType::Reduce, 28 };
		m_actionTable[5][TokenType::Asterisk] = { ActionType::Reduce, 28 };
		m_actionTable[5][TokenType::LeftParen] = { ActionType::Reduce, 28 };
		m_actionTable[8][TokenType::RightParen] = { ActionType::Reduce, 26 };
		m_actionTable[8][TokenType::Plus] = { ActionType::Reduce, 26 };
		m_actionTable[8][TokenType::Semicolon] = { ActionType::Reduce, 26 };
		m_actionTable[8][TokenType::Minus] = { ActionType::Reduce, 26 };
		m_actionTable[8][TokenType::Comma] = { ActionType::Reduce, 26 };
		m_actionTable[8][TokenType::Slash] = { ActionType::Reduce, 26 };
		m_actionTable[8][TokenType::Asterisk] = { ActionType::Reduce, 26 };
		m_actionTable[8][TokenType::LeftParen] = { ActionType::Reduce, 26 };
		m_actionTable[9][TokenType::RightParen] = { ActionType::Reduce, 25 };
		m_actionTable[9][TokenType::Plus] = { ActionType::Reduce, 25 };
		m_actionTable[9][TokenType::Semicolon] = { ActionType::Reduce, 25 };
		m_actionTable[9][TokenType::Minus] = { ActionType::Reduce, 25 };
		m_actionTable[9][TokenType::Comma] = { ActionType::Reduce, 25 };
		m_actionTable[9][TokenType::Slash] = { ActionType::Reduce, 25 };
		m_actionTable[9][TokenType::Asterisk] = { ActionType::Reduce, 25 };
		m_actionTable[9][TokenType::LeftParen] = { ActionType::Reduce, 25 };
		m_actionTable[11][TokenType::RightParen] = { ActionType::Reduce, 15 };
		m_actionTable[11][TokenType::Plus] = { ActionType::Reduce, 15 };
		m_actionTable[11][TokenType::Semicolon] = { ActionType::Reduce, 15 };
		m_actionTable[11][TokenType::Minus] = { ActionType::Reduce, 15 };
		m_actionTable[11][TokenType::Comma] = { ActionType::Reduce, 15 };
		m_actionTable[11][TokenType::Slash] = { ActionType::Reduce, 15 };
		m_actionTable[11][TokenType::Asterisk] = { ActionType::Reduce, 15 };
		m_actionTable[13][TokenType::Semicolon] = { ActionType::Reduce, 6 };
		m_actionTable[14][TokenType::Semicolon] = { ActionType::Reduce, 7 };
		m_actionTable[17][TokenType::Number] = { ActionType::Reduce, 3 };
		m_actionTable[17][TokenType::String] = { ActionType::Reduce, 3 };
		m_actionTable[17][TokenType::RightCurly] = { ActionType::Reduce, 3 };
		m_actionTable[17][TokenType::_EOF] = { ActionType::Reduce, 3 };
		m_actionTable[17][TokenType::Identifier] = { ActionType::Reduce, 3 };
		m_actionTable[17][TokenType::LeftParen] = { ActionType::Reduce, 3 };
		m_actionTable[17][TokenType::LeftCurly] = { ActionType::Reduce, 3 };
		m_actionTable[20][TokenType::Number] = { ActionType::Reduce, 13 };
		m_actionTable[20][TokenType::String] = { ActionType::Reduce, 13 };
		m_actionTable[20][TokenType::LeftParen] = { ActionType::Reduce, 13 };
		m_actionTable[20][TokenType::Identifier] = { ActionType::Reduce, 13 };
		m_actionTable[22][TokenType::Number] = { ActionType::Reduce, 12 };
		m_actionTable[22][TokenType::String] = { ActionType::Reduce, 12 };
		m_actionTable[22][TokenType::LeftParen] = { ActionType::Reduce, 12 };
		m_actionTable[22][TokenType::Identifier] = { ActionType::Reduce, 12 };
		m_actionTable[24][TokenType::Plus] = { ActionType::Reduce, 24 };
		m_actionTable[24][TokenType::Semicolon] = { ActionType::Reduce, 24 };
		m_actionTable[24][TokenType::RightParen] = { ActionType::Reduce, 24 };
		m_actionTable[24][TokenType::Minus] = { ActionType::Reduce, 24 };
		m_actionTable[24][TokenType::Comma] = { ActionType::Reduce, 24 };
		m_actionTable[24][TokenType::Slash] = { ActionType::Reduce, 24 };
		m_actionTable[24][TokenType::Asterisk] = { ActionType::Reduce, 24 };
		m_actionTable[24][TokenType::LeftParen] = { ActionType::Reduce, 24 };
		m_actionTable[27][TokenType::Number] = { ActionType::Reduce, 16 };
		m_actionTable[27][TokenType::String] = { ActionType::Reduce, 16 };
		m_actionTable[27][TokenType::LeftParen] = { ActionType::Reduce, 16 };
		m_actionTable[27][TokenType::Identifier] = { ActionType::Reduce, 16 };
		m_actionTable[28][TokenType::Number] = { ActionType::Reduce, 17 };
		m_actionTable[28][TokenType::String] = { ActionType::Reduce, 17 };
		m_actionTable[28][TokenType::LeftParen] = { ActionType::Reduce, 17 };
		m_actionTable[28][TokenType::Identifier] = { ActionType::Reduce, 17 };
		m_actionTable[30][TokenType::Number] = { ActionType::Reduce, 5 };
		m_actionTable[30][TokenType::String] = { ActionType::Reduce, 5 };
		m_actionTable[30][TokenType::RightCurly] = { ActionType::Reduce, 5 };
		m_actionTable[30][TokenType::_EOF] = { ActionType::Reduce, 5 };
		m_actionTable[30][TokenType::Identifier] = { ActionType::Reduce, 5 };
		m_actionTable[30][TokenType::LeftParen] = { ActionType::Reduce, 5 };
		m_actionTable[30][TokenType::LeftCurly] = { ActionType::Reduce, 5 };
		m_actionTable[31][TokenType::Number] = { ActionType::Reduce, 2 };
		m_actionTable[31][TokenType::String] = { ActionType::Reduce, 2 };
		m_actionTable[31][TokenType::RightCurly] = { ActionType::Reduce, 2 };
		m_actionTable[31][TokenType::_EOF] = { ActionType::Reduce, 2 };
		m_actionTable[31][TokenType::Identifier] = { ActionType::Reduce, 2 };
		m_actionTable[31][TokenType::LeftParen] = { ActionType::Reduce, 2 };
		m_actionTable[31][TokenType::LeftCurly] = { ActionType::Reduce, 2 };
		m_actionTable[32][TokenType::Semicolon] = { ActionType::Reduce, 8 };
		m_actionTable[34][TokenType::RightParen] = { ActionType::Reduce, 27 };
		m_actionTable[34][TokenType::Plus] = { ActionType::Reduce, 27 };
		m_actionTable[34][TokenType::Semicolon] = { ActionType::Reduce, 27 };
		m_actionTable[34][TokenType::Minus] = { ActionType::Reduce, 27 };
		m_actionTable[34][TokenType::Comma] = { ActionType::Reduce, 27 };
		m_actionTable[34][TokenType::Slash] = { ActionType::Reduce, 27 };
		m_actionTable[34][TokenType::Asterisk] = { ActionType::Reduce, 27 };
		m_actionTable[34][TokenType::LeftParen] = { ActionType::Reduce, 27 };
		m_actionTable[37][TokenType::RightParen] = { ActionType::Reduce, 14 };
		m_actionTable[37][TokenType::Plus] = { ActionType::Reduce, 14 };
		m_actionTable[37][TokenType::Semicolon] = { ActionType::Reduce, 14 };
		m_actionTable[37][TokenType::Minus] = { ActionType::Reduce, 14 };
		m_actionTable[37][TokenType::Comma] = { ActionType::Reduce, 14 };
		m_actionTable[37][TokenType::Slash] = { ActionType::Reduce, 14 };
		m_actionTable[37][TokenType::Asterisk] = { ActionType::Reduce, 14 };
		m_actionTable[38][TokenType::Number] = { ActionType::Reduce, 4 };
		m_actionTable[38][TokenType::String] = { ActionType::Reduce, 4 };
		m_actionTable[38][TokenType::RightCurly] = { ActionType::Reduce, 4 };
		m_actionTable[38][TokenType::_EOF] = { ActionType::Reduce, 4 };
		m_actionTable[38][TokenType::Identifier] = { ActionType::Reduce, 4 };
		m_actionTable[38][TokenType::LeftParen] = { ActionType::Reduce, 4 };
		m_actionTable[38][TokenType::LeftCurly] = { ActionType::Reduce, 4 };
		m_actionTable[39][TokenType::RightParen] = { ActionType::Reduce, 18 };
		m_actionTable[39][TokenType::Plus] = { ActionType::Reduce, 18 };
		m_actionTable[39][TokenType::Semicolon] = { ActionType::Reduce, 18 };
		m_actionTable[39][TokenType::Minus] = { ActionType::Reduce, 18 };
		m_actionTable[39][TokenType::Comma] = { ActionType::Reduce, 18 };
		m_actionTable[39][TokenType::Slash] = { ActionType::Reduce, 18 };
		m_actionTable[39][TokenType::Asterisk] = { ActionType::Reduce, 18 };
		m_actionTable[40][TokenType::RightParen] = { ActionType::Reduce, 21 };
		m_actionTable[43][TokenType::RightParen] = { ActionType::Reduce, 23 };
		m_gotoTable[0][NodeType::File] = 1;
		m_gotoTable[0][NodeType::Factor] = 12;
		m_gotoTable[0][NodeType::Identifier] = 2;
		m_gotoTable[0][NodeType::Term] = 6;
		m_gotoTable[0][NodeType::Grouping] = 9;
		m_gotoTable[0][NodeType::Literal] = 8;
		m_gotoTable[0][NodeType::Call] = 11;
		m_gotoTable[0][NodeType::Primary] = 10;
		m_gotoTable[0][NodeType::Expression] = 13;
		m_gotoTable[0][NodeType::VarDeclareStatement] = 14;
		m_gotoTable[0][NodeType::Statement] = 16;
		m_gotoTable[0][NodeType::Block] = 17;
		m_gotoTable[0][NodeType::Statements] = 18;
		m_gotoTable[6][NodeType::TermOp] = 21;
		m_gotoTable[15][NodeType::Statements] = 29;
		m_gotoTable[15][NodeType::Term] = 6;
		m_gotoTable[15][NodeType::Factor] = 12;
		m_gotoTable[15][NodeType::Identifier] = 2;
		m_gotoTable[15][NodeType::Grouping] = 9;
		m_gotoTable[15][NodeType::Literal] = 8;
		m_gotoTable[15][NodeType::Call] = 11;
		m_gotoTable[15][NodeType::Primary] = 10;
		m_gotoTable[15][NodeType::Expression] = 13;
		m_gotoTable[15][NodeType::VarDeclareStatement] = 14;
		m_gotoTable[15][NodeType::Statement] = 16;
		m_gotoTable[15][NodeType::Block] = 17;
		m_gotoTable[7][NodeType::Expression] = 23;
		m_gotoTable[7][NodeType::Term] = 6;
		m_gotoTable[7][NodeType::Factor] = 12;
		m_gotoTable[7][NodeType::Identifier] = 24;
		m_gotoTable[7][NodeType::Grouping] = 9;
		m_gotoTable[7][NodeType::Literal] = 8;
		m_gotoTable[7][NodeType::Call] = 11;
		m_gotoTable[7][NodeType::Primary] = 10;
		m_gotoTable[12][NodeType::FactorOp] = 26;
		m_gotoTable[18][NodeType::Block] = 31;
		m_gotoTable[18][NodeType::Factor] = 12;
		m_gotoTable[18][NodeType::Identifier] = 2;
		m_gotoTable[18][NodeType::Term] = 6;
		m_gotoTable[18][NodeType::Grouping] = 9;
		m_gotoTable[18][NodeType::Literal] = 8;
		m_gotoTable[18][NodeType::Call] = 11;
		m_gotoTable[18][NodeType::Primary] = 10;
		m_gotoTable[18][NodeType::Expression] = 13;
		m_gotoTable[18][NodeType::VarDeclareStatement] = 14;
		m_gotoTable[18][NodeType::Statement] = 16;
		m_gotoTable[19][NodeType::Expression] = 32;
		m_gotoTable[19][NodeType::Term] = 6;
		m_gotoTable[19][NodeType::Factor] = 12;
		m_gotoTable[19][NodeType::Identifier] = 24;
		m_gotoTable[19][NodeType::Grouping] = 9;
		m_gotoTable[19][NodeType::Literal] = 8;
		m_gotoTable[19][NodeType::Call] = 11;
		m_gotoTable[19][NodeType::Primary] = 10;
		m_gotoTable[21][NodeType::Factor] = 33;
		m_gotoTable[21][NodeType::Identifier] = 24;
		m_gotoTable[21][NodeType::Grouping] = 9;
		m_gotoTable[21][NodeType::Literal] = 8;
		m_gotoTable[21][NodeType::Call] = 11;
		m_gotoTable[21][NodeType::Primary] = 10;
		m_gotoTable[25][NodeType::Arguments] = 35;
		m_gotoTable[25][NodeType::Term] = 6;
		m_gotoTable[25][NodeType::Factor] = 12;
		m_gotoTable[25][NodeType::Identifier] = 24;
		m_gotoTable[25][NodeType::Grouping] = 9;
		m_gotoTable[25][NodeType::Literal] = 8;
		m_gotoTable[25][NodeType::Call] = 11;
		m_gotoTable[25][NodeType::Primary] = 10;
		m_gotoTable[25][NodeType::Expression] = 36;
		m_gotoTable[26][NodeType::Primary] = 10;
		m_gotoTable[26][NodeType::Call] = 37;
		m_gotoTable[26][NodeType::Identifier] = 24;
		m_gotoTable[26][NodeType::Grouping] = 9;
		m_gotoTable[26][NodeType::Literal] = 8;
		m_gotoTable[29][NodeType::Block] = 31;
		m_gotoTable[29][NodeType::Factor] = 12;
		m_gotoTable[29][NodeType::Identifier] = 2;
		m_gotoTable[29][NodeType::Term] = 6;
		m_gotoTable[29][NodeType::Literal] = 8;
		m_gotoTable[29][NodeType::Grouping] = 9;
		m_gotoTable[29][NodeType::Call] = 11;
		m_gotoTable[29][NodeType::Primary] = 10;
		m_gotoTable[29][NodeType::Expression] = 13;
		m_gotoTable[29][NodeType::VarDeclareStatement] = 14;
		m_gotoTable[29][NodeType::Statement] = 16;
		m_gotoTable[33][NodeType::FactorOp] = 26;
		m_gotoTable[36][NodeType::ArgRecurse] = 40;
		m_gotoTable[41][NodeType::Expression] = 42;
		m_gotoTable[41][NodeType::Term] = 6;
		m_gotoTable[41][NodeType::Factor] = 12;
		m_gotoTable[41][NodeType::Identifier] = 24;
		m_gotoTable[41][NodeType::Grouping] = 9;
		m_gotoTable[41][NodeType::Literal] = 8;
		m_gotoTable[41][NodeType::Call] = 11;
		m_gotoTable[41][NodeType::Primary] = 10;
		m_gotoTable[42][NodeType::ArgRecurse] = 43;
		m_grammar[0] = { NodeType::File, 1, 1, false };
		m_grammar[1] = { NodeType::Statements, 2, 2, false };
		m_grammar[2] = { NodeType::Statements, 1, 1, false };
		m_grammar[3] = { NodeType::Block, 3, 1, false };
		m_grammar[4] = { NodeType::Block, 2, 1, false };
		m_grammar[5] = { NodeType::Statement, 1, 1, false };
		m_grammar[6] = { NodeType::Statement, 1, 1, false };
		m_grammar[7] = { NodeType::VarDeclareStatement, 3, 2, false };
		m_grammar[8] = { NodeType::Expression, 1, 1, false };
		m_grammar[9] = { NodeType::Term, 3, 3, false };
		m_grammar[10] = { NodeType::Term, 1, 1, false };
		m_grammar[11] = { NodeType::TermOp, 1, 0, true };
		m_grammar[12] = { NodeType::TermOp, 1, 0, true };
		m_grammar[13] = { NodeType::Factor, 3, 3, false };
		m_grammar[14] = { NodeType::Factor, 1, 1, false };
		m_grammar[15] = { NodeType::FactorOp, 1, 0, true };
		m_grammar[16] = { NodeType::FactorOp, 1, 0, true };
		m_grammar[17] = { NodeType::Call, 4, 2, false };
		m_grammar[18] = { NodeType::Call, 1, 1, false };
		m_grammar[19] = { NodeType::Arguments, 0, 0, true };
		m_grammar[20] = { NodeType::Arguments, 2, 2, false };
		m_grammar[21] = { NodeType::ArgRecurse, 0, 0, true };
		m_grammar[22] = { NodeType::ArgRecurse, 3, 2, false };
		m_grammar[23] = { NodeType::Primary, 1, 1, false };
		m_grammar[24] = { NodeType::Primary, 1, 1, false };
		m_grammar[25] = { NodeType::Primary, 1, 1, false };
		m_grammar[26] = { NodeType::Grouping, 3, 1, false };
		m_grammar[27] = { NodeType::Literal, 1, 0, true };
		m_grammar[28] = { NodeType::Literal, 1, 0, true };
		m_grammar[29] = { NodeType::Identifier, 1, 0, true };
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
				exit(-2);
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
