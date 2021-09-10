#pragma once

#include <vector>
#include <memory>
#include <iostream>

#define FUN_PRINT "print"
#define FUN_PRINTLN "println"
#define FUN_INPUT "input"
#define FUN_TOINT "int"
#define FUN_RANDOM "random"

class Interpreter;
struct Value;
struct Function
{
	int args = 0;
	bool builtIn = true;

	virtual Value call(Interpreter& interpreter, std::vector<Value> values) = 0;
};


struct UserFunction : public Function
{
	UserFunction() { builtIn = false; }
	Value call(Interpreter& interpreter, std::vector<Value> values);
};

enum class BuiltInType
{
	PRINT,
	PRINTLN
};

struct BuiltInFunction : public Function
{

};

// Built in functions
struct Print : public Function
{
	Print() { args = 1; }

	Value call(Interpreter& interpreter, std::vector<Value> values);
};

struct PrintLn : public Function
{
	PrintLn() { args = 1; }

	Value call(Interpreter& interpreter, std::vector<Value> values);
};

struct Input : public Function
{
	Input() { args = 0; }

	Value call(Interpreter& interpreter, std::vector<Value> values);
};

struct ToInt : public Function
{
	ToInt() { args = 1; }

	Value call(Interpreter& interpreter, std::vector<Value> values);
};

struct Random : public Function
{
	Random() { args = 2; }

	Value call(Interpreter& interpreter, std::vector<Value> values);
};