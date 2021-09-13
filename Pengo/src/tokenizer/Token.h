#pragma once

#include <string>

enum class TokenType
{
	Func,
	If,
	ElseIf,
	Else,
	While,

	Return,
	Break,
	Continue,

	Bang,
	Plus,
	Minus,
	Asterisk,
	Slash,
	Equal,

	RightParen,
	LeftParen,
	RightCurly,
	LeftCurly,
	Comma,

	Identifier,
	String,
	Integer,
	Float,
	Bool,

	AndAnd,
	OrOr,

	EqualEqual,
	NotEqual,
	GreaterThan,
	LessThan,
	GreaterThanEqual,
	LessThanEqual,

	Semicolon,

	_EOF
};

struct Token
{
	TokenType type = TokenType::_EOF;
	std::string token;
	int line;
	int pos;
};