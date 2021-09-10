#pragma once

#include <string>

enum class TokenType
{
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
	Number,
	Bool,

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