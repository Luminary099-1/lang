#pragma once

#include <forward_list>
#include <map>


/**
 * @brief A data structure to store program symbols in a scoped hierarchy.
 * 
 * @tparam I Type of identifiers in the scoped hierarchy.
 * @tparam D Type of the declarations in the scoped hierarchy.
 */
template<typename I, typename D>
struct SymbolTable
{
protected:
	// A stack of scopes that map identifiers to AST definitions.
	std::forward_list<std::map<I, D*>> _stackMap;

public:
	/**
	 * @brief Mark the beginning of a new scope.
	 */
	void Enter();

	/**
	 * @brief Mark the end of a new scope.
	 */
	void Exit();

	/**
	 * @brief Attempts to define a new symbol in the current scope.
	 * 
	 * @param name The symbol's identifier (name).
	 * @param node The AST node referred to by the symbol.
	 * @return AST node that defines the symbol. If no such symbol is defined,
	 * nullptr is returned.
	 */
	D* Define(I name, D* node);

	/**
	 * @brief Returns a pointer to the AST node referred to by the specfied
	 * symbol, if it exists in any scope. The most recently defined instance of
	 * @param name The symbol's identifier (name).
	 * the symbol will be returned.
	 * 
	 * @return The AST node that defines the symbol. If no such symbol is
	 * defined, nullptr is returned. A returned pointer indicates a name
	 * collision.
	 */
	D* Lookup(I name);
};
