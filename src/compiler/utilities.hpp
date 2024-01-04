#pragma once

#include "syntaxTree/base.hpp"

#include <string_view>
#include <stack>
#include <map>


struct ScopeStack
{
	// A stack of scopes that map identifiers to AST definitions.
	std::stack<std::map<std::string_view, SyntaxTreeNode*>> _stackMap;

	/**
	 * @brief Begin a new scope in the scope stack.
	 */
	void Enter();

	/**
	 * @brief Exit the current scope in the scope stack.
	 */
	void Exit();

	/**
	 * @brief Attempts to define a new symbol in the scope stack.
	 * 
	 * @param name The symbol's identifier (name).
	 * @param node The AST node referred to by the symbol.
	 * @return true if the symbol was successfully define; false otherwise.
	 */
	bool Define(std::string_view name, SyntaxTreeNode* node);

	/**
	 * @brief Returns a pointer to the AST node referred to by the specfied
	 * symbol, if it exists.
	 * 
	 * @param name The symbol's identifier (name).
	 * @return SyntaxTreeNode* A pointer to the AST node of the already defined
	 * symbol.
	 */
	SyntaxTreeNode* Lookup(std::string_view name);
};
