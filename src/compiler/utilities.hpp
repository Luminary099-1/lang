#pragma once

#include <forward_list>
#include <map>
#include <ostream>
#include <string_view>
#include <vector>


// Stores the information necessary to locate a token in the TU buffer.
struct TokenInfo
{
	int _row {1};		// Row at the start of the match.
	int _endRow {1};	// Row after the end of the match.
	int _col {1};		// Column at the start of the match.
	int _endCol {1};	// Column after the end of the match.
	size_t _off {0};	// Offset of the first character of the match.
	size_t _endOff {0};	// Offset after the last character of the match.

	/**
	 * @brief Convenience function to copy in the fields defined by this struct
	 * from other instances of TokenInfo.
	 * 
	 * @param info An instance of TokenInfo whose values are to be copied to
	 * this.
	 */
	void SetSymbolInfo(TokenInfo info);

	/**
	 * @brief Copies the fields defined by this struct from two other instance
	 * such that the interval represented spans the intervals of both inputs.
	 * 
	 * @param i1 An instance of TokenInfo.
	 * @param i2 An instance of TokenInfo.
	 */
	void SetMergedInfo(TokenInfo& i1, TokenInfo& i2);
};


// Wraps the buffer that contains a TU's source, including its length.
struct TUBuffer
{
	char* _buf;		// A pointer to a TU's source buffer.
	size_t _size;	// The TU's size.
	bool _failed;	// Indicates a semantic error was detected during scanning.
};


/**
 * @brief Prints the line of referenced token to the specified output stream.
 * 
 * @param os The output stream to write the line and highlighting to.
 * @param src The source file buffer containing the line.
 * @param info The token to highlight from the line.
 */
void HighlightError(std::ostream& os, TUBuffer& src, TokenInfo& info);


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
