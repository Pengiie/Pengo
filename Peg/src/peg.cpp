#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <queue>
#include <stack>
#include <iostream>
#include <fstream>

enum class SymbolType
{
	TERMINAL, NONTERMINAL
};

struct Symbol
{
	std::string symbol = "";
	SymbolType type = SymbolType::TERMINAL;
	bool trackInExp = false;

	bool operator== (const Symbol& other) const
	{
		return symbol == other.symbol && type == other.type;
	}
};

namespace std {

	template <>
	struct hash<Symbol>
	{
		std::size_t operator()(const Symbol& k) const
		{
			hash<string> hasher;
			return hasher(k.symbol);
		}
	};

}

struct Production
{
	Symbol lhs;
	std::vector<Symbol> rhs;

	bool operator== (const Production& other) const
	{
		return lhs == other.lhs &&
			rhs.size() == other.rhs.size() &&
			std::equal(rhs.begin(), rhs.end(), other.rhs.begin());
	}
};

struct TrackedProduction
{
	Production production;
	int pointer = 0;
	std::unordered_set<Symbol> lookAhead;

	bool isAtEnd() const
	{
		return pointer >= (int)production.rhs.size();
	}

	const Symbol nextSymbol() const
	{
		if (pointer + 1 == production.rhs.size())
			return { "$" };
		return production.rhs[pointer + 1];
	}

	bool operator== (const TrackedProduction& other) const
	{
		return production == other.production &&
			pointer == other.pointer &&
			std::equal(lookAhead.begin(), lookAhead.end(), other.lookAhead.begin());
	}
};

struct ItemSet
{
	int setNumber = -1;
	std::vector<TrackedProduction> productions;
	std::unordered_map<Symbol, int> next;

	void print()
	{
		std::cout << "I" << setNumber << std::endl;
		for (TrackedProduction& tProd : productions)
		{
			Production& prod = tProd.production;
			std::cout << prod.lhs.symbol << " -> ";
			for (int i = 0; i < (int)prod.rhs.size(); i++)
			{
				if (tProd.pointer == i)
					std::cout << "%";
				std::cout << prod.rhs[i].symbol;
			}
			if (tProd.pointer == prod.rhs.size())
				std::cout << "%";
			if (tProd.lookAhead.size() == 0)
			{
				std::cout << " {}" << std::endl;
				continue;
			}
			std::cout << " { ";
			for (Symbol symbol : tProd.lookAhead)
			{
				std::cout << symbol.symbol << " ";
			}
			std::cout << "}" << std::endl;

		}
	}
};

class Grammar
{
public:
	Grammar(const Symbol& start) : m_start(start) {}

	void addProduction(const Production& production)
	{
		m_productions.push_back(production);
	}

	const Symbol& getStart() const { return m_start; }
	const std::vector<Production>& getProductions() const { return m_productions; }
private:
	const Symbol m_start;
	std::vector<Production> m_productions;
};

class Peg
{
public:
	Peg(const Grammar& grammar) : m_grammar(grammar) {}

	void addSubstitution(std::string symbol, std::string tokenType)
	{
		m_typeSubstitution[symbol] = tokenType;
	}

	void generateTables()
	{
		Grammar augmentedGrammar = m_grammar;
		Symbol augmentedStart = { std::string(m_grammar.getStart().symbol) + "'", SymbolType::NONTERMINAL, false };
		m_augmentedProduction = { augmentedStart, {m_grammar.getStart()} };
		augmentedGrammar.addProduction(m_augmentedProduction);

		TrackedProduction startTrackedProduction = { m_augmentedProduction, 0, { {"$" }} };

		// Start generating all the item sets and its closures
		m_itemSets.push_back(createItemSet({ startTrackedProduction }));
		m_setBus.push(0);
		while (!m_setBus.empty())
		{
			createSets(m_setBus.front());
			m_setBus.pop();
		}

		// Generate all the look ahead tokens for the item sents
		m_lookAheadBus.push(std::make_pair(0, std::vector<TrackedProduction>{ startTrackedProduction }));
		while (!m_lookAheadBus.empty())
		{
			generateLookAheads(m_itemSets[m_lookAheadBus.front().first], m_lookAheadBus.front().second);
			m_lookAheadBus.pop();
		}
		for (ItemSet& set : m_itemSets)
			deriveLookAheads(set);

		// Create reductions actions and accept actions where applicable
		defineActions();
		generateTypes();

		for (ItemSet& set : m_itemSets)
		{
			set.print();
			std::cout << std::endl;
		}
		addSubstitution("$", "_EOF");
	}

	void generateParser(std::string fileLocation)
	{
		std::string headerFileName = fileLocation + "Parser.h";
		std::string sourceFileName = fileLocation + "Parser.h";
		std::fstream headerFile, sourceFile;
		headerFile.open(headerFileName, std::fstream::out);
		sourceFile.open(sourceFileName, std::fstream::out);

		if (headerFile.is_open())
		{
			std::string contents =
				"#pragma once\n"
				"#include \"../tokenizer/Token.h\"\n"
				"#include <vector>\n"
				"#include <stack>\n"
				"#include <queue>\n"
				"#include <unordered_map>\n"
				"#include <iostream>\n"
				"#include <algorithm>\n";
			contents.append("#define SET_NUM ").append(std::to_string(m_itemSets.size())).append("\n");
			contents.append("#define GRAMMAR_NUM ").append(std::to_string(m_grammar.getProductions().size())).append("\n");

			contents.append(
				"enum class NodeType\n"
				"{\n"
			);
			for (const std::string& exp : m_expressions)
			{
				contents.append("	").append(exp).append(",\n");
			}
			contents.append(
				"};\n"
				"struct Node\n"
				"{\n"
				"	NodeType type;\n"
				"	bool terminal = false;\n"
				"	Token token;\n"
				"	std::vector<Node> children;\n"
				"};\n"
				"enum class ActionType\n"
				"{\n"
				"	None,\n"
				"	Shift,\n"
				"	Reduce,\n"
				"	Accept\n"
				"};\n"
				"struct Action\n"
				"{\n"
				"	ActionType type = ActionType::None;\n"
				"	int value = -1;\n"
				"};\n"
				"struct Production\n"
				"{\n"
				"	NodeType lhs;\n"
				"	int rhs = 0;\n"
				"	int rhsNodes = 0;\n"
				"	bool terminates = false;\n"
				"};\n"
				"class Parser\n"
				"{\n"
				"public:\n"
				"	Parser()\n"
				"	{\n"
			);
			for (auto statePair : m_action)
			{
				int num = statePair.first;
				for (auto tokenPair : m_action[num])
				{
					std::string token = getSymbolName(tokenPair.first);
					char actionType = tokenPair.second[0];
					std::string action;
					switch (actionType)
					{
					case 'A':
						action = "{ ActionType::Accept }; ";
						break;
					case 'S':
						action = std::string("{ ActionType::Shift, ").append(tokenPair.second.substr(1)).append(" }");
						break;
					case 'R':
						action = std::string("{ ActionType::Reduce, ").append(tokenPair.second.substr(1)).append(" }");
						break;
					}
					contents.append("		m_actionTable[").append(std::to_string(num)).append("][TokenType::").append(token).append("] = ").append(action).append(";\n");
				}
			}
			for (auto statePair : m_goto)
			{
				int num = statePair.first;
				for (auto gotoPair : m_goto[num])
				{
					std::string token = getSymbolName(gotoPair.first);
					contents.append("		m_gotoTable[").append(std::to_string(num)).append("][NodeType::").append(token).append("] = ").append(std::to_string(gotoPair.second)).append(";\n");
				}
			}
			for (int i = 0; i < (int)m_grammar.getProductions().size(); i++)
			{
				Production p = m_grammar.getProductions()[i];
				bool terminates = true;
				int nonTerminals = 0;
				for (const Symbol& symbol : p.rhs)
					if (symbol.type == SymbolType::NONTERMINAL)
					{
						terminates = false;
						nonTerminals++;
					}
				// mak eif you add a new token to a set and it adds onto the set it removes the links if it was changed because the new lookaheads were new
				contents.append("		m_grammar[").append(std::to_string(i)).append("] = { NodeType::").append(p.lhs.symbol).
					append(", ").append(std::to_string(p.rhs.size())).append(", ").
					append(std::to_string(nonTerminals)).append(", ").
					append(terminates ? "true" : "false").append(" };\n");
			}
			contents.append(
				"	}\n"

				"	Node parse(std::vector<Token> tokens)\n"
				"	{\n"
				"		std::queue<Token, std::deque<Token>> input(std::deque<Token>(tokens.begin(), tokens.end()));\n"
				"		std::stack<int> stack;\n"
				"		stack.push(0);\n"
				"		std::stack<Node> output;\n"
				"		Token beforeToken = input.front();\n"
				"		int state = 0;\n"
				"		while (true)\n"
				"		{\n"
				"			state = stack.top();\n"
				"			Token& current = input.front();\n"
				"			Action& nextAction = m_actionTable[state][current.type];\n"
				"			if (nextAction.type == ActionType::None)\n"
				"			{\n"
				"				if (current.type == TokenType::_EOF)\n"
				"					std::cout << \"Unexpected end of file at line \" << current.line << \" at pos \" << current.pos << std::endl;\n"
				"				else\n"
				"					std::cout << \"Error parsing line \" << current.line << \" at pos \" << current.pos << std::endl;\n"
				"				exit(-2);\n"
				"			}\n"
				"			else if (nextAction.type == ActionType::Shift)\n"
				"			{\n"
				"				beforeToken = input.front();\n"
				"				input.pop();\n"
				"				stack.push(nextAction.value);\n"
				"			}\n"
				"			else if (nextAction.type == ActionType::Reduce)\n"
				"			{\n"
				"				// Push the rule to the stack\n"
				"				Production production = m_grammar[nextAction.value - 1];\n"
				"				for (int i = 0; i < production.rhs; i++)\n"
				"					stack.pop();\n"
				"				stack.push(m_gotoTable[stack.top()][production.lhs]);\n"
				"				Node node;\n"
				"				node.terminal = production.terminates;\n"
				"				node.type = production.lhs;\n"
				"				if (production.terminates)\n"
				"					node.token = beforeToken;\n"
				"				else\n"
				"				{\n"
				"					for (int i = 0; i < production.rhsNodes; i++)\n"
				"					{\n"
				"						node.children.push_back(output.top());\n"
				"						output.pop();\n"
				"					}\n"
				"				}\n"
				"				output.push(node);\n"
				"			}\n"
				"			else if (nextAction.type == ActionType::Accept)\n"
				"			{\n"
				"				break;\n"
				"			}\n"
				"		}\n"
				"		Node root = output.top();\n"
				"		std::stack<Node*> toReverse({ &root });\n"
				"		while (!toReverse.empty())\n"
				"		{\n"
				"			Node* node = toReverse.top();\n"
				"			toReverse.pop();\n"
				"			for (Node& child : node->children)\n"
				"				toReverse.push(&child);\n"
				"			std::reverse(node->children.begin(), node->children.end());\n"
				"		}\n"
				"		return root;\n"
				"	}\n"
				"private:\n"
				"	Production m_grammar[GRAMMAR_NUM];\n"
				"	std::unordered_map<TokenType, Action> m_actionTable[SET_NUM];\n"
				"	std::unordered_map<NodeType, int> m_gotoTable[SET_NUM];\n"
				"};\n"
			);

			headerFile << contents.c_str();
		}

		headerFile.close();
		sourceFile.close();
	}
private:
	const Grammar m_grammar;
	Production m_augmentedProduction;
	std::vector<ItemSet> m_itemSets;
	std::unordered_map<int, std::unordered_map<std::string, std::string>> m_action;
	std::unordered_map<int, std::unordered_map<std::string, int>> m_goto;
	std::unordered_set<std::string> m_expressions;
	std::unordered_set<std::string> m_tokens;
	std::unordered_map<std::string, std::string> m_typeSubstitution;
	std::queue<int> m_setBus;

	ItemSet createItemSet(std::vector<TrackedProduction> productions)
	{
		ItemSet itemSet;
		itemSet.setNumber = m_itemSets.size();
		itemSet.productions.insert(itemSet.productions.end(), productions.begin(), productions.end());
		std::vector<TrackedProduction>* p = new std::vector<TrackedProduction>();
		std::vector<TrackedProduction> closures = generateClosures(productions, p);
		delete p;
		itemSet.productions.insert(itemSet.productions.end(), closures.begin(), closures.end());
		return itemSet;
	}

	std::string getSymbolName(const std::string& symbol)
	{
		if (m_typeSubstitution.find(symbol) != m_typeSubstitution.end())
			return m_typeSubstitution[symbol];
		return symbol;
	}

	void defineActions()
	{
		for (ItemSet& set : m_itemSets)
		{
			for (TrackedProduction& production : set.productions)
			{
				if (production.isAtEnd())
				{
					if (production.production == m_augmentedProduction)
					{
						for (Symbol lookAhead : production.lookAhead)
							m_action[set.setNumber][lookAhead.symbol] = "Accept";
					}
					else
					{
						for (int i = 0; i < (int)m_grammar.getProductions().size(); i++)
						{
							if (production.production == (Production)m_grammar.getProductions()[i])
							{
								for (Symbol lookAhead : production.lookAhead)
									m_action[set.setNumber][lookAhead.symbol] = std::string("R") + std::to_string(i + 1);
								break;
							}
						}
					}
				}
			}
		}
		/*
				for (ItemSet set : m_itemSets)
				{
					if (set.next.empty())
					{
						if (set.productions.size() == 1)
						{
							if (set.productions[0].production == m_augmentedProduction)
							{
								for (Symbol lookAhead : set.productions[0].lookAhead)
									m_action[set.setNumber][lookAhead.symbol] = "Accept";
							}
							else
							{
								for (int i = 0; i < (int)m_grammar.getProductions().size(); i++)
								{
									if (set.productions[0].production == (Production)m_grammar.getProductions()[i])
									{
										for (Symbol lookAhead : set.productions[0].lookAhead)
											m_action[set.setNumber][lookAhead.symbol] = std::string("R") + std::to_string(i + 1);
										break;
									}
								}
							}
						}
					}
				}*/
	}

	void createSets(int setNum)
	{
		// Collects all productions where the pointer can be advanced and assign to its respective left side nonterminal symbol
		std::unordered_map<Symbol, std::vector<TrackedProduction>> nextProductions;
		for (TrackedProduction& prod : m_itemSets[setNum].productions)
		{
			if (!prod.isAtEnd())
			{
				Symbol& symbol = prod.production.rhs[prod.pointer];
				if (nextProductions.find(symbol) == nextProductions.end())
				{
					nextProductions[symbol] = { prod };
				}
				else
				{
					nextProductions[symbol].push_back(prod);
				}
			}
		}
		// For each symbol find the productions, increment the pointer, and generate an item set for that
		// If an item set already exists then just point to the pre existing item set
		for (auto symbol : nextProductions)
		{
			for (TrackedProduction& production : symbol.second)
			{
				production.pointer++;
			}
			ItemSet newSet = createItemSet(symbol.second);
			bool setExists = false;
			for (ItemSet& dupSet : m_itemSets)
			{
				if (dupSet.productions.size() == newSet.productions.size())
				{
					bool equal = true;
					for (int i = 0; i < (int)newSet.productions.size(); i++)
					{
						if (dupSet.productions[i].production == newSet.productions[i].production &&
							dupSet.productions[i].pointer == newSet.productions[i].pointer)
							continue;
						equal = false;
						break;
					}
					if (equal)
					{
						for (int i = 0; i < (int)newSet.productions.size(); i++)
						{
							for (Symbol look : newSet.productions[i].lookAhead)
								dupSet.productions[i].lookAhead.insert(look);
						}
						newSet = dupSet;
						setExists = true;
					}
				}
			}
			if (!setExists)
			{
				m_itemSets.push_back(newSet);
				m_setBus.push(newSet.setNumber);
			}
			m_itemSets[setNum].next[symbol.first] = newSet.setNumber;
			// If its a nonterminal create a goto entry
			// If its a terminal create a shift action entry
			if (symbol.first.type == SymbolType::NONTERMINAL)
			{
				m_goto[setNum][symbol.first.symbol] = newSet.setNumber;
			}
			else if (symbol.first.type == SymbolType::TERMINAL)
			{
				m_action[setNum][symbol.first.symbol] = std::string("S") + std::to_string(newSet.setNumber);
			}
		}
	}

	std::vector<TrackedProduction> removeAndCollectClosures(std::vector<TrackedProduction> closures)
	{
		std::vector<TrackedProduction> newClosures;
		for (TrackedProduction& closure : closures)
		{
			bool found = false;
			for (TrackedProduction& prod : newClosures)
			{
				if (prod.pointer == closure.pointer && prod.production == closure.production)
				{
					for (Symbol lookAhead : closure.lookAhead)
					{
						if (prod.lookAhead.find(lookAhead) == prod.lookAhead.end())
							prod.lookAhead.insert(lookAhead);
					}
					found = true;
				}
			}
			if (!found)
				newClosures.push_back(closure);
		}
		return newClosures;
	}

	std::unordered_map<int, std::unordered_set<int>> m_lookAheadLinks;
	std::queue<std::pair<int, std::vector<TrackedProduction>>> m_lookAheadBus;
	void generateLookAheads(ItemSet& set, std::vector<TrackedProduction> startProdutions)
	{
		// Find the next symbols that need look ahead closures
		std::stack<Symbol> next;
		for (TrackedProduction& prod : set.productions)
		{
			for (TrackedProduction& start : startProdutions)
			{
				if (start.pointer == prod.pointer && start.production == prod.production)
				{
					if (!prod.isAtEnd())
						next.push(prod.production.rhs[prod.pointer]);
				}
			}
		}

		// Iterate over look ahead closures
		while (!next.empty())
		{
			Symbol nSymbol = next.top();
			next.pop();
			for (TrackedProduction& prod : set.productions)
			{
				if (!prod.isAtEnd())
					if (prod.production.rhs[prod.pointer] == nSymbol)
					{
						for (TrackedProduction& cursorProd : set.productions)
						{
							if (cursorProd.production.lhs == nSymbol)
							{
								// Finds the production to create look ahead closure for
								bool hasAll = true;
								// If look ahead of current token will be the end, just transfer all the look aheads of the current to the next
								if (prod.pointer + 1 == prod.production.rhs.size())
									for (Symbol s : prod.lookAhead)
									{
										if (cursorProd.lookAhead.find(s) == cursorProd.lookAhead.end())
										{
											cursorProd.lookAhead.insert(s);
											hasAll = false;
										}
									}
								else if (cursorProd.lookAhead.find(prod.nextSymbol()) == cursorProd.lookAhead.end())
								{
									// Adds look ahead of the current token to the next token rules
									cursorProd.lookAhead.insert(prod.nextSymbol());
									hasAll = false;
								}
								// Push the current pointed token to create  look ahead closures
								if (!cursorProd.isAtEnd() && !hasAll)
									if (cursorProd.production.rhs[cursorProd.pointer].type == SymbolType::NONTERMINAL)
										next.push(cursorProd.production.rhs[cursorProd.pointer]);
							}
						}
					}
			}
		}
		// For each item set, determine the productions where the pointer was advanced and copy the look ahead tokens to them
		// Then add to the look ahead bus the item set with new tokens to update any new look ahead closures
		for (auto& pair : set.next)
		{
			if (m_lookAheadLinks.find(set.setNumber) != m_lookAheadLinks.end() && m_lookAheadLinks[set.setNumber].find(pair.second) != m_lookAheadLinks[set.setNumber].end())
				continue;
			ItemSet& nextSet = m_itemSets[pair.second];
			std::vector<TrackedProduction> nextProductions;
			bool changed = false;
			for (TrackedProduction& prod : nextSet.productions)
			{
				if (prod.pointer > 0)
				{
					TrackedProduction comparison = prod;
					comparison.pointer--;
					for (TrackedProduction& tracked : set.productions)
					{
						if (tracked.pointer == comparison.pointer && tracked.production == comparison.production)
						{
							for (const Symbol& lookSymbol : tracked.lookAhead)
							{
								if (prod.lookAhead.find(lookSymbol) == prod.lookAhead.end())
									changed = true;
								prod.lookAhead.insert(lookSymbol);
							}
							nextProductions.push_back(prod);
							break;
						}
					}
				}
			}
			if (changed)
			{
				m_lookAheadLinks[pair.second] = std::unordered_set<int>();
			}
			if (m_lookAheadLinks.find(set.setNumber) == m_lookAheadLinks.end())
				m_lookAheadLinks[set.setNumber] = std::unordered_set<int>();
			m_lookAheadLinks[set.setNumber].insert(pair.second);
			std::cout << set.setNumber << " -> " << pair.second << std::endl;
			m_lookAheadBus.push(std::make_pair(pair.second, nextProductions));
		}
	}

	void deriveLookAheads(ItemSet& set)
	{
		for (TrackedProduction& prod : set.productions)
		{
			std::stack<Symbol, std::vector<Symbol>> aheadStack(std::vector<Symbol>(prod.lookAhead.begin(), prod.lookAhead.end()));
			std::unordered_set<Symbol> finalLookAhead;
			std::unordered_set<Symbol> checked;
			while (!aheadStack.empty())
			{
				Symbol s = aheadStack.top();
				checked.insert(s);
				aheadStack.pop();
				if (s.type == SymbolType::NONTERMINAL)
				{
					for (const Production& prodRule : m_grammar.getProductions())
					{
						if (prodRule.lhs == s)
						{
							if(prodRule.rhs.size() > 0)
								if (checked.find(prodRule.rhs[0]) == checked.end())
									aheadStack.push(prodRule.rhs[0]);
							//for (const Symbol& ss : prodRule.rhs)
						//		if(checked.find(ss) == checked.end())
							//		aheadStack.push(ss);
						}
					}
				}
				else if (s.type == SymbolType::TERMINAL)
				{
					finalLookAhead.insert(s);
				}
			}
			prod.lookAhead = finalLookAhead;
		}
	}

	std::vector<TrackedProduction> generateClosures(std::vector<TrackedProduction> prods, std::vector<TrackedProduction>* alreadyHad)
	{
		std::vector<TrackedProduction>* closures = new std::vector<TrackedProduction>();
		for (const TrackedProduction& production : prods)
		{
			if (!production.isAtEnd())
			{
				Symbol pointerSymbol = production.production.rhs[production.pointer];
				//std::vector<Symbol> lookAheads = { production.nextSymbol() };
				// TODO FIGURE OUT LOOK AHEADS AFTER MAKING CLOSURES
				if (pointerSymbol.type == SymbolType::NONTERMINAL)
				{
					for (const Production& production : m_grammar.getProductions())
					{
						if (production.lhs == pointerSymbol)
						{
							TrackedProduction tracked = { production, 0, {} };
							bool found = false;
							for (TrackedProduction& prodFind : *alreadyHad)
							{
								if (prodFind == tracked)
								{
									found = true;
									break;
								}
							}
							if (!found)
							{
								alreadyHad->push_back(tracked);
								std::vector<TrackedProduction> productions = generateClosures({ tracked }, alreadyHad);
								closures->push_back(tracked);
								closures->insert(closures->begin(), productions.begin(), productions.end());

							}
						}
					}
				}
			}
		}
		std::vector<TrackedProduction> productions = *closures;
		delete closures;
		return productions;
	}

	void generateTypes()
	{
		for (const Production& production : m_grammar.getProductions())
		{
			m_expressions.insert(production.lhs.symbol);
			for (const Symbol& symbol : production.rhs)
			{
				if (symbol.trackInExp)
				{
					m_tokens.insert(symbol.symbol);
				}
			}
		}
	}
};

#define PARSER_LOCATION "C:/Dev/cpp/Pengo/Pengo/src/parser/"
//#define PARSER_LOCATION "/"

int main()
{
	/*
	# Using EBNF to define context free grammar
	# Implied whitespace between any terminal or nonterminal where apparent

	file = statements;
	statements = statements block | block;

	statement = printStatement | varDeclare | expression;
	printStatement = 'print' expression;
	varDeclare = 'var' 'Identifier' '=' expression;

	block = '{' statements '}';
	block = statement, ';';

	expression = term;

	term = term, termOp, factor | factor;
	termOp = +;
	termOp = -;
	factor = factor, factorOp, unary | unary;
	factorOp = *;
	factorOp = /;
	unary = unaryOp, unary | primary;
	unaryOp = -;
	unaryOp = !;

	call -> primary, '(', arguments, ')';
	call -> primary;
	arguments = ;
	arguments = expression, argRecurse;
	argRecurse = ',', expression, argRecurse;
	argRecurse = ;

	primary = identifier | grouping | literal | call;

	grouping = '(', expression, ')';
	literal = NUMBER | STRING | 'true' | 'false';

	identifier = IDENTIFIER
	*/

	Symbol file = { "File", SymbolType::NONTERMINAL, true };
	Symbol statements = { "Statements", SymbolType::NONTERMINAL, true };

	Symbol statement = { "Statement", SymbolType::NONTERMINAL, true };

	Symbol block = { "Block", SymbolType::NONTERMINAL, true };

	//Symbol printStatement = { "PrintStatement", SymbolType::NONTERMINAL, true };
	Symbol varDeclareStatement = { "VarDeclareStatement", SymbolType::NONTERMINAL, true };

	Symbol expression = { "Expression", SymbolType::NONTERMINAL, true };

	Symbol term = { "Term", SymbolType::NONTERMINAL, true };
	Symbol termOp = { "TermOp", SymbolType::NONTERMINAL, true };

	Symbol factor = { "Factor", SymbolType::NONTERMINAL, true };
	Symbol factorOp = { "FactorOp", SymbolType::NONTERMINAL, true };

	Symbol unary = { "Unary", SymbolType::NONTERMINAL, true };
	Symbol unaryOp = { "UnaryOp", SymbolType::NONTERMINAL, true };

	Symbol call = { "Call", SymbolType::NONTERMINAL, true };
	Symbol arguments = { "Arguments", SymbolType::NONTERMINAL, true };
	Symbol argRecurse = { "ArgRecurse", SymbolType::NONTERMINAL, true };

	Symbol primary = { "Primary", SymbolType::NONTERMINAL, true };
	Symbol grouping = { "Grouping", SymbolType::NONTERMINAL, true };
	Symbol identifier = { "Identifier", SymbolType::NONTERMINAL, true };
	Symbol lit = { "Literal", SymbolType::NONTERMINAL, true };

	Grammar langGrammar(file);
	langGrammar.addProduction({ file, {statements} });
	langGrammar.addProduction({ statements, {statements, block} });
	langGrammar.addProduction({ statements, {block} });

	langGrammar.addProduction({ block, {{"{"}, statements, {"}"}} });
	langGrammar.addProduction({ block, {statement, {";"}} });

	langGrammar.addProduction({ statement, {expression} });
	//langGrammar.addProduction({ statement, {printStatement} });
	langGrammar.addProduction({ statement, {varDeclareStatement} });

	//langGrammar.addProduction({ printStatement, {{"Print"}, expression} });
	langGrammar.addProduction({ varDeclareStatement, {identifier, {"="}, expression} });

	langGrammar.addProduction({ expression, {term} });
	langGrammar.addProduction({ term, {term, termOp, factor} });
	langGrammar.addProduction({ term, {factor} });
	langGrammar.addProduction({ termOp, {{"+"}} });
	langGrammar.addProduction({ termOp, {{"-"}} });
	langGrammar.addProduction({ factor, {factor, factorOp, call} });
	langGrammar.addProduction({ factor, {call} });
	langGrammar.addProduction({ factorOp, {{"*"}} });
	langGrammar.addProduction({ factorOp, {{"/"}} });
	//langGrammar.addProduction({ unary, {unaryOp, unary} });
	//langGrammar.addProduction({ unary, {primary} });
	//langGrammar.addProduction({ unaryOp, {{"-"}}});
	//langGrammar.addProduction({ unaryOp, {{"!"}} });

	langGrammar.addProduction({ call, {primary, {"("}, arguments, {")"}} });
	langGrammar.addProduction({ call, {primary} });
	langGrammar.addProduction({ arguments, {} });
	langGrammar.addProduction({ arguments, {expression, argRecurse} });
	langGrammar.addProduction({ argRecurse, {} });
	langGrammar.addProduction({ argRecurse, {{","}, expression, argRecurse} });

	langGrammar.addProduction({ primary, {identifier} });
	langGrammar.addProduction({ primary, {grouping} });
	langGrammar.addProduction({ primary, {lit} });
	langGrammar.addProduction({ grouping, {{"("}, expression, {")"}} });
	langGrammar.addProduction({ lit, {{"Number"}} });
	langGrammar.addProduction({ lit, {{"String"}} });
	langGrammar.addProduction({ identifier, {{"Identifier"}} });
	//langGrammar.addProduction({ lit, {{"Bool"}} });

	/*
	S -> N + N
	S -> N - N
	N -> 1
	Symbol S = { "S", SymbolType::NONTERMINAL, true};
	Symbol N = { "N", SymbolType::NONTERMINAL, true};
	Grammar basicGrammar(S);
	basicGrammar.addProduction({ S, {N, {"+"}, N} });
	basicGrammar.addProduction({ S, {N, {"-"}, N} });
	basicGrammar.addProduction({ N, {{"1", SymbolType::TERMINAL, true}} });
	*/

	Peg peg(langGrammar);
	peg.addSubstitution("+", "Plus");
	peg.addSubstitution("-", "Minus");
	peg.addSubstitution("*", "Asterisk");
	peg.addSubstitution("/", "Slash");
	peg.addSubstitution("!", "Bang");
	peg.addSubstitution("(", "LeftParen");
	peg.addSubstitution(")", "RightParen");
	peg.addSubstitution(";", "Semicolon");
	peg.addSubstitution("=", "Equal");
	peg.addSubstitution("{", "LeftCurly");
	peg.addSubstitution("}", "RightCurly");
	peg.addSubstitution(",", "Comma");
	peg.generateTables();
	std::cin.get();
	peg.generateParser(PARSER_LOCATION);
}

/*
#pragma once

#include "../tokenizer/Token.h"
#include <vector>
#include <stack>
#include <queue>
#include <iostream>

#define SET_NUM HERE
#define GRAMMAR_NUM HERE

struct Expression
{
	enum class ExpressionType
	{
		TYPES HERE
	};

	struct Visitor
	{
		// Add expressions here as const functions
	};
	virtual void accept(const Visitor& visitor) = 0;

	// Add inner expression structs here
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
	TokenType lhs;
	std::vector<TokenType> rhs;
};

class Parser
{
public:
	Parser()
	{
	// INSERT GRAMMAR INIT DATA HERE AND OTHER DATA SUCH AS TABLE DATA;
	}

	Expression parse(const std::vector<Token>& tokens)
	{
		std::queue<Token, std::deque<Token>> input(std::deque<Token>(tokens.begin(), tokens.end()));
		std::stack<int> stack = { 0 };
		std::vector<std::pair<int, Token>> output;

		// Runs until output is accepted
		int state = 0;
		while(True)
		{
			state = stack.top();
			Token& current = input.front();
			Action& nextAction = m_actionTable[state][current.type];
			if(nextAction.type == ActionType::None)
			{
				std::cout << "Error parsing line " << token.line << std::endl;
				exit(-2);
			}
			else if(nextAction.type == ActionType::Shift)
			{
				input.pop();
				stack.push(nextAction.value);
			}
			else if(nextAction.type == ActionType::Reduce)
			{
				// Push the rule to the stack
				output.push_back(std::make_pair(nextAction.value, current));
				Production production = grammar[nextAction.value-1];
				TokenType leftToken = production.lhs;
				for(int i = 0; i < production.rhs.size(); i++)
					stack.pop();
				stack.push(m_gotoTable[state][production.lhs]);
			}
			else if(nextAction.type == ActionType::Accept)
			{
				break;
			}
		}
	}
private:
	Production m_grammar[GRAMMAR_NUM];
	std::unordered_map<TokenType, Action> m_actionTable[SET_NUM];
	std::unordered_map<TokenType, int> m_gotoTable[SET_NUM];
};
*/