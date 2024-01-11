#pragma once

#include <forward_list>
#include <map>
#include <ostream>
#include <string_view>

// Forward declarations to avoid use of cyclic includes.
struct Function;
struct SyntaxTreeNode;
struct Type;


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
};


// Wraps the buffer that contains a TU's source, including its length.
struct TUBuffer
{
	char* _buf;		// A pointer to a TU's source buffer.
	size_t _size;	// The TU's size.
};


/**
 * @brief Prints the line of referenced token to the specified output stream.
 * 
 * @param os The output stream to write the line and highlighting to.
 * @param src The source file buffer containing the line.
 * @param info The token to highlight from the line.
 */
void HighlightError(std::ostream& os, TUBuffer& src, TokenInfo& info);


// A data structure to store program symbols in a scope hierarchy.
struct ScopeStack
{
protected:
	// A stack of scopes that map identifiers to AST definitions.
	std::forward_list<std::map<std::string_view, SyntaxTreeNode*>> _stackMap;

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
	SyntaxTreeNode* Define(std::string_view name, SyntaxTreeNode* node);

	/**
	 * @brief Returns a pointer to the AST node referred to by the specfied
	 * symbol, if it exists in any scope. The most recently defined instance of
	 * the symbol will be returned.
	 * 
	 * @param name The symbol's identifier (name).
	 * @return SyntaxTreeNode* A pointer to the AST node of the already defined
	 * symbol. If no such symbol is define, nullptr is returned.
	 */
	SyntaxTreeNode* Lookup(std::string_view name);
};


struct ValidateData
{
	// The TUBuffer for the source file expressing this AST.
	TUBuffer& _src;
	// Tree stack containing the chain of breakable predecessor AST nodes.
	std::forward_list<SyntaxTreeNode*>& _bs;
	// The current function definition.
	Function* _curFunc;
};
