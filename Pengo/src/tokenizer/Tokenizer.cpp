#pragma once

#include "Tokenizer.h"
#include <unordered_map>

const std::unordered_map<std::string, TokenType> keywords = {
	{"print", TokenType::Print},
	{"var", TokenType::Var}
};

Tokenizer::Tokenizer(const std::string& source) : m_source(source)
{ }

std::vector<Token> Tokenizer::tokenize()
{
	while (!isAtEnd())
	{
		m_start = m_current;
		nextToken();
	}
	addToken(TokenType::_EOF);
	return m_tokens;
}

void Tokenizer::nextToken()
{
	char next = advance();

	switch (next)
	{
	case ' ': break;
	case '\t': break;
	case '\r': break;
	case '\n': m_line++; m_lineCurrent = 0; break;
	case '!': addToken(TokenType::Bang); break;
	case '+': addToken(TokenType::Plus); break;
	case '-': addToken(TokenType::Minus); break;
	case '*': addToken(TokenType::Asterisk); break;
	case '/': addToken(TokenType::Slash); break;
	case '=': addToken(TokenType::Equal); break;
	case '(': addToken(TokenType::LeftParen); break;
	case ')': addToken(TokenType::RightParen); break;
	case ';': addToken(TokenType::Semicolon); break;
	case '"': tokenizeString(); break;
	default:
		if (isNumeric(next))
		{
			tokenizeNumber();
			break;
		}
		if (isAlpha(next))
		{
			tokenizeIdentifier();
			break;
		}
		exit(-51);
	}
}

void Tokenizer::tokenizeNumber()
{
	while (isNumeric(peek())) advance();

	addToken(TokenType::Number);
}

void Tokenizer::tokenizeString()
{
	while (peek() != '"' && !isAtEnd()) {
		if (peek() == '\n') {
			m_line++;
			m_lineCurrent = 0;
		}
		advance();
	}
	advance();
	std::string token = m_source.substr(m_start+1, (m_current - m_start)-2);
	addToken(TokenType::String, token);
}

void Tokenizer::tokenizeIdentifier()
{
	advance();
	while (isAlpha(peek()) || isNumeric(peek())) advance();

	std::string token = m_source.substr(m_start, m_current - m_start);
	if (keywords.find(token) != keywords.end())
	{
		addToken(keywords.at(token));
	}
	else
	{
		addToken(TokenType::Identifier, token);
	}
}

void Tokenizer::addToken(TokenType type)
{
	addToken(type, m_source.substr(m_start, m_current - m_start));
}

void Tokenizer::addToken(TokenType type, std::string token)
{
	m_tokens.push_back({ type, token, m_line, m_lineCurrent });
}

char Tokenizer::peek()
{
	return m_source[m_current];
}

char Tokenizer::advance()
{
	m_lineCurrent++;
	return m_source[m_current++];
}

bool Tokenizer::match(char matchee)
{
	if (isAtEnd())
		return false;
	if (m_source[m_current] == matchee)
	{
		m_lineCurrent++;
		m_current++;
		return true;
	}
	return false;
}

bool Tokenizer::isNumeric(char val)
{
	return '0' <= val && val <= '9';
}

bool Tokenizer::isAlpha(char val)
{
	return ('a' <= val && val <= 'z') ||
		('A' <= val && val <= 'Z') ||
		val == '_';
}

bool Tokenizer::isAtEnd()
{
	return m_current >= (int) m_source.size();
}