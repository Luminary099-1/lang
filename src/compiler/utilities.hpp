#pragma once

#include "syntaxTree/base.hpp"

#include <string_view>
#include <forward_list>
#include <map>
#include <ostream>

// Forward declarations to accommodate cyclic includes.
struct Symbol;
struct TokenInfo;


struct TUBuffer
{
	char* _buf;
	size_t _size;
};




struct ScopeStack
{
protected:
	// A stack of scopes that map identifiers to AST definitions.
	std::forward_list<std::map<std::string_view, Symbol*>> _stackMap;

public:
	/**
	 * @brief Begin a new scope in the scope stack.
	 */
	void Enter();

	/**
	 * @brief Exit the current scope in the scope stack.
	 */
	void Exit();

	/**
	 * @brief Attempts to define a new symbol in the scope stack in the current
	 * scope.
	 * 
	 * @param name The symbol's identifier (name).
	 * @param node The AST node referred to by the symbol.
	 * @return A pointer to the already defined AST node referred to by name. If
	 * there isn't a name collision, nullptr is returned.
	 */
	Symbol* Define(std::string_view name, Symbol* node);

	/**
	 * @brief Returns a pointer to the AST node referred to by the specfied
	 * symbol, if it exists in any scope. The most recently defined instance of
	 * the symbol will be returned.
	 * 
	 * @param name The symbol's identifier (name).
	 * @return Symbol* A pointer to the AST node of the already defined symbol.
	 * If no such symbol is define, nullptr is returned.
	 */
	Symbol* Lookup(std::string_view name);
};
