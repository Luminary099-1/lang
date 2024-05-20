#pragma once

#include <forward_list>
#include <map>
#include <memory>
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


// Wraps a buffer to contain a TU's source.
struct TU
{
	char* _buf;		// Pointer to a TU's source buffer.
	size_t _size;	// Size of the buffer referred to by _buf.

	/**
	 * @brief Construct a new TU object.
	 * 
	 * @param size Size of the stored translation unit in bytes.
	 */
	TU(size_t size);

	// Destroy the TU object,
	~TU();

	/**
	 * @brief Creates a new TU and populates the buffer with the contents of a
	 * file.
	 * 
	 * @param src_path Path to the file that will populate the buffer.
	 * @return TU instance containing the contents of the file.
	 * @throws std::runtime_error If the specified file could not be opened.
	 */
	static std::unique_ptr<TU> LoadFromFile(char* src_path);

	/**
	 * @brief Outputs an error message that contains an erroneous line of code
	 * and highlights which token causes the error.
	 * 
	 * @param os Stream to write the message to.
	 * @param info Position information of the offending token.
	 */
	void HighlightError(std::ostream& os, TokenInfo& info);
};


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
