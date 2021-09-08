#pragma once

#include "Token.h"

#include <string>
#include <vector>

class Tokenizer
{
public:
	Tokenizer(const std::string& source);

	std::vector<Token> tokenize();
private:
	std::string m_source;
	int m_start = 0, m_current = 0, m_lineCurrent = 0;
	int m_line = 1;

	std::vector<Token> m_tokens;

	void nextToken();

	void addToken(TokenType type);
	void addToken(TokenType type, std::string token);

	char peek();
	char advance();
	bool match(char matchee);

	void tokenizeNumber();
	void tokenizeString();
	void tokenizeIdentifier();

	bool isNumeric(char val);
	bool isAlpha(char val);
	bool isAtEnd();
};