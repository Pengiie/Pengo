#pragma once

#include "Tokenizer.h"
#include <unordered_map>
#include <iostream>

const std::unordered_map<std::string, TokenType> keywords = {
	{"true", TokenType::Bool},
	{"false", TokenType::Bool},

	{"if", TokenType::If},
	{"elif", TokenType::ElseIf},
	{"else", TokenType::Else},
	{"while", TokenType::While},
	{"func", TokenType::Func},

	{"return", TokenType::Return},
	{"break", TokenType::Break},
	{"continue", TokenType::Continue},
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
	case '+': addToken(TokenType::Plus); break;
	case '-': addToken(TokenType::Minus); break;
	case '*': addToken(TokenType::Asterisk); break;
	case '/': if (match('/')) { while (!match('\n') && !isAtEnd()) advance(); break; }
			else addToken(TokenType::Slash); break;
	case '(': addToken(TokenType::LeftParen); break;
	case ')': addToken(TokenType::RightParen); break;
	case '{': addToken(TokenType::LeftCurly); break;
	case '}': addToken(TokenType::RightCurly); break;
	case ',': addToken(TokenType::Comma); break;
	case ';': addToken(TokenType::Semicolon); break;
	case '=': if (match('=')) addToken(TokenType::EqualEqual); else addToken(TokenType::Equal); break;
	case '!': if (match('=')) addToken(TokenType::NotEqual); else addToken(TokenType::Bang); break;
	case '>': if (match('=')) addToken(TokenType::GreaterThanEqual); else addToken(TokenType::GreaterThan); break;
	case '<': if (match('=')) addToken(TokenType::LessThanEqual); else addToken(TokenType::LessThan); break;
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
		std::cout << "Could not parse at line " << m_line << " position " << m_lineCurrent << "!" << std::endl;
		exit(-51);
	}
}

void Tokenizer::tokenizeNumber()
{
	while (isNumeric(peek())) advance();
	if (match('.'))
	{
		while (isNumeric(peek())) advance();
		addToken(TokenType::Float);
	}
	else addToken(TokenType::Integer);
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
	size_t pos;
	while ((pos = token.find('\\')) != std::string::npos) {
		if (token.size() >= pos+1 && token.substr(pos, 2) == "\\n")
			token.replace(pos, 2, "\n");
	}
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